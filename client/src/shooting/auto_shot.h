#ifndef ARCANE_CLIENT_SRC_SHOOTING_AUTO_SHOT_H
#define ARCANE_CLIENT_SRC_SHOOTING_AUTO_SHOT_H

#include <arcane_packets/configuration.hpp>

namespace modification::client::shooting {
class auto_shot {
 public:
  void process();

 public:
  struct packets::configuration::auto_shot settings;
};
}  // namespace modification::client::shooting

#endif  // ARCANE_CLIENT_SRC_SHOOTING_AUTO_SHOT_H
