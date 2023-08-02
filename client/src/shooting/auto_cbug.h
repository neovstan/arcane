#ifndef ARCANE_CLIENT_SRC_SHOOTING_AUTO_CBUG_H
#define ARCANE_CLIENT_SRC_SHOOTING_AUTO_CBUG_H

#include <chrono>
#include <configuration/configuration.hpp>

namespace modification::client::shooting {
class auto_cbug {
 public:
  void process();

  configuration::auto_cbug settings{};

 private:
  enum class state { no, start, attack, aim_and_squat } state_;
  std::chrono::steady_clock::time_point state_update_time_{};

  void update_state(const state new_state) {
    state_ = new_state;
    state_update_time_ = std::chrono::steady_clock::now();
  }
};
}  // namespace modification::client::shooting

#endif  // ARCANE_CLIENT_SRC_SHOOTING_AUTO_CBUG_H
