#ifndef ARCANE_CLIENT_SRC_ACTOR_CLIP_H
#define ARCANE_CLIENT_SRC_ACTOR_CLIP_H

namespace modification::client::actor {
class infinite_clip {
 public:
  enum class order : bool { no, not_decrease_ammo_in_clip };

 public:
  order process(bool enabled);
};
}

#endif  // ARCANE_CLIENT_SRC_ACTOR_CLIP_H
