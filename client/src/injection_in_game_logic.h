#ifndef ARCANE_CLIENT_SRC_INJECTION_IN_GAME_LOGIC_H
#define ARCANE_CLIENT_SRC_INJECTION_IN_GAME_LOGIC_H

#include <mutex>

#include "game_logic_signals.hpp"

#include "shooting/auto_cbug.h"
#include "shooting/auto_shot.h"
#include "shooting/silent_aimbot.h"
#include "shooting/vector_aimbot.h"

#include "visuals/visuals.h"

namespace modification::client {
class injection_in_game_logic {
 public:
  injection_in_game_logic();
  injection_in_game_logic(const injection_in_game_logic&) = delete;
  injection_in_game_logic(injection_in_game_logic&&) = delete;

 public:
  shooting::vector_aimbot vector_aimbot;
  shooting::silent_aimbot silent_aimbot;
  shooting::auto_cbug auto_cbug;
  shooting::auto_shot auto_shot;

 public:
  visuals::visuals visuals;

 private:
  void load_debug_console();
  void load_imgui_context();
  void load_anticheat_patch();
  void load_keys();
  void load_samp();
  void load_unload();
  void load_vector_aimbot();
  void load_silent_aimbot();
  void load_auto_shot();
  void load_auto_cbug();
  void load_visuals();

 private:
  void thread_updating_settings();

 private:  // details of loads
  game_logic_signals signals_;
  bool is_aiming_at_person_;
  bool was_last_compute_mouse_target_caller_local_player_;

 private:  // things for multithreading
  std::mutex mutex_;
  std::atomic_bool has_to_break_thread_;
};
}  // namespace modification::client

#endif  // ARCANE_CLIENT_SRC_INJECTION_IN_GAME_LOGIC_H
