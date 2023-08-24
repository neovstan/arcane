#ifndef ARCANE_CLIENT_SRC_VEHICLE_VEHICLE_H
#define ARCANE_CLIENT_SRC_VEHICLE_VEHICLE_H

#include <arcane_packets/configuration.hpp>

#include "infinite_nitro.h"
#include "drive_on_water.h"

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
};
}

#endif // ARCANE_CLIENT_SRC_VEHICLE_VEHICLE_H
