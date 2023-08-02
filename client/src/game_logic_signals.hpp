#ifndef ARCANE_CLIENT_SRC_GAME_LOGIC_SIGNALS_HPP
#define ARCANE_CLIENT_SRC_GAME_LOGIC_SIGNALS_HPP

#include <kthook/kthook.hpp>

class CVector;
class CPlayerPed;
class CColPoint;
class CEntity;
class CPlaceable;
class IDirect3DDevice9;

namespace modification::client {
class game_logic_signals {
  using CHud__DrawAfterFade_t = void(__cdecl*)();
  using CBirds__HandleGunShot_t = void(__cdecl*)(CVector* start, CVector* end);

  using CPlayerPed__Compute3rdPersonMouseTarget = void(__thiscall*)(CPlayerPed* player_ped,
                                                                    bool melee);

  using CWorld__ProcessLineOfSight = bool(__cdecl*)(CVector* start, CVector* end,
                                                    CColPoint* colpoint, CEntity** entity,
                                                    bool buildings, bool vehicles, bool peds,
                                                    bool objects, bool dummies, bool see_through,
                                                    bool camera_ignore, bool shoot_through);

  using CPlaceable__SetHeading = int(__thiscall*)(CPlaceable* placeable, float heading);

  using IDirect3DDevice9_Present = HRESULT(__stdcall*)(IDirect3DDevice9* device, const RECT* source,
                                                       const RECT* dest, HWND window,
                                                       const RGNDATA* dirty_region);

  using CPad__UpdatePads = bool(__cdecl*)();

 public:
  kthook::kthook_signal<CHud__DrawAfterFade_t> main_loop{0x58D490};
  kthook::kthook_signal<CBirds__HandleGunShot_t> gun_shot{0x712E40};
  kthook::kthook_signal<CPlayerPed__Compute3rdPersonMouseTarget> compute_mouse_target{0x60B650};
  kthook::kthook_signal<IDirect3DDevice9_Present> present{};
  kthook::kthook_signal<CPad__UpdatePads> update_pads{0x541DD0};
  kthook::kthook_simple<CWorld__ProcessLineOfSight> aim_point{0x56BA00};
  kthook::kthook_simple<CPlaceable__SetHeading> set_heading{0x43E0C0};

  const std::uintptr_t aim_point_return_address{0x60B83D};
  const std::uintptr_t set_heading_return_address{0x522C48};
};
}  // namespace modification::client

#endif  // ARCANE_CLIENT_SRC_GAME_LOGIC_SIGNALS_HPP
