#ifndef ARCANE_CLIENT_SRC_ACTOR_SPREAD_CONTROL_H
#define ARCANE_CLIENT_SRC_ACTOR_SPREAD_CONTROL_H

#include <configuration/configuration.hpp>

namespace modification::client::actor {
class spread_control {
  using data = configuration::actor::spread_control_data;

 public:
  void process(const data& settings);
};
}  // namespace modification::client::actor

#endif  // ARCANE_CLIENT_SRC_ACTOR_SPREAD_CONTROL_H
