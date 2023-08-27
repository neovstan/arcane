#include "auto_reload.h"

#include <imgui.h>
#include <psdk_utils/psdk_utils.h>

using namespace modification::client::actor;

auto_reload::auto_reload() : state_{}, state_update_time_{} {
}

void auto_reload::process(bool enabled) {
  auto player = psdk_utils::player();
  auto slot = player->m_nActiveWeaponSlot;
  auto weapon = player->m_aWeapons[slot];

  static auto last_slot = slot;

  if (!enabled || weapon.m_nType == 25 || ((slot < 2 || slot > 5) && state_ == state::no)) return;

  using namespace std::chrono;

  if (state_ == state::no && weapon.m_nAmmoInClip == 1 &&
      weapon.m_nAmmoInClip != weapon.m_nTotalAmmo) {
    last_slot = slot;
    update_state(state::switch_to_prev);
  }

  const auto time_elapsed_from_state_update =
      duration_cast<milliseconds>(clock::now() - state_update_time_).count();

  const auto multiplier = 15.0f / psdk_utils::math::sqrt(ImGui::GetIO().Framerate);

  const auto duration_switch_to_prev = 125 * multiplier, duration_switch_back = 200 * multiplier;

  if (state_ == state::switch_to_prev && time_elapsed_from_state_update > duration_switch_to_prev) {
    player->SetCurrentWeapon(last_slot - 1);
    player->m_aWeapons[last_slot].Reload(player);
    update_state(state::switch_back);
  } else if (state_ == state::switch_back &&
             time_elapsed_from_state_update > duration_switch_back) {
    player->m_aWeapons[last_slot - 1].Reload(player);
    player->SetCurrentWeapon(last_slot);
    update_state(state::no);
  }
}

void auto_reload::update_state(state new_state) {
  state_ = new_state;
  state_update_time_ = std::chrono::steady_clock::now();
}
