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
  return {settings[number].max_angle_in_degrees,
          settings[number].min_distance,
          settings[number].delay_between_target_changes,
          settings[number].divide_angle_by_distance,
          settings[number].hold_target,
          settings[number].check_for_obstacles,
          settings[number].check_for_distance,
          settings[number].ignore_the_dead,
          settings[number].ignore_same_group,
          settings[number].ignore_friendly_nicknames,
          settings[number].ignore_same_color,
          settings[number].head,
          settings[number].neck,
          settings[number].right_shoulder,
          settings[number].left_shoulder,
          settings[number].right_elbow,
          settings[number].left_elbow,
          settings[number].stomach,
          settings[number].right_knee,
          settings[number].left_knee,
          settings[number].model_groups,
          settings[number].friendly_nicknames,
          use_target_range_instead_of_weapons_};
}
