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

visuals::visuals::visuals()
    : line_width_{1.5f},
      vector_aimbot_fov_color_{0.09f, 0.92f, 0.59f, 1.0f},
      silent_aimbot_fov_color_{0.09f, 0.92f, 0.88f, 1.0f},
      nametags_font_{},
      framerate_font_{},
      framerate_{},
      framerate_update_time_{} {
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
    if (settings.name) name(i, color);
    if (settings.bones) bones(i, color);
    if (settings.fov) fov(i, vector_aimbot_fov_color_, silent_aimbot_fov_color_);
  }

  if (settings.hide_samp_nametag) {
    samp_utils::execute([&](auto version) {
      samp_utils::invoke<decltype(version)>::set_nametag_visibility_status(
          !settings.hide_samp_nametag);
    });
  }

  if (settings.framerate) framerate();
}

void visuals::flat_box(CPed* ped, ImColor color) {
  draw_flat_box_in_space(ped->GetPosition(), psdk_utils::math::rad2deg(ped->m_fCurrentRotation),
                         0.4f, 1.0f, line_width_, color);
}

void visuals::three_dimensional_box(CPed* ped, ImColor color) {
  const psdk_utils::local_vector& ped_pos{ped->GetPosition()};

  constexpr auto pi = psdk_utils::math::pi();
  const auto ped_rot = psdk_utils::math::rad2deg(ped->m_fCurrentRotation), radius = 0.3f,
             height = 1.0f;

  draw_flat_box_in_space(ped_pos.transit(psdk_utils::polar_vector{ped_rot + 90.0f, radius}),
                         ped_rot, radius, height, line_width_, color);

  draw_flat_box_in_space(ped_pos.transit(psdk_utils::polar_vector{ped_rot + 270.0f, radius}),
                         ped_rot, radius, height, line_width_, color);

  draw_flat_box_in_space(ped_pos.transit(psdk_utils::polar_vector{ped_rot + 180.0f, radius}),
                         ped_rot + 90.0f, radius, height, line_width_, color);

  draw_flat_box_in_space(ped_pos.transit(psdk_utils::polar_vector{ped_rot, radius}),
                         ped_rot + 90.0f, radius, height, line_width_, color);
}

void visuals::line(CPed* ped, ImColor color) {
  namespace psdk = psdk_utils;

  const auto ped_pos = psdk::local_vector{ped->GetPosition()}.to_screen(),
             player_pos = psdk::local_vector{psdk::player()->GetPosition()}.to_screen();

  if (ped_pos.z() || player_pos.z()) return;

  draw()->AddLine(imvec2(ped_pos), imvec2(player_pos), color, line_width_);
}

void visuals::name(CPed* ped, ImColor color, bar health_bar, bar armor_bar) {
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
      (ped_head_pos - psdk::local_vector{psdk::player()->GetPosition()}).r();

  if (dist_to_ped_head < nametag_draw_distance &&
      psdk::world::is_line_of_sight_clear(psdk::get_camera().GetPosition(), ped_head_pos, true,
                                          false, false, true, true, true, true)) {
    return;
  }

  const auto origin_point_screen_pos = [dist_to_ped_head, ped_head_pos{ped_head_pos}]() mutable {
    const float part_of_dist_for_comfortable_dynamic_offset{0.047f}, static_offset_from_head{0.4f};

    ped_head_pos.z() +=
        dist_to_ped_head * part_of_dist_for_comfortable_dynamic_offset + static_offset_from_head;

    return ped_head_pos.to_screen();
  }();

  if (origin_point_screen_pos.z() != 0.0f) return;

  const auto text = std::format("{} ({})", name, id);

  const auto text_size = [&]() {
    ImGui::PushFont(nametags_font_.ptr);
    const auto result = ImGui::CalcTextSize(text.c_str());
    ImGui::PopFont();
    return result;
  }();

  const ImVec2 nametag_screen_pos{origin_point_screen_pos.x() - text_size.x / 2.0f,
                                  origin_point_screen_pos.y()};

  draw()->AddText(nametags_font_.ptr, nametags_font_.size, nametag_screen_pos, color, text.c_str());

  psdk_utils::local_vector bar_screen_pos{origin_point_screen_pos.x(),
                                          origin_point_screen_pos.y() + text_size.y + 2.0f, 0.0f};

  if (armor) {
    draw_progress_bar(armor_bar, bar_screen_pos, armor, 100.0f);
    bar_screen_pos.y() += armor_bar.size.y() + armor_bar.border_width + 4.0f;
  }

  draw_progress_bar(health_bar, bar_screen_pos, health, 100.0f);
}

void visuals::bones(CPed* ped, ImColor color) {
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
    const auto first_bone_screen_pos = psdk::bone_position(ped, i.first).to_screen(),
               second_bone_screen_pos = psdk::bone_position(ped, i.second).to_screen();
    if (first_bone_screen_pos.z() || second_bone_screen_pos.z()) continue;
    draw()->AddLine(imvec2(first_bone_screen_pos), imvec2(second_bone_screen_pos), color,
                    line_width_);
  }
}

