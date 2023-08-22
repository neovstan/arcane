//
// Created by Developer on 22.08.2023.
//

#include "infinite_ammo.h"

using namespace modification::client::actor;

infinite_ammo::order infinite_ammo::process(bool enabled) {
  return order{enabled};
}
