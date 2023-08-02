#include "vector_aimbot.h"

#include <imgui.h>
#include <psdk_utils/psdk_utils.h>

using namespace modification::client::shooting;

void vector_aimbot::process() {
  const auto weapon_mode = psdk_utils::weapon::get_mode(psdk_utils::weapon_in_hand());
  if (weapon_mode == psdk_utils::weapon::mode::unknown) return;

  const auto& settings = this->settings.at(weapon_mode);

  if (!settings.enable) return;

  if (settings.aim_only_when_shooting &&
      !psdk_utils::player()->GetPadFromPlayer()->NewState.ButtonCircle) {
    return;
  }

  auto& finder = this->finder_.at(weapon_mode);
  if (const auto enemy = finder.get(); enemy.ped) {
    auto& cam = psdk_utils::get_active_cam();
    const auto smooth = settings.smooth * ImGui::GetIO().Framerate / 100.0f;

    cam.m_fHorizontalAngle += (enemy.angle.x - cam.m_fHorizontalAngle) / smooth;

    if (!settings.do_not_change_z_angle) {
      cam.m_fVerticalAngle += (enemy.angle.y - cam.m_fVerticalAngle) / smooth;
    }
  }
}
