#ifndef ARCANE_CLIENT_SRC_ACTOR_ACTOR_H
#define ARCANE_CLIENT_SRC_ACTOR_ACTOR_H

#include <configuration/configuration.hpp>

#include "air_walking.h"

namespace modification::client::actor {
class actor {
 public:
  void process();

 public:
  configuration::actor settings;

 private:
  air_walking air_walking;
};
}  // namespace modification::client::actor

#endif  // ARCANE_CLIENT_SRC_ACTOR_ACTOR_H
