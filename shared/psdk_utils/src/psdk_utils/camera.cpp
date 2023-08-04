#include "camera.h"

#include <CDraw.h>
#include <CHud.h>
#include <CMenuManager.h>
#include <Patch.h>

#include "cmath.h"
#include "weapon.h"

using namespace psdk_utils;

local_vector camera::crosshair_offset() {
  if (weapon_in_hand() == eWeaponType::WEAPON_SNIPERRIFLE) return {};

  const auto& camera = get_camera();
  const auto mult = std::tan(math::deg2rad(get_active_cam().m_fFOV * 0.5f));

  auto fx =
      math::pi() / 2.0f - std::atan2(1.0f, mult * 2.0f * (camera.m_f3rdPersonCHairMultX - 0.5f));

  auto fz =
      math::pi() / 2.0f - std::atan2(1.0f, mult * (2.0f * (0.5f - camera.m_f3rdPersonCHairMultY) *
                                                   (1.0f / CDraw::ms_fAspectRatio)));

  if (is_widescreen_fix_on()) {
    const auto mult = 1.35f;
    fx *= mult;
    fz *= mult;
  }

  return {math::rad2deg(fx), -math::rad2deg(fz)};
}

bool camera::is_player_aiming() {
  const auto& cam = get_camera().m_aCams[get_camera().m_nActiveCam];
  return cam.m_nMode == MODE_AIMWEAPON || cam.m_nMode == MODE_SNIPER ||
         cam.m_nMode == MODE_AIMWEAPON_FROMCAR;
}

bool camera::is_widescreen_fix_on() {
  return RsGlobal.ps->fullScreen && plugin::patch::GetUInt(0x53D7B1 + 0x1) != 0x1C1C6A;
}
