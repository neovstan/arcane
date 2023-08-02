#include "auto_cbug.h"

#include <imgui.h>
#include <psdk_utils/psdk_utils.h>

using namespace modification::client::shooting;

void auto_cbug::process() {
  using namespace std::chrono;
  namespace psdk = psdk_utils;

  auto player = psdk::player();

  if (settings.enable && psdk::camera::is_player_aiming() &&
      psdk::weapon_in_hand() == eWeaponType::WEAPON_DESERT_EAGLE &&
      player->m_aWeapons[player->m_nActiveWeaponSlot].m_nAmmoInClip) {
    if (state_ == state::no && ((settings.auto_repeat && psdk::key::down(settings.key) ||
                                 (!settings.auto_repeat && psdk::key::pressed(settings.key))))) {
      update_state(state::start);
    }
  }

  auto& keys = player->GetPadFromPlayer()->NewState;

  const auto time_elapsed_from_state_update =
      duration_cast<milliseconds>(steady_clock::now() - state_update_time_).count();

  const auto multiplier = 15.0f / std::sqrt(ImGui::GetIO().Framerate);

  const auto duration_from_squat_to_attack = settings.duration_from_squat_to_attack * multiplier,
             duration_from_attack_to_squat = settings.duration_from_attack_to_squat * multiplier;

  if (state_ == state::start &&
      (!settings.auto_repeat || time_elapsed_from_state_update > duration_from_squat_to_attack)) {
    update_state(state::attack);
    keys.ButtonCircle = psdk::game_key_state::down;
  } else if (state_ == state::attack &&
             time_elapsed_from_state_update > duration_from_attack_to_squat) {
    update_state(state::aim_and_squat);
    keys.RightShoulder1 = psdk::game_key_state::no;
    keys.ShockButtonL = psdk::game_key_state::down;
  } else if (state_ == state::aim_and_squat) {
    state_ = state::no;
  }
}
