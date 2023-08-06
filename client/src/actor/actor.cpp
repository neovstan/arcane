#include "actor.h"

using namespace modification::client::actor;

void actor::process() {
  air_walking.process(settings.air_walking);
}

anti_stun::order actor::process_anti_stun() {
  return anti_stun.process(settings.anti_stun);
}
