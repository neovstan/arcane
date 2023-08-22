#ifndef ARCANE_CLIENT_SRC_PATCHES_CAMERA_RESET_H
#define ARCANE_CLIENT_SRC_PATCHES_CAMERA_RESET_H

#include <array>

namespace modification::client::patches {
class camera_reset {
 public:
  camera_reset();
  camera_reset(const camera_reset&) = delete;
  camera_reset(camera_reset&&) = delete;
  ~camera_reset();

 public:
  void horizontal() const;
  void vertical() const;
  void restore() const;

 private:
  const std::array<std::intptr_t, 3> addresses_;
  const std::uint8_t default_value_;
};
}  // namespace modification::client::patches

#endif  // ARCANE_CLIENT_SRC_PATCHES_CAMERA_RESET_H
