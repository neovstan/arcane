#ifndef ARCANE_CLIENT_SRC_ACTOR_INFINITE_HEALTH_H
#define ARCANE_CLIENT_SRC_ACTOR_INFINITE_HEALTH_H

namespace modification::client::actor {
class infinite_health {
 public:
  enum class order : bool { no, not_decrease_player_health };

 public:
  order process(bool enabled);
};
}  // namespace modification::client::actor

#endif  // ARCANE_CLIENT_SRC_ACTOR_INFINITE_HEALTH_H
