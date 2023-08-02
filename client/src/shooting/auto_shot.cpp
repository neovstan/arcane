#include "auto_shot.h"

#include <psdk_utils/psdk_utils.h>

using namespace modification::client::shooting;

void auto_shot::process() {
  namespace psdk = psdk_utils;

  const auto weapon_radius_screen = psdk::player()->GetWeaponRadiusOnScreen();

  auto& keys = psdk::player()->GetPadFromPlayer()->NewState;
  keys.ButtonCircle = psdk::game_key_state::down;
}