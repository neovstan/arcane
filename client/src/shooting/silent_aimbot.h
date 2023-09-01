#ifndef ARCANE_CLIENT_SRC_SHOOTING_SILENT_AIMBOT_H
#define ARCANE_CLIENT_SRC_SHOOTING_SILENT_AIMBOT_H

#include <psdk_utils/local_vector.h>
#include <psdk_utils/weapon.h>

#include "enemy_finder.h"

namespace modification::client::shooting {
class silent_aimbot {
 public:
  silent_aimbot();

  void heading_process(float& heading);
  void aim_look_process(psdk_utils::local_vector& camera_look);
  void bullet_process(const psdk_utils::local_vector& origin, psdk_utils::local_vector& target);
  void process(bool is_aiming_at_person);

  enemy_finder::target get_enemy();
  enemy_finder::settings get_settings(psdk_utils::weapon::mode number);

 public:
  std::vector<packets::configuration::model_group> model_groups;
  std::vector<std::string> friendly_nicknames;
  std::unordered_map<psdk_utils::weapon::mode, packets::configuration::silent_aimbot> settings;

 private:
  std::unordered_map<psdk_utils::weapon::mode, enemy_finder> finder_;
  std::unordered_map<psdk_utils::weapon::mode, int> missed_hits_;

  bool is_aiming_at_person_;
  bool use_target_range_instead_of_weapons_;
};
}  // namespace modification::client::shooting

#endif  // ARCANE_CLIENT_SRC_SHOOTING_SILENT_AIMBOT_H
