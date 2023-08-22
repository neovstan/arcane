#ifndef ARCANE_CLIENT_SRC_ACTOR_DISABLE_CAMERA_RESET_H
#define ARCANE_CLIENT_SRC_ACTOR_DISABLE_CAMERA_RESET_H

#include <arcane_packets/configuration.hpp>

namespace modification::client::actor {
class disable_camera_reset {
  using data = packets::configuration::actor::disable_camera_reset_data;

 public:
  enum class order {
    no,
    not_reset_camera_horizontal,
    not_reset_camera_vertical,
    not_reset_camera_all
  };

 public:
  order process(const data& settings);
};
}  // namespace modification::client::actor

#endif  // ARCANE_CLIENT_SRC_ACTOR_DISABLE_CAMERA_RESET_H
