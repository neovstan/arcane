#include "weapon.h"

#include "psdk_utils.h"

using namespace psdk_utils;

CWeaponInfo& weapon::get_info(const eWeaponType weapon) {
  return *CWeaponInfo::GetWeaponInfo(weapon, player()->m_nWeaponSkill);
}
