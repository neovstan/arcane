#ifndef ARCANE_CLIENT_SRC_MISCELLANEOUS_MISCELLANEOUS_H
#define ARCANE_CLIENT_SRC_MISCELLANEOUS_MISCELLANEOUS_H

#include <arcane_packets/configuration.hpp>

namespace modification::client::miscellaneous {
class miscellaneous {
 public:
  void process();

 public:
  struct packets::configuration::miscellaneous settings;

 private:
};
}  // namespace modification::client::miscellaneous

#endif  // ARCANE_CLIENT_SRC_MISCELLANEOUS_MISCELLANEOUS_H
