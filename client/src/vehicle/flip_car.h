#ifndef ARCANE_CLIENT_SRC_VEHICLE_FLIP_CAR_H
#define ARCANE_CLIENT_SRC_VEHICLE_FLIP_CAR_H

#include <arcane_packets/configuration.hpp>

namespace modification::client::vehicle {
class flip_car {
  using data = packets::configuration::vehicle::flip_car_data;

 public:
  void process(const data& settings);
};
}  // namespace modification::client::vehicle

#endif  // ARCANE_CLIENT_SRC_VEHICLE_FLIP_CAR_H
