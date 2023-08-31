#ifndef ARCANE_CLIENT_SRC_ACTOR_STAY_ON_FEET_H
#define ARCANE_CLIENT_SRC_ACTOR_STAY_ON_FEET_H

namespace modification::client::actor {
class stay_on_feet {
 public:
  enum class order : bool { no, no_fall_anim };

 public:
  order process(bool enabled);
};
}  // namespace modification::client::actor

#endif  // ARCANE_CLIENT_SRC_ACTOR_STAY_ON_FEET_H
