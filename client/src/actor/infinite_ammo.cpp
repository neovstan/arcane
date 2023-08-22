#include "infinite_ammo.h"

using namespace modification::client::actor;

infinite_ammo::order infinite_ammo::process(bool enabled) {
  return order{enabled};
}
