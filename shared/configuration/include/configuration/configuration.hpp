#ifndef ARCANE_CONFIGURATION_HPP
#define ARCANE_CONFIGURATION_HPP

#include <list>
#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace modification {
namespace configuration {
struct rpc {
  std::string name{}, password{}, hwid{}, command{};
};

struct model_group {
  std::uint32_t id{};
  std::string name{};
  std::list<int> models{};
  bool active{false};
};

struct vector_aimbot {
  float smooth{10.0f};
  float max_angle_in_degrees{50.0f};

  bool enable{false};
  bool divide_angle_by_distance{true}, hold_target{true},
      check_for_obstacles{true}, check_for_distance{true},
      ignore_the_dead{true}, ignore_same_group{true},
      ignore_friendly_nicknames{true}, ignore_same_color{false},
      aim_only_when_shooting{true};

  bool head{true}, neck{true}, right_shoulder{true}, left_shoulder{true},
      right_elbow{true}, left_elbow{true}, stomach{true}, right_knee{true},
      left_knee{true};

  std::vector<model_group> model_groups{};
  std::vector<std::string> friendly_nicknames{};

  float min_distance{6.0f};
  int delay_between_target_changes{1000};
  bool do_not_change_z_angle{true};
};

struct silent_aimbot {
  int hit_rate{2};
  float max_angle_in_degrees{35.0f};

  bool enable{false};
  bool divide_angle_by_distance{true}, hold_target{true},
      check_for_obstacles{true}, check_for_distance{true},
      ignore_the_dead{true}, ignore_same_group{true},
      ignore_friendly_nicknames{true}, ignore_same_color{false};

  bool head{true}, neck{true}, right_shoulder{true}, left_shoulder{true},
      right_elbow{true}, left_elbow{true}, stomach{true}, right_knee{true},
      left_knee{true};

  std::vector<model_group> model_groups{};
  std::vector<std::string> friendly_nicknames{};

  bool pull_camera_toward_enemy{true}, turn_player_toward_enemy{false},
      display_triangle_above_enemy{true}, increase_hit_rate{true};

  float min_distance{6.0f}, step_to_increase_hit_rate{12.0f};
  int delay_between_target_changes{1000};

  struct details {
    int missed_hits{1};
  } details;
};

struct auto_cbug {
  int duration_from_attack_to_squat{50}, duration_from_squat_to_attack{50},
      key{'R'};

  bool enable{true}, auto_repeat{true};
};

struct visuals {
  bool flat_box{false}, three_dimensional_box{false}, line{false}, name{false},
      hide_samp_nametag{false}, bones{false}, fov{false}, framerate{false};
};
}  // namespace configuration
}  // namespace modification

#endif  // ARCANE_CONFIGURATION_HPP
