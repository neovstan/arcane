#ifndef ARCANE_CLIENT_SRC_ACTOR_HEAVY_FIST_H
#define ARCANE_CLIENT_SRC_ACTOR_HEAVY_FIST_H

#include <arcane_packets/configuration.hpp>

namespace modification::client::actor {
class heavy_fist {
 public:
  heavy_fist();

  void process(bool enabled);

 private:
  using clock = std::chrono::steady_clock;
  enum class state { no, process_hfist_enable, process_hfist_disable } state_;

 private:
  void update_state(state new_state);

 private:
  unsigned char last_slot_;
  clock::time_point state_update_time_;
};
}  // namespace modification::client::actor

#endif  // ARCANE_CLIENT_SRC_ACTOR_HEAVY_FIST_H
