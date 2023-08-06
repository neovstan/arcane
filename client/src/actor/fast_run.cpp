#include "fast_run.h"

using namespace modification::client::actor;

void fast_run::process(const data& settings, double& out_run_speed) {
  if (settings.enable) out_run_speed = settings.speed;
}
