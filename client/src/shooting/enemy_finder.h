#ifndef ARCANE_CLIENT_SRC_SHOOTING_ENEMY_FINDER_H
#define ARCANE_CLIENT_SRC_SHOOTING_ENEMY_FINDER_H

#include <chrono>
#include <list>
#include <map>
#include <string>

// plugin sdk
#include <CPed.h>
#include <CVector.h>
#include <CVector2D.h>

#include <configuration/configuration.hpp>

namespace modification::client::shooting {
class enemy_finder {
 public:
  struct target {
    CPed* ped{};
    CVector position{};
    CVector2D angle{};
    float difference{};
  };

  struct settings {
    float &max_angle_in_degrees, &min_distance;
    int& delay_between_target_changes;

    // main
    bool &divide_angle_by_distance, &hold_target, &check_for_obstacles, &check_for_distance,
        &ignore_the_dead, &ignore_same_group, &ignore_friendly_nicknames, &ignore_same_color;

    // bones
    bool &head, &neck, &right_shoulder, &left_shoulder, &right_elbow, &left_elbow, &stomach,
        &right_knee, &left_knee;

    std::vector<configuration::model_group>& model_groups;
    std::vector<std::string>& friendly_nicknames;

    bool& use_target_range_instead_of_weapons;
  };

  enemy_finder(const settings& settings) : settings_{settings} {}

  target get();
  target produce_target(CPed* const ped) const;

 private:
  settings settings_;
  target enemy_{};
  std::chrono::steady_clock::time_point target_change_time_{};
  CPed* previous_enemy_ped_{};
};
}  // namespace modification::client::shooting

#endif  // ARCANE_CLIENT_SRC_SHOOTING_ENEMY_FINDER_H
