#include "heavy_fist.h"

#include <psdk_utils/psdk_utils.h>

using namespace modification::client::actor;

heavy_fist::heavy_fist() : state_{}, state_update_time_{}, last_mode_{} {
}

void heavy_fist::process(bool enabled) {
  const auto player_data = psdk_utils::player()->m_pPlayerData;
  const auto mode = TheCamera.m_aCams[TheCamera.m_nActiveCam].m_nMode;

  if (!enabled) {
    last_mode_ = mode;
    return;
  }

  using namespace std::chrono;

  if (last_mode_ == MODE_AIMWEAPON && last_mode_ != mode) {
    update_state(state::process_hfist_enable);
  }

  const auto time_elapsed_from_state_update =
      duration_cast<milliseconds>(clock::now() - state_update_time_).count();

  if (state_ == state::process_hfist_enable && time_elapsed_from_state_update > 75) {
    player_data->m_bPlayerSprintDisabled = true;
    update_state(state::process_hfist_disable);
  } else if (state_ == state::process_hfist_disable && time_elapsed_from_state_update > 20) {
    player_data->m_bPlayerSprintDisabled = false;
    update_state(state::no);
  }

  last_mode_ = mode;
}

void heavy_fist::update_state(state new_state) {
  state_ = new_state;
  state_update_time_ = std::chrono::steady_clock::now();
}
