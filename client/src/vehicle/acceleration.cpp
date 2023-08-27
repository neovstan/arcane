#include "acceleration.h"

#include <imgui.h>
#include <psdk_utils/psdk_utils.h>
#include <samp_utils/samp_utils.h>

using namespace modification::client::vehicle;

acceleration::acceleration() : state_{}, state_update_time_{} {
}

void acceleration::process(const acceleration::data& settings) {
  if (!settings.enable || samp_utils::is_cursor_enabled()) return;

  const auto vehicle = psdk_utils::player()->m_pVehicle;

  if (vehicle == nullptr) return;

  using namespace std::chrono;

  const auto time_elapsed_from_state_update =
      duration_cast<milliseconds>(clock::now() - state_update_time_).count();

  if (state_ == state::no && psdk_utils::key::down(settings.key)) {
    update_state(state::process_accelerate);
  }

  const auto multiplier = 15.0f / psdk_utils::math::sqrt(ImGui::GetIO().Framerate);
  const auto duration_to_slap = 250 * multiplier;

  if (state_ == state::process_accelerate && time_elapsed_from_state_update > duration_to_slap) {
    auto angle = vehicle->GetHeading();
    vehicle->m_vecMoveSpeed.x += settings.additional_acceleration * sin(angle) * -1;
    vehicle->m_vecMoveSpeed.y += settings.additional_acceleration * cosf(angle);
    update_state(state::no);
  }
}

void acceleration::update_state(acceleration::state new_state) {
  state_ = new_state;
  state_update_time_ = std::chrono::steady_clock::now();
}
