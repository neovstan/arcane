#ifndef ARCANE_JSON_ENCRYPTED_HPP
#define ARCANE_JSON_ENCRYPTED_HPP

#include <VMProtectSDK.h>

#include "configuration.hpp"
#include "data_representation/data_representation.h"

namespace modification {
namespace configuration {
// rpc
inline void to_json(nlohmann::json& j, const rpc& p) {
  auto vmp = &VMProtectDecryptStringA;
  auto sha = [](const char* const str) {
    return data_representation::sha256_string(str);
  };

  const char* const text[]{
#include "keys/rpc.hpp"
  };

  j = nlohmann::json{{sha(text[0]), p.name},
                     {sha(text[1]), p.password},
                     {sha(text[2]), p.hwid},
                     {sha(text[3]), p.command}};

  for (const auto& i : text) {
    VMProtectFreeString(i);
  }
}

inline void from_json(const nlohmann::json& j, rpc& p) {
  auto vmp = &VMProtectDecryptStringA;
  auto sha = [](const char* const str) {
    return data_representation::sha256_string(str);
  };

  const char* const text[]{
#include "keys/rpc.hpp"
  };

  j.at(sha(text[0])).get_to(p.name);
  j.at(sha(text[1])).get_to(p.password);
  j.at(sha(text[2])).get_to(p.hwid);
  j.at(sha(text[3])).get_to(p.command);

  for (const auto& i : text) {
    VMProtectFreeString(i);
  }
}

// model_group
inline void to_json(nlohmann::json& j, const model_group& p) {
  auto vmp = &VMProtectDecryptStringA;
  auto sha = [](const char* const str) {
    return data_representation::sha256_string(str);
  };

  const char* const text[]{
#include "keys/model_group.hpp"
  };

  j = nlohmann::json{{sha(text[0]), p.id},
                     {sha(text[1]), p.name},
                     {sha(text[2]), p.models},
                     {sha(text[3]), p.active}};

  for (const auto& i : text) {
    VMProtectFreeString(i);
  }
}

inline void from_json(const nlohmann::json& j, model_group& p) {
  auto vmp = &VMProtectDecryptStringA;
  auto sha = [](const char* const str) {
    return data_representation::sha256_string(str);
  };

  const char* const text[]{
#include "keys/model_group.hpp"
  };

  j.at(sha(text[0])).get_to(p.id);
  j.at(sha(text[1])).get_to(p.name);
  j.at(sha(text[2])).get_to(p.models);
  j.at(sha(text[3])).get_to(p.active);

  for (const auto& i : text) {
    VMProtectFreeString(i);
  }
}

// vector_aimbot
inline void to_json(nlohmann::json& j, const vector_aimbot& p) {
  auto vmp = &VMProtectDecryptStringA;
  auto sha = [](const char* const str) {
    return data_representation::sha256_string(str);
  };

  const char* const text[]{
#include "keys/vector_aimbot.hpp"
  };

  j = nlohmann::json{{sha(text[0]), p.enable},
                     {sha(text[1]), p.smooth},
                     {sha(text[2]), p.max_angle_in_degrees},
                     {sha(text[3]), p.divide_angle_by_distance},
                     {sha(text[4]), p.hold_target},
                     {sha(text[5]), p.check_for_obstacles},
                     {sha(text[6]), p.check_for_distance},
                     {sha(text[7]), p.ignore_the_dead},
                     {sha(text[8]), p.ignore_same_group},
                     {sha(text[9]), p.ignore_friendly_nicknames},
                     {sha(text[10]), p.ignore_same_color},
                     {sha(text[11]), p.aim_only_when_shooting},
                     {sha(text[12]), p.head},
                     {sha(text[13]), p.neck},
                     {sha(text[14]), p.right_shoulder},
                     {sha(text[15]), p.left_shoulder},
                     {sha(text[16]), p.right_elbow},
                     {sha(text[17]), p.left_elbow},
                     {sha(text[18]), p.stomach},
                     {sha(text[19]), p.right_knee},
                     {sha(text[20]), p.left_knee},
                     {sha(text[21]), p.model_groups},
                     {sha(text[22]), p.friendly_nicknames},
                     {sha(text[23]), p.min_distance},
                     {sha(text[24]), p.delay_between_target_changes},
                     {sha(text[25]), p.do_not_change_z_angle}};

  for (const auto& i : text) {
    VMProtectFreeString(i);
  }
}

inline void from_json(const nlohmann::json& j, vector_aimbot& p) {
  auto vmp = &VMProtectDecryptStringA;
  auto sha = [](const char* const str) {
    return data_representation::sha256_string(str);
  };

  const char* const text[]{
#include "keys/vector_aimbot.hpp"
  };

  j.at(sha(text[0])).get_to(p.enable);
  j.at(sha(text[1])).get_to(p.smooth);
  j.at(sha(text[2])).get_to(p.max_angle_in_degrees);
  j.at(sha(text[3])).get_to(p.divide_angle_by_distance);
  j.at(sha(text[4])).get_to(p.hold_target);
  j.at(sha(text[5])).get_to(p.check_for_obstacles);
  j.at(sha(text[6])).get_to(p.check_for_distance);
  j.at(sha(text[7])).get_to(p.ignore_the_dead);
  j.at(sha(text[8])).get_to(p.ignore_same_group);
  j.at(sha(text[9])).get_to(p.ignore_friendly_nicknames);
  j.at(sha(text[10])).get_to(p.ignore_same_color);
  j.at(sha(text[11])).get_to(p.aim_only_when_shooting);
  j.at(sha(text[12])).get_to(p.head);
  j.at(sha(text[13])).get_to(p.neck);
  j.at(sha(text[14])).get_to(p.right_shoulder);
  j.at(sha(text[15])).get_to(p.left_shoulder);
  j.at(sha(text[16])).get_to(p.right_elbow);
  j.at(sha(text[17])).get_to(p.left_elbow);
  j.at(sha(text[18])).get_to(p.stomach);
  j.at(sha(text[19])).get_to(p.right_knee);
  j.at(sha(text[20])).get_to(p.left_knee);
  j.at(sha(text[21])).get_to(p.model_groups);
  j.at(sha(text[22])).get_to(p.friendly_nicknames);
  j.at(sha(text[23])).get_to(p.min_distance);
  j.at(sha(text[24])).get_to(p.delay_between_target_changes);
  j.at(sha(text[25])).get_to(p.do_not_change_z_angle);

  for (const auto& i : text) {
    VMProtectFreeString(i);
  }
}

// silent_aimbot
inline void to_json(nlohmann::json& j, const silent_aimbot& p) {
  auto vmp = &VMProtectDecryptStringA;
  auto sha = [](const char* const str) {
    return data_representation::sha256_string(str);
  };

  const char* const text[]{
#include "keys/silent_aimbot.hpp"
  };

  j = nlohmann::json{{sha(text[0]), p.enable},
                     {sha(text[1]), p.hit_rate},
                     {sha(text[2]), p.max_angle_in_degrees},
                     {sha(text[3]), p.divide_angle_by_distance},
                     {sha(text[4]), p.hold_target},
                     {sha(text[5]), p.check_for_obstacles},
                     {sha(text[6]), p.check_for_distance},
                     {sha(text[7]), p.ignore_the_dead},
                     {sha(text[8]), p.ignore_same_group},
                     {sha(text[9]), p.ignore_friendly_nicknames},
                     {sha(text[10]), p.ignore_same_color},
                     {sha(text[11]), p.head},
                     {sha(text[12]), p.neck},
                     {sha(text[13]), p.right_shoulder},
                     {sha(text[14]), p.left_shoulder},
                     {sha(text[15]), p.right_elbow},
                     {sha(text[16]), p.left_elbow},
                     {sha(text[17]), p.stomach},
                     {sha(text[18]), p.right_knee},
                     {sha(text[19]), p.left_knee},
                     {sha(text[20]), p.model_groups},
                     {sha(text[21]), p.friendly_nicknames},
                     {sha(text[22]), p.pull_camera_toward_enemy},
                     {sha(text[23]), p.turn_player_toward_enemy},
                     {sha(text[24]), p.display_triangle_above_enemy},
                     {sha(text[25]), p.increase_hit_rate},
                     {sha(text[26]), p.min_distance},
                     {sha(text[27]), p.step_to_increase_hit_rate},
                     {sha(text[28]), p.delay_between_target_changes}};

  for (const auto& i : text) {
    VMProtectFreeString(i);
  }
}

inline void from_json(const nlohmann::json& j, silent_aimbot& p) {
  auto vmp = &VMProtectDecryptStringA;
  auto sha = [](const char* const str) {
    return data_representation::sha256_string(str);
  };

  const char* const text[]{
#include "keys/silent_aimbot.hpp"
  };

  j.at(sha(text[0])).get_to(p.enable);
  j.at(sha(text[1])).get_to(p.hit_rate);
  j.at(sha(text[2])).get_to(p.max_angle_in_degrees);
  j.at(sha(text[3])).get_to(p.divide_angle_by_distance);
  j.at(sha(text[4])).get_to(p.hold_target);
  j.at(sha(text[5])).get_to(p.check_for_obstacles);
  j.at(sha(text[6])).get_to(p.check_for_distance);
  j.at(sha(text[7])).get_to(p.ignore_the_dead);
  j.at(sha(text[8])).get_to(p.ignore_same_group);
  j.at(sha(text[9])).get_to(p.ignore_friendly_nicknames);
  j.at(sha(text[10])).get_to(p.ignore_same_color);
  j.at(sha(text[11])).get_to(p.head);
  j.at(sha(text[12])).get_to(p.neck);
  j.at(sha(text[13])).get_to(p.right_shoulder);
  j.at(sha(text[14])).get_to(p.left_shoulder);
  j.at(sha(text[15])).get_to(p.right_elbow);
  j.at(sha(text[16])).get_to(p.left_elbow);
  j.at(sha(text[17])).get_to(p.stomach);
  j.at(sha(text[18])).get_to(p.right_knee);
  j.at(sha(text[19])).get_to(p.left_knee);
  j.at(sha(text[20])).get_to(p.model_groups);
  j.at(sha(text[21])).get_to(p.friendly_nicknames);
  j.at(sha(text[22])).get_to(p.pull_camera_toward_enemy);
  j.at(sha(text[23])).get_to(p.turn_player_toward_enemy);
  j.at(sha(text[24])).get_to(p.display_triangle_above_enemy);
  j.at(sha(text[25])).get_to(p.increase_hit_rate);
  j.at(sha(text[26])).get_to(p.min_distance);
  j.at(sha(text[27])).get_to(p.step_to_increase_hit_rate);
  j.at(sha(text[28])).get_to(p.delay_between_target_changes);

  for (const auto& i : text) {
    VMProtectFreeString(i);
  }
}
}  // namespace configuration
}  // namespace modification

#endif  // ARCANE_JSON_ENCRYPTED_HPP
