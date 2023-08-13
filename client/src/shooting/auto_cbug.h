#ifndef ARCANE_CLIENT_SRC_SHOOTING_AUTO_CBUG_H
#define ARCANE_CLIENT_SRC_SHOOTING_AUTO_CBUG_H

#include <chrono>
#include <arcane_packets/configuration.hpp>

namespace modification::client::shooting {
class auto_cbug {
 public:
  auto_cbug();

  void process();

 public:
  struct packets::configuration::auto_cbug settings;

 private:
  using clock = std::chrono::steady_clock;
  enum class state { no, start, attack, aim_and_squat } state_;

 private:
  void update_state(state new_state);

 private:
  clock::time_point state_update_time_;
};
}  // namespace modification::client::shooting

#endif  // ARCANE_CLIENT_SRC_SHOOTING_AUTO_CBUG_H
