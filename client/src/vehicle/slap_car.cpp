#include "slap_car.h"

#include <imgui.h>
#include <psdk_utils/psdk_utils.h>
#include <samp_utils/samp_utils.h>

using namespace modification::client::vehicle;

slap_car::slap_car() : state_{}, state_update_time_{} {
}

void slap_car::process(const slap_car::data& settings) {
  if (!settings.enable || samp_utils::is_cursor_enabled()) return;

  const auto vehicle = psdk_utils::player()->m_pVehicle;

  if (vehicle == nullptr) return;

  using namespace std::chrono;

  const auto time_elapsed_from_state_update =
      duration_cast<milliseconds>(clock::now() - state_update_time_).count();

  if (state_ == state::no && psdk_utils::key::down(settings.key)) {
    update_state(state::slap_process);
  }

  const auto multiplier = 15.0f / psdk_utils::math::sqrt(ImGui::GetIO().Framerate);
  const auto duration_to_slap = 6 * multiplier;

  if (state_ == state::slap_process && time_elapsed_from_state_update > duration_to_slap) {
    auto pos = vehicle->GetPosition();
    vehicle->SetPosn(pos.x, pos.y, pos.z + 0.18f);
    vehicle->SetOrientation(0.f, 0.f, vehicle->GetHeading());
    vehicle->m_vecMoveSpeed.z = 0.f;
    update_state(state::no);
  }
}

void slap_car::update_state(slap_car::state new_state) {
  state_ = new_state;
  state_update_time_ = std::chrono::steady_clock::now();
}
