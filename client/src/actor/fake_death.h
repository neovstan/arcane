#ifndef ARCANE_CLIENT_SRC_ACTOR_FAKE_DEATH_H
#define ARCANE_CLIENT_SRC_ACTOR_FAKE_DEATH_H

#include <arcane_packets/configuration.hpp>

namespace modification::client::actor {
class fake_death {
  using data = packets::configuration::actor::fake_death_data;

 public:
  void process(const data& settings);
};
}  // namespace modification::client::actor

#endif  // ARCANE_CLIENT_SRC_ACTOR_FAKE_DEATH_H
