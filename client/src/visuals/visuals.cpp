#include "visuals.h"

#include <imgui.h>
#include <psdk_utils/psdk_utils.h>
#include <samp_utils/samp_utils.h>

#include <cmrc/cmrc.hpp>
#include <format>

#include "../main.h"
#include "ePedBones.h"

using namespace modification::client::visuals;

CMRC_DECLARE(fonts);

inline CVector2D operator+(const CVector2D& a, const CVector2D& b) {
  return {a.x + b.x, a.y + b.y};
}

template <class T>
inline CVector2D operator-(const CVector2D& a, const T& b) {
  return {a.x - b.x, a.y - b.y};
}

void visuals::initialize() {
  const auto& io = ImGui::GetIO();
  const auto fs = cmrc::fonts::get_filesystem();

  const void* const jetbrains_mono_bold_font{fs.open("rc/fonts/jetbrains_mono_bold.ttf").begin()};
  const void* const geforce_bold_font{fs.open("rc/fonts/geforce_bold.ttf").begin()};

  nametags_font_.size = 20.0f;
  nametags_font_.ptr = io.Fonts->AddFontFromMemoryTTF(const_cast<void*>(jetbrains_mono_bold_font),
                                                      nametags_font_.size, nametags_font_.size);

  framerate_font_.size = 36.0f;
  framerate_font_.ptr = io.Fonts->AddFontFromMemoryTTF(const_cast<void*>(geforce_bold_font),
                                                       framerate_font_.size, framerate_font_.size);
}

void visuals::process() {
  for (const auto& i : psdk_utils::peds_around()) {
    auto [id, color] = samp_utils::execute([i](auto version) -> std::tuple<sampapi::ID, ImU32> {
      using samp = samp_utils::invoke<decltype(version)>;
      const auto id = samp::get_id_by_ped(i);
      return {id, samp::get_color_as_argb(id)};
    });

    if (id == samp_utils::invalid_id()) continue;
    if (!color) color = 0xFF00FF00;

    const auto vec_color = ImGui::ColorConvertU32ToFloat4(color);
    color = ImGui::ColorConvertFloat4ToU32({vec_color.z, vec_color.y, vec_color.x, 1.0f});

    if (settings.flat_box) flat_box(i, color);
    if (settings.three_dimensional_box) three_dimensional_box(i, color);
    if (settings.line) line(i, color);
    if (!settings.name) name(i, color);
    if (settings.bones) bones(i, color);
    if (settings.fov) fov(i, vector_aimbot_fov_color, silent_aimbot_fov_color);
  }

  if (settings.hide_samp_nametag) {
    samp_utils::execute([&](auto version) {
      samp_utils::invoke<decltype(version)>::set_nametag_visibility_status(
          !settings.hide_samp_nametag);
    });
  }

  if (!settings.framerate) framerate();
}

void visuals::flat_box(CPed* const ped, const ImU32 color) {
  draw_flat_box_in_space(ped->GetPosition(), ped->m_fCurrentRotation, 0.4f, 1.0f, line_width_,
                         color);
}

void visuals::three_dimensional_box(CPed* const ped, const ImU32 color) {
  const auto& ped_pos = ped->GetPosition();

  constexpr auto pi = psdk_utils::math::pi();
  const float ped_rot{ped->m_fCurrentRotation}, radius{0.3f}, height{1.0f};

  draw_flat_box_in_space(get_point_on_circumference(ped_pos, ped_rot + pi / 2.0f, radius), ped_rot,
                         radius, height, line_width_, color);

  draw_flat_box_in_space(get_point_on_circumference(ped_pos, ped_rot + 3.0f * pi / 2.0f, radius),
                         ped_rot, radius, height, line_width_, color);

  draw_flat_box_in_space(get_point_on_circumference(ped_pos, ped_rot + pi, radius),
                         ped_rot + pi / 2.0f, radius, height, line_width_, color);

  draw_flat_box_in_space(get_point_on_circumference(ped_pos, ped_rot, radius), ped_rot + pi / 2.0f,
                         radius, height, line_width_, color);
}

void visuals::line(CPed* const ped, const ImU32 color) {
  namespace psdk = psdk_utils;

  const auto ped_pos = psdk::calc_screen_coors(ped->GetPosition()),
             player_pos = psdk::calc_screen_coors(psdk::player()->GetPosition());

  if (ped_pos.z != 0.0f || player_pos.z != 0.0f) return;

  draw()->AddLine(imvec2(ped_pos), imvec2(player_pos), color, line_width_);
}

