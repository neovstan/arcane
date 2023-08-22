#ifndef ARCANE_CLIENT_SRC_PATCHES_FAST_RUN_H
#define ARCANE_CLIENT_SRC_PATCHES_FAST_RUN_H

#include <array>

namespace modification::client::patches {
class fast_run {
 public:
  fast_run();
  fast_run(const fast_run&) = delete;
  fast_run(fast_run&&) = delete;
  ~fast_run();

 public:
  float speed{1.0f};

 private:
  const std::array<std::intptr_t, 2> addresses_;
  const std::uintptr_t default_value_;
};
}  // namespace modification::client::patches

#endif  // ARCANE_CLIENT_SRC_PATCHES_FAST_RUN_H
