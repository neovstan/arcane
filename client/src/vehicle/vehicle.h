//
// Created by Developer on 20.08.2023.
//

#ifndef ARCANE_CLIENT_SRC_VEHICLE_VEHICLE_H
#define ARCANE_CLIENT_SRC_VEHICLE_VEHICLE_H

#include <arcane_packets/configuration.hpp>

#include "infinite_nitro.h"

namespace modification::client::vehicle {
class vehicle {
 public:
  void process();
  infinite_nitro::order process_infinite_nitro();

 public:
  struct packets::configuration::vehicle settings;

 private:
  infinite_nitro infinite_nitro;
};
}

#endif // ARCANE_CLIENT_SRC_VEHICLE_VEHICLE_H