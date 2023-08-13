#ifndef ARCANE_CLIENT_SRC_SHOOTING_ENEMY_FINDER_H
#define ARCANE_CLIENT_SRC_SHOOTING_ENEMY_FINDER_H

#include <chrono>
#include <vector>

#include <psdk_utils/local_vector.h>
#include <arcane_packets/configuration.hpp>

class CPed;

namespace modification::client::shooting {
class enemy_finder {
 public:
  struct target {
    CPed* ped;
    psdk_utils::local_vector position;
    psdk_utils::local_vector angle;
    float difference;
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

    std::vector<packets::configuration::model_group>& model_groups;
    std::vector<std::string>& friendly_nicknames;

    bool& use_target_range_instead_of_weapons;
  };

 public:
  enemy_finder(const settings& settings);

  target get();
  target produce_target(CPed* ped) const;

 private:
  using clock = std::chrono::steady_clock;

 private:
  settings settings_;
  target enemy_;
  clock::time_point target_change_time_;
  CPed* previous_enemy_ped_;
};
}  // namespace modification::client::shooting

#endif  // ARCANE_CLIENT_SRC_SHOOTING_ENEMY_FINDER_H
