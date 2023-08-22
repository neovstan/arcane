#ifndef ARCANE_CLIENT_SRC_ACTOR_INFINITE_RUN_H
#define ARCANE_CLIENT_SRC_ACTOR_INFINITE_RUN_H

namespace modification::client::actor {
class infinite_run {
 public:
  enum class order : bool { no, not_decrease_time_can_run };

 public:
  order process(bool enabled);
};
}  // namespace modification::client::actor

#endif  // ARCANE_CLIENT_SRC_ACTOR_INFINITE_RUN_H
