#ifndef ARCANE_CLIENT_SRC_VEHICLE_INFINITE_HEALTH_H
#define ARCANE_CLIENT_SRC_VEHICLE_INFINITE_HEALTH_H

namespace modification::client::vehicle {
class infinite_health {
 public:
  enum class order : bool { no, not_decrease_vehicle_health };

 public:
  order process(bool enabled);
};
}  // namespace modification::client::vehicle

#endif  // ARCANE_CLIENT_SRC_VEHICLE_INFINITE_HEALTH_H
