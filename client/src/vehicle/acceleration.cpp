#include "acceleration.h"

#include <psdk_utils/psdk_utils.h>
#include <samp_utils/samp_utils.h>

using namespace modification::client::vehicle;

acceleration::acceleration() : time_{clock::now()}, velocity_{} {
}

void acceleration::process(const data& settings) {
  const auto vehicle = get_vehicle(settings);
  if (!vehicle) {
    maintain_validity();
    return;
  }

  const psdk_utils::local_vector velocity{vehicle->m_vecMoveSpeed};

  if (psdk_utils::key::down(settings.key)) {
    const auto delta_time = get_delta_time();
    const auto acceleration = (velocity - velocity_) / delta_time;

    const auto difference = velocity.r() - velocity_.r();
    const auto multiplier = 1.0f + settings.additional_acceleration;

    const psdk_utils::polar_vector new_acceleration{
        acceleration.a_xy(), acceleration.r() * (difference > 0.0f ? multiplier : 1.0f)};

    vehicle->m_vecMoveSpeed = velocity_ + (new_acceleration * delta_time).local();
  }

  velocity_ = velocity;
}

void acceleration::maintain_validity() {
  velocity_ = {};
  time_ = clock::now();
}

CVehicle* acceleration::get_vehicle(const data& settings) {
  if (!settings.enable) return nullptr;

  const auto player = psdk_utils::player();
  if (!player) return nullptr;

  const auto vehicle = player->m_pVehicle;
  if (samp_utils::is_cursor_enabled() || !vehicle) return nullptr;

  return vehicle;
}

float acceleration::get_delta_time() {
  const auto now = clock::now();

  const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - time_);
  const auto delta_time = psdk_utils::math::clamp(0.0f, float(duration.count()) / 1000.0f, 0.2f);

  time_ = now;

  return delta_time;
}
