#ifndef ARCANE_CLIENT_SRC_VEHICLE_ACCELERATION_H
#define ARCANE_CLIENT_SRC_VEHICLE_ACCELERATION_H

#include <chrono>

#include <psdk_utils/local_vector.h>

#include <arcane_packets/configuration.hpp>

class CVehicle;

namespace modification::client::vehicle {
class acceleration {
  using data = packets::configuration::vehicle::acceleration_data;

 public:
  acceleration();

 public:
  void process(const data& settings);

 private:
  void maintain_validity();

 private:
  CVehicle* get_vehicle(const data& settings);
  float get_delta_time();

 private:
  using clock = std::chrono::steady_clock;

 private:
  clock::time_point time_;
  psdk_utils::local_vector velocity_;
};
}  // namespace modification::client::vehicle

#endif  // ARCANE_CLIENT_SRC_VEHICLE_ACCELERATION_H
