#include "flip_car.h"

#include <psdk_utils/psdk_utils.h>
#include <samp_utils/samp_utils.h>

using namespace modification::client::vehicle;

void flip_car::process(const flip_car::data& settings) {
  if (!settings.enable || samp_utils::is_cursor_enabled()) return;

  auto player = psdk_utils::player();

  if (player->m_nPedState != PEDSTATE_DRIVING || player->m_pVehicle == nullptr) return;

  if (psdk_utils::key::pressed(settings.key)) {
    player->m_pVehicle->SetOrientation(0.f, 0.f, player->m_pVehicle->GetHeading());
  }
}
