#ifndef ARCANE_CLIENT_SRC_ACTOR_ANTI_STUN_H
#define ARCANE_CLIENT_SRC_ACTOR_ANTI_STUN_H

#include <configuration/configuration.hpp>

namespace modification::client::actor {
class anti_stun {
  using data = configuration::actor::anti_stun_data;

 public:
  enum class order { no, not_execute_compute_damage_anim_for_local_player };

 public:
  order process(const data& settings);
};
}  // namespace modification::client::actor

#endif  // ARCANE_CLIENT_SRC_ACTOR_ANTI_STUN_H
