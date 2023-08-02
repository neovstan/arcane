#include "silent_aimbot.h"

#include <CWorld.h>
#include <psdk_utils/cmath.h>

#include <random>

using namespace modification::client::shooting;

void silent_aimbot::heading_process(float& heading) {
  const auto weapon_mode = psdk_utils::weapon::get_mode(psdk_utils::weapon_in_hand());
  if (weapon_mode == psdk_utils::weapon::mode::unknown) return;

  const auto& settings = this->settings.at(weapon_mode);

  if (const auto enemy = get_enemy(); enemy.ped) {
    using namespace psdk_utils;

    const auto angle =
        math::angle_between_points(get_camera().GetPosition(), enemy.ped->GetPosition()).x +
        math::pi() / 2.0f;

    if (settings.pull_camera_toward_enemy) {
      player()->m_fCurrentRotation = player()->m_fAimingRotation =
          enemy.angle.x + math::pi() / 2.0f - 0.05f;
    }

    if (settings.turn_player_toward_enemy) {
      heading = angle;
    }
  }
}

void silent_aimbot::aim_look_process(CVector& target) {
  namespace psdk = psdk_utils;

  if (is_aiming_at_person_) return;

  const auto weapon_mode = psdk::weapon::get_mode(psdk::weapon_in_hand());
  if (weapon_mode == psdk::weapon::mode::unknown) return;

  const auto& settings = this->settings.at(weapon_mode);

  if (const auto enemy = get_enemy(); enemy.ped) {
    if (settings.display_triangle_above_enemy) {
      target = enemy.position;
    }
  }
}

void silent_aimbot::bullet_process(const CVector& origin, CVector& target) {
  const auto weapon_mode = psdk_utils::weapon::get_mode(psdk_utils::weapon_in_hand());
  if (weapon_mode == psdk_utils::weapon::mode::unknown) return;

  auto& settings = this->settings.at(weapon_mode);

  const auto enemy = get_enemy();
  if (!enemy.ped || is_aiming_at_person_) {
    ++settings.details.missed_hits;
    return;
  }

  const auto enemy_pos = [enemy]() {
    CVector enemy_pos{enemy.ped->GetPosition()};
    enemy_pos.z = enemy.position.z;
    return enemy_pos;
  }();

  const auto step = [enemy_pos, target]() {
    auto target_to_enemy = enemy_pos - target;
    target_to_enemy /= 1000.0f;
    return target_to_enemy;
  }();

  for (int i{1}; i <= 1000; ++i) {
    CColPoint colpoint{};
    CEntity* entity{};

    const CVector new_target{target + i * step};

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
                   (math::distance_between_points(get_camera().GetPosition(), enemy.position) -
                    settings.min_distance) /
                   settings.step_to_increase_hit_rate))
             : 0);

    if (settings.details.missed_hits < hit_rate) {
      ++settings.details.missed_hits;
      break;
    }

    settings.details.missed_hits = 1;

    target = new_target + offset_to_make_target_closer_to_enemy;

    const auto offset_to_make_target_original_and_unrepeatable = [](float min, float max) {
      std::default_random_engine engine{};
      engine.seed(std::chrono::duration_cast<std::chrono::seconds>(
                      std::chrono::system_clock::now().time_since_epoch())
                      .count());

      std::uniform_real_distribution<float> distribution{min, max};
      return CVector{distribution(engine), distribution(engine), distribution(engine)};
    }(-0.2f, 0.2f);

    target += offset_to_make_target_original_and_unrepeatable;
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

void silent_aimbot::process(bool is_aiming_at_person) {
  is_aiming_at_person_ = is_aiming_at_person;
  get_enemy();
}
