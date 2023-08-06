#ifndef ARCANE_CONFIGURATION_HPP
#define ARCANE_CONFIGURATION_HPP

#include <list>
#include <map>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace modification {
namespace configuration {
struct rpc {
  std::string name;
  std::string password;
  std::string hwid;
  std::string command;
};

struct model_group {
  std::uint32_t id;
  std::string name;
  std::list<int> models;
  bool active;
};

struct vector_aimbot {
  float smooth{10.0f};
  float max_angle_in_degrees{50.0f};

  bool enable{false};

  bool divide_angle_by_distance{true};
  bool hold_target{true};
  bool check_for_obstacles{true};
  bool check_for_distance{true};
  bool ignore_the_dead{true};
  bool ignore_same_group{true};
  bool ignore_friendly_nicknames{true};
  bool ignore_same_color{false};
  bool aim_only_when_shooting{true};

  bool head{true};
  bool neck{true};
  bool right_shoulder{true};
  bool left_shoulder{true};
  bool right_elbow{true};
  bool left_elbow{true};
  bool stomach{true};
  bool right_knee{true};
  bool left_knee{true};

  std::vector<model_group> model_groups;
  std::vector<std::string> friendly_nicknames;

  float min_distance{6.0f};
  int delay_between_target_changes{1000};
  bool do_not_change_z_angle{true};
};

struct silent_aimbot {
  int hit_rate{2};
  float max_angle_in_degrees{35.0f};

  bool enable{false};

  bool divide_angle_by_distance{true};
  bool hold_target{true};
  bool check_for_obstacles{true};
  bool check_for_distance{true};
  bool ignore_the_dead{true};
  bool ignore_same_group{true};
  bool ignore_friendly_nicknames{true};
  bool ignore_same_color{false};

  bool head{true};
  bool neck{true};
  bool right_shoulder{true};
  bool left_shoulder{true};
  bool right_elbow{true};
  bool left_elbow{true};
  bool stomach{true};
  bool right_knee{true};
  bool left_knee{true};

  std::vector<model_group> model_groups;
  std::vector<std::string> friendly_nicknames;

  bool pull_camera_toward_enemy{true};
  bool turn_player_toward_enemy{false};
  bool display_triangle_above_enemy{true};
  bool increase_hit_rate{true};

  float min_distance{6.0f};
  float step_to_increase_hit_rate{12.0f};
  int delay_between_target_changes{1000};

  struct details {
    int missed_hits{1};
  } details;
};

struct auto_cbug {
  int duration_from_attack_to_squat{50};
  int duration_from_squat_to_attack{50};
  int key{'R'};

  bool enable{false};
  bool auto_repeat{true};
};

struct auto_shot {
  bool enable{false};
};

struct visuals {
  bool flat_box{false};
  bool three_dimensional_box{false};
  bool line{false};
  bool name{false};
  bool hide_samp_nametag{false};
  bool bones{false};
  bool fov{false};
  bool framerate{false};
};

struct actor {
  struct air_walking_data {
    bool enable{false};

    float speed{30.0f};
    float acceleration_time{1.0f};

    struct movement_keys {
      int forward{'W'};
      int backward{'S'};
      int left{'A'};
      int right{'D'};
      int up{0xA0};
      int down{0xA2};
    } keys;
  } air_walking;

  struct anti_stun_data {
    bool enable{false};
    int chance{50};
  } anti_stun;

  struct fast_run_data {
    bool enable{false};
    float speed{2.0f};
  } fast_run;
};
}  // namespace configuration
}  // namespace modification

#endif  // ARCANE_CONFIGURATION_HPP
