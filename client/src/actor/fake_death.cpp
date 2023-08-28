#include "fake_death.h"

#include <psdk_utils/psdk_utils.h>
#include <samp_utils/samp_utils.h>

using namespace modification::client::actor;

void fake_death::process(const fake_death::data& settings) {
  if (!settings.enable || samp_utils::is_cursor_enabled()) return;

  if (psdk_utils::key::pressed(settings.key)) {
    psdk_utils::player()->m_fHealth = 0.f;
  }
}