void visuals::name(CPed* const ped, const ImU32 color, const bar health_bar, const bar armor_bar) {
  namespace psdk = psdk_utils;

  const auto [nametag_draw_distance, id, name, health, armor] = samp_utils::execute(
      [ped](
          auto version) -> std::tuple<float, sampapi::ID, std::string, std::uint8_t, std::uint8_t> {
        using samp = samp_utils::invoke<decltype(version)>;
        const auto id = samp::get_id_by_ped(ped);
        return {samp::get_nametag_draw_distance(), id, samp::get_name(id), samp::get_health(id),
                samp::get_armor(id)};
      });

  if (id == samp_utils::invalid_id()) return;

  const auto ped_head_pos = psdk::bone_position(ped, BONE_HEAD);
  const auto dist_to_ped_head =
      psdk::math::distance_between_points(ped_head_pos, psdk::player()->GetPosition());

  if (dist_to_ped_head < nametag_draw_distance &&
      psdk::world::is_line_of_sight_clear(psdk::get_camera().GetPosition(), ped_head_pos, true,
                                          false, false, true, true, true, true)) {
    return;
  }

  const auto origin_point_screen_pos = [dist_to_ped_head, ped_head_pos{ped_head_pos}]() mutable {
    const float part_of_dist_for_comfortable_dynamic_offset{0.047f}, static_offset_from_head{0.4f};

    ped_head_pos.z +=
        dist_to_ped_head * part_of_dist_for_comfortable_dynamic_offset + static_offset_from_head;

    const auto result = psdk::calc_screen_coors(ped_head_pos);
    return result;
  }();

  if (origin_point_screen_pos.z != 0.0f) return;

  const auto text = std::format("{} ({})", name, id);

  const auto text_size = [&]() {
    ImGui::PushFont(nametags_font_.ptr);
    const auto result = ImGui::CalcTextSize(text.c_str());
    ImGui::PopFont();
    return result;
  }();

  const ImVec2 nametag_screen_pos{origin_point_screen_pos.x - text_size.x / 2.0f,
                                  origin_point_screen_pos.y};

  draw()->AddText(nametags_font_.ptr, nametags_font_.size, nametag_screen_pos, color, text.c_str());

  CVector2D bar_screen_pos{origin_point_screen_pos.x,
                           origin_point_screen_pos.y + text_size.y + 2.0f};

  if (armor) {
    draw_progress_bar(armor_bar, bar_screen_pos, armor, 100.0f);
    bar_screen_pos.y += armor_bar.size.y + armor_bar.border_width + 4.0f;
  }

  draw_progress_bar(health_bar, bar_screen_pos, health, 100.0f);
}

void visuals::bones(CPed* const ped, const ImU32 color) {
  namespace psdk = psdk_utils;

  static std::multimap<ePedBones, ePedBones> lines_between_bones{
      {BONE_HEAD2, BONE_HEAD},
      {BONE_HEAD, BONE_NECK},
      {BONE_NECK, BONE_UPPERTORSO},
      {BONE_UPPERTORSO, BONE_SPINE1},
      {BONE_SPINE1, BONE_PELVIS},
      {BONE_PELVIS, BONE_PELVIS1},
      {BONE_PELVIS1, BONE_RIGHTHIP},
      {BONE_PELVIS1, BONE_LEFTHIP},
      {BONE_RIGHTHIP, BONE_RIGHTKNEE},
      {BONE_RIGHTKNEE, BONE_RIGHTANKLE},
      {BONE_RIGHTANKLE, BONE_RIGHTFOOT},
      {BONE_LEFTHIP, BONE_LEFTKNEE},
      {BONE_LEFTKNEE, BONE_LEFTANKLE},
      {BONE_LEFTANKLE, BONE_LEFTFOOT},
      {BONE_UPPERTORSO, BONE_RIGHTSHOULDER},
      {BONE_RIGHTSHOULDER, BONE_RIGHTELBOW},
      {BONE_RIGHTELBOW, BONE_RIGHTWRIST},
      {BONE_UPPERTORSO, BONE_LEFTSHOULDER},
      {BONE_LEFTSHOULDER, BONE_LEFTELBOW},
      {BONE_LEFTELBOW, BONE_LEFTWRIST}};

  for (const auto& i : lines_between_bones) {
    const auto first_bone_screen_pos = psdk::calc_screen_coors(psdk::bone_position(ped, i.first)),
               second_bone_screen_pos = psdk::calc_screen_coors(psdk::bone_position(ped, i.second));
    if (first_bone_screen_pos.z != 0.0f || second_bone_screen_pos.z != 0.0f) continue;
    draw()->AddLine(imvec2(first_bone_screen_pos), imvec2(second_bone_screen_pos), color,
                    line_width_);
  }
}

