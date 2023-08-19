#ifndef ARCANE_CLIENT_SRC_ACTOR_ACTOR_H
#define ARCANE_CLIENT_SRC_ACTOR_ACTOR_H

#include <arcane_packets/configuration.hpp>

#include "air_walking.h"
#include "anti_stun.h"
#include "fast_run.h"
#include "spread_control.h"

namespace modification::client::actor {
class actor {
 public:
  void process();
  anti_stun::order process_anti_stun();
  void process_fast_run(float& out_run_speed);

 public:
  struct packets::configuration::actor settings;

 private:
  air_walking air_walking;
  anti_stun anti_stun;
  fast_run fast_run;
  spread_control spread_control;
};
}  // namespace modification::client::actor

#endif  // ARCANE_CLIENT_SRC_ACTOR_ACTOR_H
