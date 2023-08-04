#include "silent_aimbot.h"

#include <random>
#include <CWorld.h>
#include <psdk_utils/psdk_utils.h>

using namespace modification::client::shooting;

silent_aimbot::silent_aimbot()
    : finder_{{psdk_utils::weapon::mode::pistols,
               {get_settings(psdk_utils::weapon::mode::pistols)}},
              {psdk_utils::weapon::mode::shotguns,
               {get_settings(psdk_utils::weapon::mode::shotguns)}},
              {psdk_utils::weapon::mode::semi, {get_settings(psdk_utils::weapon::mode::semi)}},
              {psdk_utils::weapon::mode::assault,
               {get_settings(psdk_utils::weapon::mode::assault)}},
              {psdk_utils::weapon::mode::rifles, {get_settings(psdk_utils::weapon::mode::rifles)}}},
      is_aiming_at_person_{},
      use_target_range_instead_of_weapons_{true} {
}

void silent_aimbot::heading_process(float& heading) {
  const auto weapon_mode = psdk_utils::weapon::get_mode(psdk_utils::weapon_in_hand());
  if (weapon_mode == psdk_utils::weapon::mode::unknown) return;

  const auto& settings = this->settings.at(weapon_mode);

  if (const auto enemy = get_enemy(); enemy.ped) {
    using namespace psdk_utils;

    const local_vector difference_vector{get_camera().GetPosition() - enemy.ped->GetPosition()};

    const auto angle = math::deg2rad(difference_vector.a_xy()) + math::pi() / 2.0f;

    if (settings.pull_camera_toward_enemy) {
      player()->m_fCurrentRotation = player()->m_fAimingRotation =
          math::deg2rad(enemy.angle.x()) + math::pi() / 2.0f - 0.05f;
    }

    if (settings.turn_player_toward_enemy) {
      heading = angle;
    }
  }
}

void silent_aimbot::aim_look_process(psdk_utils::local_vector& target) {
  namespace psdk = psdk_utils;

  const auto weapon_mode = psdk::weapon::get_mode(psdk::weapon_in_hand());
  if (weapon_mode == psdk::weapon::mode::unknown) return;

  const auto& settings = this->settings.at(weapon_mode);
  if (is_aiming_at_person_ || !settings.display_triangle_above_enemy) return;

  if (const auto enemy = get_enemy(); enemy.ped) {
    target = enemy.position;
  }
}

void silent_aimbot::bullet_process(const psdk_utils::local_vector& origin,
                                   psdk_utils::local_vector& target) {
  const auto weapon_mode = psdk_utils::weapon::get_mode(psdk_utils::weapon_in_hand());
  if (weapon_mode == psdk_utils::weapon::mode::unknown) return;

  auto& settings = this->settings.at(weapon_mode);

  const auto enemy = get_enemy();
  if (!enemy.ped || is_aiming_at_person_) {
    ++settings.details.missed_hits;
    return;
  }

  const auto enemy_pos = [enemy]() {
    auto enemy_pos = enemy.ped->GetPosition();
    enemy_pos.z = enemy.position.z();
    return psdk_utils::local_vector{enemy_pos};
  }();

  const auto step = [enemy_pos, target]() {
    auto target_to_enemy = enemy_pos - target;
    target_to_enemy /= 1000.0f;
    return target_to_enemy;
  }();

  for (auto i = 1; i <= 1000; ++i) {
    CColPoint colpoint{};
    CEntity* entity{};

    const auto new_target = target + i * step;

    if (!CWorld::ProcessLineOfSight(origin, new_target, colpoint, entity, false, false, true, false,
                                    false, false, false, false) ||
        entity != enemy.ped) {
      continue;
    }

    const auto offset_to_make_target_closer_to_enemy = [enemy_pos, new_target](float divider) {
      auto difference = enemy_pos - new_target;
      difference /= divider;
      return difference;
    }(2.0f);

    using namespace psdk_utils;

    const auto hit_rate =
        settings.hit_rate +
        (settings.increase_hit_rate
             ? (static_cast<int>(
                   ((psdk_utils::local_vector{get_camera().GetPosition()} - enemy.position).r() -
                    settings.min_distance) /
                   settings.step_to_increase_hit_rate))
             : 0);

    if (settings.details.missed_hits < hit_rate) {
      ++settings.details.missed_hits;
      break;
    }

    settings.details.missed_hits = 1;

    target = new_target + offset_to_make_target_closer_to_enemy;

    const auto offset_to_make_target_original = [](float min, float max) {
      std::default_random_engine engine;
      engine.seed(std::chrono::duration_cast<std::chrono::seconds>(
                      std::chrono::system_clock::now().time_since_epoch())
                      .count());

      std::uniform_real_distribution<float> distribution{min, max};
      return psdk_utils::local_vector{distribution(engine), distribution(engine),
                                      distribution(engine)};
    }(-0.2f, 0.2f);

    target += offset_to_make_target_original;
    break;
  }
}

enemy_finder::target silent_aimbot::get_enemy() {
  const auto weapon_mode = psdk_utils::weapon::get_mode(psdk_utils::weapon_in_hand());
  if (weapon_mode == psdk_utils::weapon::mode::unknown) return {};

  const auto& settings = this->settings.at(weapon_mode);
  if (!settings.enable) return {};

  auto& finder = this->finder_.at(weapon_mode);
  return finder.get();
}

enemy_finder::settings silent_aimbot::get_settings(psdk_utils::weapon::mode number) {
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

void silent_aimbot::process(bool is_aiming_at_person) {
  is_aiming_at_person_ = is_aiming_at_person;
  get_enemy();
}
