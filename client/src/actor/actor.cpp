#include "actor.h"

using namespace modification::client::actor;

void actor::process() {
  air_walking.process(settings.air_walking);
}

anti_stun::order actor::process_anti_stun() {
  return anti_stun.process(settings.anti_stun);
}

void actor::process_fast_run(double& out_run_speed) {
  fast_run.process(settings.fast_run, out_run_speed);
}
