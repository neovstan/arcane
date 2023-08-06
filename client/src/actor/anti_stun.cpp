#include "anti_stun.h"

#include <utils/randomizer.hpp>

using namespace modification::client::actor;

anti_stun::order anti_stun::process(const data& settings) {
  if (settings.enable && (settings.chance >= utils::randomizer<int>{0, 100}())) {
    return order::not_execute_compute_damage_anim_for_local_player;
  }
  return order::no;
}
