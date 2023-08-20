#ifndef ARCANE_CLIENT_SRC_MAIN_H
#define ARCANE_CLIENT_SRC_MAIN_H

#include <memory>

#include "plugin.h"
#include "client.h"
#include "unload.h"
#include "injection_in_game_logic.h"

namespace modification::client {
class main : public singleton<main> {
  friend class singleton<main>;

  main();
  ~main();

  std::shared_ptr<injection_in_game_logic> injection_;
  std::unique_ptr<client> client_;
  std::unique_ptr<unload> unload_;

 public:
  std::tuple<bool, shooting::enemy_finder::settings> vector_aimbot_finder_settings(
      const psdk_utils::weapon::mode mode) {
    return {injection_->vector_aimbot.settings[mode].enable,
            injection_->vector_aimbot.get_settings(mode)};
  }
  std::tuple<bool, shooting::enemy_finder::settings> silent_aimbot_finder_settings(
      const psdk_utils::weapon::mode mode) {
    return {injection_->silent_aimbot.settings[mode].enable,
            injection_->silent_aimbot.get_settings(mode)};
  }

  [[nodiscard]] const std::unique_ptr<client>& client() const {
    return client_;
  }

  [[nodiscard]] const std::unique_ptr<unload>& unload() const {
    return unload_;
  }
};
}  // namespace modification::client

#endif  // ARCANE_CLIENT_SRC_MAIN_H
