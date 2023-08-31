#include "air_walking.h"

#include <psdk_utils/psdk_utils.h>

using namespace modification::client::actor;

air_walking::air_walking() : time_{clock::now()}, speed_{} {
}

void air_walking::process(const data& settings) {
  if (!settings.enable) {
    maintain_validity();
    return;
  }

  const auto delta_time = get_delta_time();
  const auto current_speed =
      get_current_speed(delta_time, settings.speed, settings.acceleration_time);

  const auto heading = get_camera_heading();
  const auto magnitude = current_speed * delta_time;
  const auto movement = compute_movement_vector(settings.keys, heading, magnitude);

  speed_ = movement.r() ? current_speed : 0.0f;

  move(movement);
}

void air_walking::maintain_validity() {
  time_ = clock::now();
  speed_ = 0.0f;
}

float air_walking::get_camera_heading() {
  return psdk_utils::math::rad2deg(psdk_utils::get_active_cam().m_fHorizontalAngle) + 180.0f;
}

float air_walking::get_delta_time() {
  const auto now = clock::now();

  const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - time_);
  const auto delta_time = psdk_utils::math::clamp(0.0f, float(duration.count()) / 1000.0f, 0.2f);

  time_ = now;

  return delta_time;
}

float air_walking::get_current_speed(float delta_time, float speed, float acceleration_time) {
  const auto acceleration = speed / acceleration_time;
  return psdk_utils::math::clamp(0.0f, speed_ + acceleration * delta_time, speed);
}

psdk_utils::local_vector air_walking::compute_movement_vector(const data::movement_keys& keys,
                                                              float heading,
                                                              float movement_delta_magnitude) {
  psdk_utils::local_vector movement_vector;

  if (psdk_utils::key::down(keys.forward)) {
    movement_vector = movement_vector.transit(
        psdk_utils::polar_vector_3d{heading, 0.0f, movement_delta_magnitude});
  } else if (psdk_utils::key::down(keys.backward)) {
    movement_vector = movement_vector.transit(
        psdk_utils::polar_vector_3d{heading - 180.0f, 0.0f, movement_delta_magnitude});
  }

  if (psdk_utils::key::down(keys.right)) {
    movement_vector = movement_vector.transit(
        psdk_utils::polar_vector_3d{heading - 90.0f, 0.0f, movement_delta_magnitude});
  } else if (psdk_utils::key::down(keys.left)) {
    movement_vector = movement_vector.transit(
        psdk_utils::polar_vector_3d{heading + 90.0f, 0.0f, movement_delta_magnitude});
  }

  if (psdk_utils::key::down(keys.up)) {
    movement_vector =
        movement_vector.transit(psdk_utils::polar_vector_3d{0.0f, 90.0f, movement_delta_magnitude});
  } else if (psdk_utils::key::down(keys.down)) {
    movement_vector = movement_vector.transit(
        psdk_utils::polar_vector_3d{0.0f, -90.0f, movement_delta_magnitude});
  }

  return movement_vector;
}

void air_walking::move(const psdk_utils::local_vector& vec) {
  if (!is_player_ready_to_move()) return;
  auto player = psdk_utils::player();
  player->SetPosn(psdk_utils::local_vector{player->GetPosition()} + vec);
}
bool air_walking::is_player_ready_to_move() {
  auto player = psdk_utils::player();
  if (!(player->IsPointerValid() && player->IsAlive())) return false;

  auto& flags = player->m_nPedFlags;
  flags.bIsStanding = flags.bWasStanding = flags.bStayInSamePlace = true;

  player->m_vecMoveSpeed = psdk_utils::local_vector{};

  return true;
}
