#include "repair_car.h"

#include <psdk_utils/psdk_utils.h>
#include <samp_utils/samp_utils.h>

using namespace modification::client::vehicle;

void repair_car::process(const repair_car::data& settings) {
  if (!settings.enable || samp_utils::is_cursor_enabled()) return;

  const auto vehicle = psdk_utils::player()->m_pVehicle;
  if (!vehicle) return;

  if (psdk_utils::key::pressed(settings.key)) {
    vehicle->m_fHealth = 1000.0f;
    vehicle->Fix();
  }
}
