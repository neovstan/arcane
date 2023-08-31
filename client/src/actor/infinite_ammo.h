#ifndef ARCANE_CLIENT_SRC_ACTOR_INFINITE_AMMO_H
#define ARCANE_CLIENT_SRC_ACTOR_INFINITE_AMMO_H

#include <arcane_packets/configuration.hpp>

namespace modification::client::actor {
class infinite_ammo {
 public:
  enum class order : bool { no, not_decrease_ammo };

 public:
  order process(bool enabled);
};
}  // namespace modification::client::actor

#endif  // ARCANE_CLIENT_SRC_ACTOR_INFINITE_AMMO_H
