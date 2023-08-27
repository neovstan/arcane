#ifndef ARCANE_CLIENT_SRC_VEHICLE_VEHICLE_H
#define ARCANE_CLIENT_SRC_VEHICLE_VEHICLE_H

#include <arcane_packets/configuration.hpp>

#include "infinite_nitro.h"
#include "drive_on_water.h"
#include "repair_car.h"
#include "fast_exit.h"
#include "flip_car.h"
#include "slap_car.h"

namespace modification::client::vehicle {
class vehicle {
 public:
  void process();
  infinite_nitro::order process_infinite_nitro();
  drive_on_water::order process_drive_on_water();

 public:
  struct packets::configuration::vehicle settings;

 private:
  infinite_nitro infinite_nitro;
  drive_on_water drive_on_water;
  repair_car repair_car;
  fast_exit fast_exit;
  flip_car flip_car;
  slap_car slap_car;
};
}

#endif // ARCANE_CLIENT_SRC_VEHICLE_VEHICLE_H
