#include "infinite_health.h"

using namespace modification::client::actor;

infinite_health::order infinite_health::process(bool enabled) {
  return order{enabled};
}
