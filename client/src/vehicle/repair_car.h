#ifndef ARCANE_CLIENT_SRC_VEHICLE_REPAIR_VEHICLE_H
#define ARCANE_CLIENT_SRC_VEHICLE_REPAIR_VEHICLE_H

#include <arcane_packets/configuration.hpp>

namespace modification::client::vehicle {
class repair_car {
  using data = packets::configuration::vehicle::repair_car_data;

 public:
  void process(const data& settings);
};
}  // namespace modification::client::vehicle

#endif  // ARCANE_CLIENT_SRC_VEHICLE_REPAIR_VEHICLE_H
