#ifndef ARCANE_CLIENT_SRC_ACTOR_ACTOR_H
#define ARCANE_CLIENT_SRC_ACTOR_ACTOR_H

#include <arcane_packets/configuration.hpp>

#include "air_walking.h"
#include "anti_stun.h"
#include "fast_run.h"
#include "heavy_fist.h"
#include "auto_reload.h"
#include "infinite_run.h"
#include "spread_control.h"
#include "infinite_ammo.h"
#include "infinite_clip.h"
#include "stay_on_feet.h"
#include "disable_camera_reset.h"

namespace modification::client::actor {
class actor {
 public:
  void process();
  anti_stun::order process_anti_stun();

  infinite_run::order process_infinite_run();
  infinite_ammo::order process_infinite_ammo();
  infinite_clip::order process_infinite_clip();
  stay_on_feet::order process_stay_on_feet();
  disable_camera_reset::order process_camera_reset();

  void process_fast_run(float& out_run_speed);
  void process_control();

 public:
  struct packets::configuration::actor settings;

 private:
  air_walking air_walking;
  anti_stun anti_stun;
  fast_run fast_run;
  heavy_fist heavy_fist;
  auto_reload auto_reload;
  infinite_run infinite_run;
  spread_control spread_control;
  infinite_ammo infinite_ammo;
  infinite_clip infinite_clip;
  stay_on_feet stay_on_feet;
  disable_camera_reset disable_camera_reset;
};
}  // namespace modification::client::actor

#endif  // ARCANE_CLIENT_SRC_ACTOR_ACTOR_H
