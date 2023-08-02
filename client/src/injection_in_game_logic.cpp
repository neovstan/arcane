#include "injection_in_game_logic.h"

#include <samp_utils/samp_utils.h>

using namespace modification::client;

injection_in_game_logic::injection_in_game_logic() {
  patch_samp_anticheat();
  load_vector_aimbot();
  load_silent_aimbot();
  load_auto_shot();
  load_auto_cbug();
  load_visuals();
}

void injection_in_game_logic::patch_samp_anticheat() {
  // signals.main_loop.after([this](const auto& hook) { samp_utils::patch_anticheat(); });
}

void injection_in_game_logic::load_vector_aimbot() {
}

void injection_in_game_logic::load_silent_aimbot() {
}

void injection_in_game_logic::load_auto_shot() {
}

void injection_in_game_logic::load_auto_cbug() {
}

void injection_in_game_logic::load_visuals() {
}
