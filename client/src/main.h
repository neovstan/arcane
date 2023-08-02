#ifndef ARCANE_CLIENT_SRC_CLIENT_H
#define ARCANE_CLIENT_SRC_CLIENT_H

#include <mutex>
#include <thread>

// kthook
#include <kthook/kthook.hpp>

#include "game_logic_signals.hpp"
#include "plugin.h"
#include "shooting/auto_cbug.h"
#include "shooting/auto_shot.h"
#include "shooting/silent_aimbot.h"
#include "shooting/vector_aimbot.h"
#include "visuals/visuals.h"

// d3d9
#include <d3d9.h>

namespace modification::client {
class main : public singleton<main> {
  friend class singleton<main>;

  ~main();
  main();

  void update_loop();

  std::atomic<bool> break_thread;

  game_logic_signals signals;

  shooting::vector_aimbot vector_aimbot;
  shooting::silent_aimbot silent_aimbot;
  shooting::auto_cbug auto_cbug;
  shooting::auto_shot auto_shot;

  visuals::visuals visuals{};

  std::recursive_mutex mtx{};
  bool is_aiming_at_person{};
  bool was_last_compute_mouse_target_caller_local_player{};
  bool unloaded{};

 public:
  std::tuple<bool, shooting::enemy_finder::settings> vector_aimbot_finder_settings(
      const psdk_utils::weapon::mode mode) {
    return {vector_aimbot.settings[mode].enable, vector_aimbot.get_settings(mode)};
  }
  std::tuple<bool, shooting::enemy_finder::settings> silent_aimbot_finder_settings(
      const psdk_utils::weapon::mode mode) {
    return {silent_aimbot.settings[mode].enable, silent_aimbot.get_settings(mode)};
  }
};
}  // namespace modification::client

#endif  // ARCANE_CLIENT_SRC_CLIENT_H
