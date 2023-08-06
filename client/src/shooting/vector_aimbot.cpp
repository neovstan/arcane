#include "vector_aimbot.h"

#include <imgui.h>
#include <psdk_utils/psdk_utils.h>

using namespace modification::client::shooting;

vector_aimbot::vector_aimbot()
    : finder_{{psdk_utils::weapon::mode::pistols,
               {get_settings(psdk_utils::weapon::mode::pistols)}},
              {psdk_utils::weapon::mode::shotguns,
               {get_settings(psdk_utils::weapon::mode::shotguns)}},
              {psdk_utils::weapon::mode::semi, {get_settings(psdk_utils::weapon::mode::semi)}},
              {psdk_utils::weapon::mode::assault,
               {get_settings(psdk_utils::weapon::mode::assault)}},
              {psdk_utils::weapon::mode::rifles, {get_settings(psdk_utils::weapon::mode::rifles)}}},
      use_target_range_instead_of_weapons_{} {
}

void vector_aimbot::process() {
  const auto weapon_mode = psdk_utils::weapon::get_mode(psdk_utils::weapon_in_hand());
  if (weapon_mode == psdk_utils::weapon::mode::unknown) return;

  const auto& settings = this->settings.at(weapon_mode);

  if (!settings.enable) return;

  if (settings.aim_only_when_shooting &&
      !psdk_utils::player()->GetPadFromPlayer()->NewState.ButtonCircle) {
    return;
  }

  auto& finder = this->finder_.at(weapon_mode);
  if (const auto enemy = finder.get(); enemy.ped) {
    using namespace psdk_utils;
    auto& cam = get_active_cam();

    const auto smooth = settings.smooth * ImGui::GetIO().Framerate / 100.0f;

    cam.m_fHorizontalAngle += math::deg2rad(
        math::difference(enemy.angle.x() - math::rad2deg(cam.m_fHorizontalAngle)) / smooth);

    if (!settings.do_not_change_z_angle) {
      cam.m_fVerticalAngle += math::deg2rad(
          math::difference(enemy.angle.y() - math::rad2deg(cam.m_fVerticalAngle)) / smooth);
    }
  }
}

enemy_finder::settings vector_aimbot::get_settings(psdk_utils::weapon::mode number) {
  auto& s = settings[number];
  return {s.max_angle_in_degrees,
          s.min_distance,
          s.delay_between_target_changes,
          s.divide_angle_by_distance,
          s.hold_target,
          s.check_for_obstacles,
          s.check_for_distance,
          s.ignore_the_dead,
          s.ignore_same_group,
          s.ignore_friendly_nicknames,
          s.ignore_same_color,
          s.head,
          s.neck,
          s.right_shoulder,
          s.left_shoulder,
          s.right_elbow,
          s.left_elbow,
          s.stomach,
          s.right_knee,
          s.left_knee,
          s.model_groups,
          s.friendly_nicknames,
          use_target_range_instead_of_weapons_};
}
