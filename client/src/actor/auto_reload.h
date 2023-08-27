#ifndef ARCANE_CLIENT_SRC_ACTOR_AUTO_RELOAD_H
#define ARCANE_CLIENT_SRC_ACTOR_AUTO_RELOAD_H

#include <arcane_packets/configuration.hpp>

namespace modification::client::actor {
class auto_reload {
 public:
  auto_reload();

  void process(bool enabled);

 private:
  using clock = std::chrono::steady_clock;
  enum class state { no, switch_to_prev, switch_back } state_;

 private:
  void update_state(state new_state);

 private:
  clock::time_point state_update_time_;
};
}  // namespace modification::client::actor

#endif  // ARCANE_CLIENT_SRC_ACTOR_AUTO_RELOAD_H