void visuals::fov(CPed* const ped, const ImU32 vector_aimbot_color,
                  const ImU32 silent_aimbot_color) {
  namespace psdk = psdk_utils;

  const auto weapon_mode = psdk::weapon::get_mode(psdk::weapon_in_hand());
  if (weapon_mode == psdk::weapon::mode::unknown) return;

  auto draw_fov = [this, ped](const shooting::enemy_finder::settings& settings, const ImU32 color) {
    if (settings.ignore_the_dead) {
      if (samp_utils::get_version() != samp_utils::version::unknown) {
        const auto is_alive = samp_utils::execute([&](auto version) {
          using samp = samp_utils::invoke<decltype(version)>;
          const auto id = samp::get_id_by_ped(ped);
          return samp::is_player_alive(id) && !samp::is_player_afk(id);
        });

        if (!is_alive) return;
      } else {
        if (!ped->IsAlive()) return;
      }
    }

    const auto nearest_bone = psdk::find_bone_making_minimum_angle_with_camera(
        ped, settings.check_for_obstacles, settings.check_for_distance,
        settings.divide_angle_by_distance, 1000.0f, settings.min_distance,
        settings.use_target_range_instead_of_weapons, settings.head, settings.neck,
        settings.right_shoulder, settings.left_shoulder, settings.right_elbow, settings.left_elbow,
        settings.stomach, settings.right_knee, settings.left_knee);

    if (!nearest_bone.existing) return;

    const auto& cam_pos = psdk::get_camera().GetPosition();
    const auto distance = psdk::math::distance_between_points(cam_pos, nearest_bone.world_position);
    const auto angle =
        psdk::math::angle_between_points(cam_pos, nearest_bone.world_position).x - psdk::math::pi();

    const float max_angle{[&]() {
      float angle{psdk::math::deg2rad(settings.max_angle_in_degrees)};
      if (settings.divide_angle_by_distance) {
        angle /= distance;
      }
      return angle;
    }()};

    const auto bone_point_screen_pos =
                   psdk::calc_screen_coors(get_point_on_circumference(cam_pos, angle, distance)),
               max_angle_point_screen_pos = psdk::calc_screen_coors(
                   get_point_on_circumference(cam_pos, angle - max_angle, distance));

    if (bone_point_screen_pos.z != 0.0f || max_angle_point_screen_pos.z != 0.0f) return;

    const auto nearest_bone_screen_pos = psdk::calc_screen_coors(nearest_bone.world_position);
    if (nearest_bone_screen_pos.z != 0.0f) return;

    const auto radius =
        psdk::math::distance_between_points(bone_point_screen_pos, max_angle_point_screen_pos);
    draw()->AddCircle(imvec2(nearest_bone_screen_pos), radius, color, 0, line_width_);
  };

  const auto [vector_aimbot_enabled, vector_aimbot_settings] =
      main::instance().vector_aimbot_finder_settings(weapon_mode);

  const auto [silent_aimbot_enabled, silent_aimbot_settings] =
      main::instance().silent_aimbot_finder_settings(weapon_mode);

  if (vector_aimbot_enabled) draw_fov(vector_aimbot_settings, vector_aimbot_color);
  if (silent_aimbot_enabled) draw_fov(silent_aimbot_settings, silent_aimbot_color);
}

void visuals::framerate(const ImU32 color) {
  using namespace std::chrono;

  const auto now = steady_clock::now();
  if (duration_cast<seconds>(now - framerate_update_time_) >= 1s) {
    framerate_ = ImGui::GetIO().Framerate;
    framerate_update_time_ = now;
  }

  const auto text = std::to_string(static_cast<int>(framerate_));
  ImGui::PushFont(framerate_font_.ptr);
  const auto text_size = ImGui::CalcTextSize(text.c_str());
  ImGui::PopFont();

  const auto resolution = psdk_utils::resolution();
  auto position = resolution - text_size;
  position.x -= 5.0f;

  draw()->AddText(framerate_font_.ptr, framerate_font_.size, imvec2(position), color, text.c_str());
}

void visuals::draw_progress_bar(const bar bar, CVector2D position, const float progress,
                                const float max) {
  position.x -= bar.size.x / 2.0f + bar.border_width;

  draw()->AddRectFilled(imvec2(position),
                        imvec2(position + CVector2D{bar.size.x + bar.border_width * 2.0f,
                                                    bar.size.y + bar.border_width * 2.0f}),
                        bar.background_color, bar.rounding);

  position += {bar.border_width, bar.border_width};

  draw()->AddRectFilled(
      imvec2(position),
      imvec2(position + CVector2D{std::min(progress / (max / bar.size.x), bar.size.x), bar.size.y}),
      bar.progress_color, progress > 0.0f ? bar.rounding : 0.0f);
}

void visuals::draw_flat_box_in_space(const CVector& center, const float angle, const float radius,
                                     const float height, const float line_width_,
                                     const ImU32 color) {
  const CVector2D right{get_point_on_circumference(center, angle, radius)},
      left{get_point_on_circumference(center, angle + psdk_utils::math::pi(), radius)};

  std::array<CVector, 4> points{CVector{right.x, right.y, center.z + height},
                                {right.x, right.y, center.z - height},
                                {left.x, left.y, center.z - height},
                                {left.x, left.y, center.z + height}};

  for (int i{0}; i < points.size(); ++i) {
    auto& point = points[i];
    point = psdk_utils::calc_screen_coors(point);
    if (point.z != 0.0f) return;
    if (i > 0) {
      draw()->AddLine(imvec2(points[i - 1]), imvec2(point), color, line_width_);
    }
  }

  draw()->AddLine(imvec2(points[0]), imvec2(points[points.size() - 1]), color, line_width_);
}

CVector visuals::get_point_on_circumference(const CVector& center, const float angle,
                                            const float radius) {
  return center + CVector{radius * std::cos(angle), radius * std::sin(angle), 0.0f};
}
