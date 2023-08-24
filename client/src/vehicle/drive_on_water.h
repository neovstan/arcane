#ifndef ARCANE_CLIENT_SRC_VEHICLE_DRIVE_ON_WATER_H
#define ARCANE_CLIENT_SRC_VEHICLE_DRIVE_ON_WATER_H

namespace modification::client::vehicle {
class drive_on_water {
 public:
  enum class order : bool { no, not_sink_vehicle };

 public:
  order process(bool enabled);
};
}  // namespace modification::client::vehicle

#endif  // ARCANE_CLIENT_SRC_VEHICLE_DRIVE_ON_WATER_H
