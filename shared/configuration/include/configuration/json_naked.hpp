#ifndef ARCANE_JSON_NAKED_HPP
#define ARCANE_JSON_NAKED_HPP

#include "configuration.hpp"
#include "data_representation/data_representation.h"

namespace modification {
namespace configuration {
// rpc
inline void to_json(nlohmann::json& j, const rpc& p) {
  auto vmp = [](const char* const string) {
    return data_representation::sha256_string(string);
  };

  std::string text[]{
#include "keys/rpc.hpp"
  };

  j = nlohmann::json{{text[0], p.name},
                     {text[1], p.password},
                     {text[2], p.hwid},
                     {text[3], p.command}};
}

inline void from_json(const nlohmann::json& j, rpc& p) {
  auto vmp = [](const char* const string) {
    return data_representation::sha256_string(string);
  };

  std::string text[]{
#include "keys/rpc.hpp"
  };

  j.at(text[0]).get_to(p.name);
  j.at(text[1]).get_to(p.password);
  j.at(text[2]).get_to(p.hwid);
  j.at(text[3]).get_to(p.command);
}

// model_group
inline void to_json(nlohmann::json& j, const model_group& p) {
  auto vmp = [](const char* const string) {
    return data_representation::sha256_string(string);
  };

  std::string text[]{
#include "keys/model_group.hpp"
  };

  j = nlohmann::json{{text[0], p.id},
                     {text[1], p.name},
                     {text[2], p.models},
                     {text[3], p.active}};
}

inline void from_json(const nlohmann::json& j, model_group& p) {
  auto vmp = [](const char* const string) {
    return data_representation::sha256_string(string);
  };

  std::string text[]{
#include "keys/model_group.hpp"
  };

  j.at(text[0]).get_to(p.id);
  j.at(text[1]).get_to(p.name);
  j.at(text[2]).get_to(p.models);
  j.at(text[3]).get_to(p.active);
}

// vector_aimbot
inline void to_json(nlohmann::json& j, const vector_aimbot& p) {
  auto vmp = [](const char* const string) {
    return data_representation::sha256_string(string);
  };

  std::string text[]{
#include "keys/vector_aimbot.hpp"
  };

  j = nlohmann::json{{text[0], p.enable},
                     {text[1], p.smooth},
                     {text[2], p.max_angle_in_degrees},
                     {text[3], p.divide_angle_by_distance},
                     {text[4], p.hold_target},
                     {text[5], p.check_for_obstacles},
                     {text[6], p.check_for_distance},
                     {text[7], p.ignore_the_dead},
                     {text[8], p.ignore_same_group},
                     {text[9], p.ignore_friendly_nicknames},
                     {text[10], p.ignore_same_color},
                     {text[11], p.aim_only_when_shooting},
                     {text[12], p.head},
                     {text[13], p.neck},
                     {text[14], p.right_shoulder},
                     {text[15], p.left_shoulder},
                     {text[16], p.right_elbow},
                     {text[17], p.left_elbow},
                     {text[18], p.stomach},
                     {text[19], p.right_knee},
                     {text[20], p.left_knee},
                     {text[21], p.model_groups},
                     {text[22], p.friendly_nicknames},
                     {text[23], p.min_distance},
                     {text[24], p.delay_between_target_changes},
                     {text[25], p.do_not_change_z_angle}};
}

inline void from_json(const nlohmann::json& j, vector_aimbot& p) {
  auto vmp = [](const char* const string) {
    return data_representation::sha256_string(string);
  };

  std::string text[]{
#include "keys/vector_aimbot.hpp"
  };

  j.at(text[0]).get_to(p.enable);
  j.at(text[1]).get_to(p.smooth);
  j.at(text[2]).get_to(p.max_angle_in_degrees);
  j.at(text[3]).get_to(p.divide_angle_by_distance);
  j.at(text[4]).get_to(p.hold_target);
  j.at(text[5]).get_to(p.check_for_obstacles);
  j.at(text[6]).get_to(p.check_for_distance);
  j.at(text[7]).get_to(p.ignore_the_dead);
  j.at(text[8]).get_to(p.ignore_same_group);
  j.at(text[9]).get_to(p.ignore_friendly_nicknames);
  j.at(text[10]).get_to(p.ignore_same_color);
  j.at(text[11]).get_to(p.aim_only_when_shooting);
  j.at(text[12]).get_to(p.head);
  j.at(text[13]).get_to(p.neck);
  j.at(text[14]).get_to(p.right_shoulder);
  j.at(text[15]).get_to(p.left_shoulder);
  j.at(text[16]).get_to(p.right_elbow);
  j.at(text[17]).get_to(p.left_elbow);
  j.at(text[18]).get_to(p.stomach);
  j.at(text[19]).get_to(p.right_knee);
  j.at(text[20]).get_to(p.left_knee);
  j.at(text[21]).get_to(p.model_groups);
  j.at(text[22]).get_to(p.friendly_nicknames);
  j.at(text[23]).get_to(p.min_distance);
  j.at(text[24]).get_to(p.delay_between_target_changes);
  j.at(text[25]).get_to(p.do_not_change_z_angle);
}

// silent_aimbot
inline void to_json(nlohmann::json& j, const silent_aimbot& p) {
  auto vmp = [](const char* const string) {
    return data_representation::sha256_string(string);
  };

  std::string text[]{
#include "keys/silent_aimbot.hpp"
  };

  j = nlohmann::json{{text[0], p.enable},
                     {text[1], p.hit_rate},
                     {text[2], p.max_angle_in_degrees},
                     {text[3], p.divide_angle_by_distance},
                     {text[4], p.hold_target},
                     {text[5], p.check_for_obstacles},
                     {text[6], p.check_for_distance},
                     {text[7], p.ignore_the_dead},
                     {text[8], p.ignore_same_group},
                     {text[9], p.ignore_friendly_nicknames},
                     {text[10], p.ignore_same_color},
                     {text[11], p.head},
                     {text[12], p.neck},
                     {text[13], p.right_shoulder},
                     {text[14], p.left_shoulder},
                     {text[15], p.right_elbow},
                     {text[16], p.left_elbow},
                     {text[17], p.stomach},
                     {text[18], p.right_knee},
                     {text[19], p.left_knee},
                     {text[20], p.model_groups},
                     {text[21], p.friendly_nicknames},
                     {text[22], p.pull_camera_toward_enemy},
                     {text[23], p.turn_player_toward_enemy},
                     {text[24], p.display_triangle_above_enemy},
                     {text[25], p.increase_hit_rate},
                     {text[26], p.min_distance},
                     {text[27], p.step_to_increase_hit_rate},
                     {text[28], p.delay_between_target_changes}};
}

inline void from_json(const nlohmann::json& j, silent_aimbot& p) {
  auto vmp = [](const char* const string) {
    return data_representation::sha256_string(string).c_str();
  };

  std::string text[]{
#include "keys/silent_aimbot.hpp"
  };

  j.at(text[0]).get_to(p.enable);
  j.at(text[1]).get_to(p.hit_rate);
  j.at(text[2]).get_to(p.max_angle_in_degrees);
  j.at(text[3]).get_to(p.divide_angle_by_distance);
  j.at(text[4]).get_to(p.hold_target);
  j.at(text[5]).get_to(p.check_for_obstacles);
  j.at(text[6]).get_to(p.check_for_distance);
  j.at(text[7]).get_to(p.ignore_the_dead);
  j.at(text[8]).get_to(p.ignore_same_group);
  j.at(text[9]).get_to(p.ignore_friendly_nicknames);
  j.at(text[10]).get_to(p.ignore_same_color);
  j.at(text[11]).get_to(p.head);
  j.at(text[12]).get_to(p.neck);
  j.at(text[13]).get_to(p.right_shoulder);
  j.at(text[14]).get_to(p.left_shoulder);
  j.at(text[15]).get_to(p.right_elbow);
  j.at(text[16]).get_to(p.left_elbow);
  j.at(text[17]).get_to(p.stomach);
  j.at(text[18]).get_to(p.right_knee);
  j.at(text[19]).get_to(p.left_knee);
  j.at(text[20]).get_to(p.model_groups);
  j.at(text[21]).get_to(p.friendly_nicknames);
  j.at(text[22]).get_to(p.pull_camera_toward_enemy);
  j.at(text[23]).get_to(p.turn_player_toward_enemy);
  j.at(text[24]).get_to(p.display_triangle_above_enemy);
  j.at(text[25]).get_to(p.increase_hit_rate);
  j.at(text[26]).get_to(p.min_distance);
  j.at(text[27]).get_to(p.step_to_increase_hit_rate);
  j.at(text[28]).get_to(p.delay_between_target_changes);
}
}  // namespace configuration
}  // namespace modification

#endif  // ARCANE_JSON_NAKED_HPP
