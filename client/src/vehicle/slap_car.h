#ifndef ARCANE_CLIENT_SRC_VEHICLE_SLAP_CAR_H
#define ARCANE_CLIENT_SRC_VEHICLE_SLAP_CAR_H

#include <arcane_packets/configuration.hpp>

namespace modification::client::vehicle {
class slap_car {
  using data = packets::configuration::vehicle::slap_car_data;

 public:
  slap_car();

  void process(const data& settings);

 private:
  using clock = std::chrono::steady_clock;
  enum class state { no, process_slap } state_;

 private:
  void update_state(state new_state);

 private:
  clock::time_point state_update_time_;
};
}  // namespace modification::client::vehicle

#endif  // ARCANE_CLIENT_SRC_VEHICLE_SLAP_CAR_H
