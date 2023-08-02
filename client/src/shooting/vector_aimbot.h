#ifndef ARCANE_CLIENT_SRC_SHOOTING_VECTOR_AIMBOT_H
#define ARCANE_CLIENT_SRC_SHOOTING_VECTOR_AIMBOT_H

#include <psdk_utils/psdk_utils.h>

#include <configuration/configuration.hpp>
#include <nlohmann/json.hpp>
#include <unordered_map>

#include "enemy_finder.h"

namespace modification::client::shooting {
class vector_aimbot {
 public:
  void process();

  std::unordered_map<psdk_utils::weapon::mode, configuration::vector_aimbot> settings{};

  enemy_finder::settings get_settings(const psdk_utils::weapon::mode number) {
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

 private:
  std::unordered_map<psdk_utils::weapon::mode, enemy_finder> finder_{
      {psdk_utils::weapon::mode::pistols, {get_settings(psdk_utils::weapon::mode::pistols)}},
      {psdk_utils::weapon::mode::shotguns, {get_settings(psdk_utils::weapon::mode::shotguns)}},
      {psdk_utils::weapon::mode::semi, {get_settings(psdk_utils::weapon::mode::semi)}},
      {psdk_utils::weapon::mode::assault, {get_settings(psdk_utils::weapon::mode::assault)}},
      {psdk_utils::weapon::mode::rifles, {get_settings(psdk_utils::weapon::mode::rifles)}}};

  bool use_target_range_instead_of_weapons_{false};
};

}  // namespace modification::client::shooting

#endif  // ARCANE_CLIENT_SRC_SHOOTING_VECTOR_AIMBOT_H
