#include "fast_exit.h"

#include <imgui.h>
#include <psdk_utils/psdk_utils.h>

using namespace modification::client::vehicle;

fast_exit::fast_exit() : state_{}, state_update_time_{} {
}

void fast_exit::process(bool enabled) {
  if (!enabled) return;
  const auto player_info = psdk_utils::player_info();

  using namespace std::chrono;

  if (state_ == state::no && player_info->m_bTryingToExitCar) {
    update_state(state::reset_animation);
  }

  const auto time_elapsed_from_state_update =
      duration_cast<milliseconds>(clock::now() - state_update_time_).count();

  const auto multiplier = 15.0f / psdk_utils::math::sqrt(ImGui::GetIO().Framerate);
  const auto duration_to_reset = 375 * multiplier;

  if (state_ == state::reset_animation && time_elapsed_from_state_update > duration_to_reset) {
    if (player_info->m_pPed->m_nPedState != PEDSTATE_DRIVING) {
      player_info->m_pPed->Teleport(player_info->m_pPed->m_matrix->pos);
    }
    update_state(state::no);
  }
}

void fast_exit::update_state(state new_state) {
  state_ = new_state;
  state_update_time_ = std::chrono::steady_clock::now();
}
