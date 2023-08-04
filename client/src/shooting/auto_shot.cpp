#include "auto_shot.h"

#include <psdk_utils/psdk_utils.h>

using namespace modification::client::shooting;

void auto_shot::process() {
  namespace psdk = psdk_utils;

  const auto targetted_ped = psdk::player()->m_pPlayerTargettedPed;
  if (!settings.enable || !targetted_ped) return;

  if (psdk::weapon_in_hand() == eWeaponType::WEAPON_DESERT_EAGLE) {
    const auto r =
        psdk::local_vector{psdk::get_camera().GetPosition() - targetted_ped->GetPosition()}.r();

    const auto k = 5.0f;
    const auto weapon_radius = psdk::player()->GetWeaponRadiusOnScreen();
    const auto min_radius = psdk::math::clamp(0.2f, k / r, 1.0f);

    if (weapon_radius > min_radius) return;
  }

  auto& keys = psdk::player()->GetPadFromPlayer()->NewState;
  keys.ButtonCircle = psdk::game_key_state::down;
}