void visuals::fov(CPed* ped, ImColor vector_aimbot_color, ImColor silent_aimbot_color) {
  namespace psdk = psdk_utils;

  const auto weapon_mode = psdk::weapon::get_mode(psdk::weapon_in_hand());
  if (weapon_mode == psdk::weapon::mode::unknown) return;

  auto draw_fov = [this, ped](const shooting::enemy_finder::settings& settings, ImColor color) {
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

    const psdk::local_vector& cam_pos{psdk::get_camera().GetPosition()};
    const auto difference_vector = cam_pos - nearest_bone.world_position;

    const auto max_angle = [&]() {
      auto angle = settings.max_angle_in_degrees;
      if (settings.divide_angle_by_distance) {
        angle /= difference_vector.r();
      }
      return angle;
    }();

    auto& cam = psdk::get_active_cam();

    const auto p1 = cam_pos.transit(
        psdk::polar_vector_3d{psdk::math::difference(nearest_bone.desired_angle.x() -
                                                     psdk::camera::crosshair_offset().x() - 180.0f),
                              psdk::math::difference(nearest_bone.desired_angle.y() -
                                                     psdk::camera::crosshair_offset().y()),
                              difference_vector.r()});

    const auto p2 = cam_pos.transit(
        psdk::polar_vector_3d{psdk::math::difference(max_angle + nearest_bone.desired_angle.x() -
                                                     psdk::camera::crosshair_offset().x() - 180.0f),
                              psdk::math::difference(nearest_bone.desired_angle.y() -
                                                     psdk::camera::crosshair_offset().y()),
                              difference_vector.r()});

    const auto radius = (p2.to_screen() - p1.to_screen()).r();

    const auto bone_screen_pos = nearest_bone.world_position.to_screen();
    if (bone_screen_pos.z()) return;

    draw()->AddCircle(imvec2(bone_screen_pos), radius, color, 0, line_width_);
  };

  const auto [vector_aimbot_enabled, vector_aimbot_settings] =
      main::instance().vector_aimbot_finder_settings(weapon_mode);

  const auto [silent_aimbot_enabled, silent_aimbot_settings] =
      main::instance().silent_aimbot_finder_settings(weapon_mode);

  if (vector_aimbot_enabled) draw_fov(vector_aimbot_settings, vector_aimbot_color);
  if (silent_aimbot_enabled) draw_fov(silent_aimbot_settings, silent_aimbot_color);
}

void visuals::framerate(ImColor color) {
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
  auto position = resolution - psdk_utils::local_vector{text_size.x, text_size.y, 0.0f};
  position.x() -= 5.0f;

  draw()->AddText(framerate_font_.ptr, framerate_font_.size, imvec2(position), color, text.c_str());
}

void visuals::draw_progress_bar(bar bar, psdk_utils::local_vector position, float progress,
                                float max) {
  position.x() -= bar.size.x() / 2.0f + bar.border_width;

  draw()->AddRectFilled(
      imvec2(position),
      imvec2(position + psdk_utils::local_vector{bar.size.x() + bar.border_width * 2.0f,
                                                 bar.size.y() + bar.border_width * 2.0f, 0.0f}),
      bar.background_color, bar.rounding);

  position += {bar.border_width, bar.border_width, 0.0f};

  draw()->AddRectFilled(
      imvec2(position),
      imvec2(position +
             psdk_utils::local_vector{std::min(progress / (max / bar.size.x()), bar.size.x()),
                                      bar.size.y(), 0.0f}),
      bar.progress_color, progress > 0.0f ? bar.rounding : 0.0f);
}

void visuals::draw_flat_box_in_space(const psdk_utils::local_vector& center, float angle,
                                     float radius, float height, float line_width_, ImColor color) {
  const auto right = center.transit(psdk_utils::polar_vector{angle, radius}),
             left = center.transit(psdk_utils::polar_vector{angle + 180.0f, radius});

  std::array<psdk_utils::local_vector, 4> points{
      psdk_utils::local_vector{right.x(), right.y(), center.z() + height},
      {right.x(), right.y(), center.z() - height},
      {left.x(), left.y(), center.z() - height},
      {left.x(), left.y(), center.z() + height}};

  for (auto i = 0; i < points.size(); ++i) {
    auto& point = points[i];
    point = point.to_screen();
    if (point.z()) return;
    if (i > 0) {
      draw()->AddLine(imvec2(points.at(i - 1)), imvec2(point), color, line_width_);
    }
  }

  draw()->AddLine(imvec2(points.at(0)), imvec2(points.at(points.size() - 1)), color, line_width_);
}

ImDrawList* visuals::draw() {
  return ImGui::GetForegroundDrawList();
}

ImVec2 visuals::imvec2(const psdk_utils::local_vector& in) {
  return ImVec2{in.x(), in.y()};
}
