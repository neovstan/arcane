#ifndef ARCANE_CLIENT_SRC_INJECTION_IN_GAME_LOGIC_H
#define ARCANE_CLIENT_SRC_INJECTION_IN_GAME_LOGIC_H

#include "game_logic_signals.hpp"

namespace modification::client {
class injection_in_game_logic {
 public:
  injection_in_game_logic();
  injection_in_game_logic(const injection_in_game_logic&) = delete;
  injection_in_game_logic(injection_in_game_logic&&) = delete;

 private:
  void patch_samp_anticheat();

 private:
  void load_vector_aimbot();
  void load_silent_aimbot();
  void load_auto_shot();
  void load_auto_cbug();
  void load_visuals();

 private:
  game_logic_signals signals;
};
}  // namespace modification::client

#endif  // ARCANE_CLIENT_SRC_INJECTION_IN_GAME_LOGIC_H
