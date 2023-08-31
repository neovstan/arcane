#ifndef ARCANE_CLIENT_SRC_VEHICLE_INFINITE_NITRO_H
#define ARCANE_CLIENT_SRC_VEHICLE_INFINITE_NITRO_H

namespace modification::client::vehicle {
class infinite_nitro {
 public:
  enum class order : bool { no, not_decrease_vehicle_nitro_level };

 public:
  order process(bool enabled);
};
}  // namespace modification::client::vehicle

#endif  // ARCANE_CLIENT_SRC_VEHICLE_INFINITE_NITRO_H
