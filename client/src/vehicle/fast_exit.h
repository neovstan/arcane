#ifndef ARCANE_CLIENT_SRC_VEHICLE_FAST_EXIT_H
#define ARCANE_CLIENT_SRC_VEHICLE_FAST_EXIT_H

#include <arcane_packets/configuration.hpp>

namespace modification::client::vehicle {
class fast_exit {
 public:
  fast_exit();

  void process(bool enabled);

 private:
  using clock = std::chrono::steady_clock;
  enum class state { no, reset_animation } state_;

 private:
  void update_state(state new_state);

 private:
  clock::time_point state_update_time_;
};
}  // namespace modification::client::vehicle

#endif  // ARCANE_CLIENT_SRC_VEHICLE_FAST_EXIT_H
