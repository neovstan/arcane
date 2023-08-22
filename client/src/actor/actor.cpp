#include "actor.h"

using namespace modification::client::actor;

void actor::process() {
  air_walking.process(settings.air_walking);
  spread_control.process(settings.spread_control);
}

anti_stun::order actor::process_anti_stun() {
  return anti_stun.process(settings.anti_stun);
}

void actor::process_fast_run(float& out_run_speed) {
  fast_run.process(settings.fast_run, out_run_speed);
}

infinite_ammo::order actor::process_infinite_ammo() {
  return infinite_ammo.process(settings.infinite_ammo);
}

infinite_clip::order actor::process_infinite_clip() {
  return infinite_clip.process(settings.infinite_clip);
}

disable_camera_reset::order actor::process_camera_reset() {
  return disable_camera_reset.process(settings.disable_camera_reset);
}