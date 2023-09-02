#include "spread_control.h"

#include <CWeapon.h>

using namespace modification::client::actor;

void spread_control::process(const data& settings) {
  const auto default_aim_scale_dist = 5.0f;
  fPlayerAimScaleDist =
      settings.enable ? settings.spread * 0.01f * default_aim_scale_dist : default_aim_scale_dist;
}
