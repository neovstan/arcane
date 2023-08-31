#ifndef ARCANE_CLIENT_SRC_PATCHES_DRIVE_ON_WATER_H
#define ARCANE_CLIENT_SRC_PATCHES_DRIVE_ON_WATER_H

#include <cstdint>

namespace modification::client::patches {
class drive_on_water {
 public:
  drive_on_water();
  drive_on_water(const drive_on_water&) = delete;
  drive_on_water(drive_on_water&&) = delete;
  ~drive_on_water();

 public:
  void install() const;
  void restore() const;

 private:
  const std::intptr_t address_;
  const std::uint16_t default_value_;
};
}  // namespace modification::client::patches

#endif  // ARCANE_CLIENT_SRC_PATCHES_DRIVE_ON_WATER_H
