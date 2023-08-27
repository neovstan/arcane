#ifndef ARCANE_CLIENT_SRC_VEHICLE_ACCELERATION_H
#define ARCANE_CLIENT_SRC_VEHICLE_ACCELERATION_H

#include <arcane_packets/configuration.hpp>

namespace modification::client::vehicle {
class acceleration {
  using data = packets::configuration::vehicle::acceleration_data;

 public:
  acceleration();

  void process(const data& settings);

 private:
  using clock = std::chrono::steady_clock;
  enum class state { no, process_accelerate } state_;

 private:
  void update_state(state new_state);

 private:
  clock::time_point state_update_time_;
};
}  // namespace modification::client::vehicle

#endif  // ARCANE_CLIENT_SRC_VEHICLE_ACCELERATION_H
