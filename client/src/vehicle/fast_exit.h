#ifndef ARCANE_CLIENT_SRC_VEHICLE_FAST_EXIT_H
#define ARCANE_CLIENT_SRC_VEHICLE_FAST_EXIT_H

namespace modification::client::vehicle {
class fast_exit {
 public:
  enum class order : bool { no, no_vehicle_exit_anim };

 public:
  order process(bool enabled);
};
}  // namespace modification::client::vehicle

#endif  // ARCANE_CLIENT_SRC_VEHICLE_FAST_EXIT_H
