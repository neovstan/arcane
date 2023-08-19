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
