#include "actor.h"

using namespace modification::client::actor;

void actor::process() {
  air_walking.process(settings.air_walking);
}
