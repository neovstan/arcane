#ifndef ARCANE_CLIENT_SRC_INJECTION_IN_GAME_LOGIC_H
#define ARCANE_CLIENT_SRC_INJECTION_IN_GAME_LOGIC_H

#include <string_view>

#include "game_logic_signals.hpp"

#include "patches/fast_run.h"
#include "patches/camera_reset.h"
#include "patches/drive_on_water.h"

#include "shooting/auto_cbug.h"
#include "shooting/auto_shot.h"
#include "shooting/silent_aimbot.h"
#include "shooting/vector_aimbot.h"

#include "visuals/visuals.h"
#include "vehicle/vehicle.h"
#include "actor/actor.h"
#include "miscellaneous/miscellaneous.h"

#include "client.h"

namespace modification::client {
class injection_in_game_logic {
  friend class client;

 public:
  injection_in_game_logic(std::string_view username, std::string_view password,
                          std::string_view hwid);
  injection_in_game_logic(const injection_in_game_logic&) = delete;
  injection_in_game_logic(injection_in_game_logic&&) = delete;

 public:
  shooting::vector_aimbot vector_aimbot;
  shooting::silent_aimbot silent_aimbot;
  shooting::auto_cbug auto_cbug;
  shooting::auto_shot auto_shot;

 public:
  visuals::visuals visuals;
  actor::actor actor;
  vehicle::vehicle vehicle;
  miscellaneous::miscellaneous miscellaneous;

 private:
  static void load_debug_console();
  static void load_imgui_context();
  void load_anticheat_patch();
  void load_keys();
  void load_samp();
  void load_unload();
  void load_vector_aimbot();
  void load_silent_aimbot();
  void load_auto_shot();
  void load_auto_cbug();
  void load_visuals();
  void load_vehicle();
  void load_actor();

 private:  // details of loads
  game_logic_signals signals_;
  patches::fast_run fast_run_patch_;
  patches::camera_reset camera_reset_patch_;
  patches::drive_on_water drive_on_water_patch_;
  bool is_aiming_at_person_;
  bool was_last_compute_mouse_target_caller_local_player_;

 private:  // things for multithreading
  std::atomic_bool has_to_break_thread_;

 private:
  const std::string username_;
  const std::string password_;
  const std::string hwid_;
};
}  // namespace modification::client

#endif  // ARCANE_CLIENT_SRC_INJECTION_IN_GAME_LOGIC_H
