#include "enemy_finder.h"

// plugin sdk
#include <CWorld.h>
#include <ePedBones.h>

// psdk & samp utils
#include <psdk_utils/psdk_utils.h>
#include <samp_utils/samp_utils.h>

using namespace modification::client::shooting;

enemy_finder::enemy_finder(const settings& settings)
    : settings_{settings}, enemy_{}, target_change_time_{}, previous_enemy_ped_{} {
}

enemy_finder::target enemy_finder::get() {
  const auto previous_enemy_ped = enemy_.ped;

  if (settings_.hold_target) {
    enemy_ = produce_target(enemy_.ped);
    if (enemy_.ped) {
      return enemy_;
    }
  } else {
    enemy_ = {};
  }

  if (previous_enemy_ped && !enemy_.ped) {
    previous_enemy_ped_ = previous_enemy_ped;
    target_change_time_ = clock::now();
  }

  enemy_.difference = settings_.max_angle_in_degrees;

  for (const auto& i : psdk_utils::peds_around()) {
    const auto target = produce_target(i);
    if (target.ped && target.difference < enemy_.difference) {
      using namespace std::chrono;
      if (target.ped != previous_enemy_ped_ &&
          duration_cast<milliseconds>(clock::now() - target_change_time_).count() <
              settings_.delay_between_target_changes) {
        continue;
      }

      enemy_ = target;
    }
  }

  return enemy_;
}

enemy_finder::target enemy_finder::produce_target(CPed* ped) const {
  namespace psdk = psdk_utils;

  if (!ped || !ped->IsPointerValid() || !ped->GetIsOnScreen() ||
      !psdk::camera::is_player_aiming() || psdk::is_ped_stunned(psdk::player())) {
    return {};
  }

  const auto is_samp_found = samp_utils::get_version() != samp_utils::version::unknown;
  auto ped_samp_id = samp_utils::invalid_id();

  if (is_samp_found) {
    ped_samp_id = samp_utils::execute([ped](auto version) {
      using samp = samp_utils::invoke<decltype(version)>;
      return samp::get_id_by_ped(ped);
    });

    if (ped_samp_id == samp_utils::invalid_id()) return {};
  }

  if (settings_.ignore_same_group) {
    const auto player_model = psdk::player()->m_nModelIndex, target_model = ped->m_nModelIndex;

    const auto have_same_group = [&]() {
      for (const auto& [id, name, list, active] : settings_.model_groups) {
        if (!active) continue;
        auto player_model_found = false, target_model_found = false;

        for (const auto i : list) {
          if (i == player_model) player_model_found = true;
          if (i == target_model) target_model_found = true;

          if (player_model_found && target_model_found) {
            return true;
          }
        }
      }

      return false;
    }();

    if (have_same_group) return {};
  }

  if (settings_.ignore_the_dead) {
    if (is_samp_found) {
      const auto is_alive = samp_utils::execute([&](auto version) {
        using samp = samp_utils::invoke<decltype(version)>;
        return samp::is_player_alive(ped_samp_id) && !samp::is_player_afk(ped_samp_id);
      });

      if (!is_alive) return {};
    } else {
      if (!ped->IsAlive()) return {};
    }
  }

  if (settings_.ignore_friendly_nicknames) {
    const auto has_friendly_nickname = samp_utils::execute([&](auto version) {
      using samp = samp_utils::invoke<decltype(version)>;

      const auto player_pool = RefNetGame(version)->GetPlayerPool();
      const auto name = player_pool->GetName(ped_samp_id);

      const auto& vec = settings_.friendly_nicknames;
      return std::find(vec.begin(), vec.end(), name) != vec.end();
    });

    if (is_samp_found && has_friendly_nickname) return {};
  }

  if (settings_.ignore_same_color) {
    const auto have_same_color = samp_utils::execute([&](auto version) {
      using samp = samp_utils::invoke<decltype(version)>;

      const auto player_pool = RefNetGame(version)->GetPlayerPool();
      return player_pool->GetLocalPlayer()->GetColorAsARGB() ==
             player_pool->GetPlayer(ped_samp_id)->GetColorAsARGB();
    });

    if (is_samp_found && have_same_color) return {};
  }

  const auto nearest_bone = psdk::find_bone_making_minimum_angle_with_camera(
      ped, settings_.check_for_obstacles, settings_.check_for_distance,
      settings_.divide_angle_by_distance, settings_.max_angle_in_degrees, settings_.min_distance,
      settings_.use_target_range_instead_of_weapons, settings_.head, settings_.neck,
      settings_.right_shoulder, settings_.left_shoulder, settings_.right_elbow,
      settings_.left_elbow, settings_.stomach, settings_.right_knee, settings_.left_knee);

  if (!nearest_bone.existing) return {};
  return target{ped, nearest_bone.world_position, nearest_bone.desired_angle,
                nearest_bone.angle_to_sight};
}
