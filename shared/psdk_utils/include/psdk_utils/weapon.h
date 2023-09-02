#ifndef PSDK_UTILS_WEAPON_H
#define PSDK_UTILS_WEAPON_H

#include <CHud.h>
#include <CWeaponInfo.h>
#include <eWeaponType.h>

namespace psdk_utils {
inline eWeaponType weapon_in_hand() {
  return static_cast<eWeaponType>(CHud::m_LastWeapon);
}

namespace weapon {
enum class mode : int { pistols, shotguns, semi, assault, rifles, unknown };
inline mode get_mode(const eWeaponType weapon) {
  switch (weapon) {
    case WEAPON_PISTOL:
    case WEAPON_PISTOL_SILENCED:
    case WEAPON_DESERT_EAGLE:
      return mode::pistols;
    case WEAPON_SHOTGUN:
    case WEAPON_SAWNOFF:
    case WEAPON_SPAS12:
      return mode::shotguns;
    case WEAPON_MICRO_UZI:
    case WEAPON_MP5:
    case WEAPON_TEC9:
      return mode::semi;
    case WEAPON_AK47:
    case WEAPON_M4:
      return mode::assault;
    case WEAPON_COUNTRYRIFLE:
    case WEAPON_SNIPERRIFLE:
      return mode::rifles;
    default:
      return mode::unknown;
  }
}

CWeaponInfo& get_info(eWeaponType weapon);
}  // namespace weapon
}  // namespace psdk_utils

#endif  // PSDK_UTILS_WEAPON_H
