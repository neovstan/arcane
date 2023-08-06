#ifndef ARCANE_CLIENT_SRC_GAME_LOGIC_SIGNALS_HPP
#define ARCANE_CLIENT_SRC_GAME_LOGIC_SIGNALS_HPP

#include <kthook/kthook.hpp>
#include <psdk_utils/local_vector.h>

class CPed;
class CPlayerPed;
class CEventDamage;
class CColPoint;
class CEntity;
class CPlaceable;
class IDirect3DDevice9;

namespace modification::client {
class game_logic_signals {
  using CHud__DrawAfterFade_t = void(__cdecl*)();
  using CBirds__HandleGunShot_t = void(__cdecl*)(psdk_utils::local_vector* start,
                                                 psdk_utils::local_vector* end);

  using CPlayerPed__Compute3rdPersonMouseTarget = void(__thiscall*)(CPlayerPed* player_ped,
                                                                    bool melee);

  using CWorld__ProcessLineOfSight = bool(__cdecl*)(psdk_utils::local_vector* start,
                                                    psdk_utils::local_vector* end,
                                                    CColPoint* colpoint, CEntity** entity,
                                                    bool buildings, bool vehicles, bool peds,
                                                    bool objects, bool dummies, bool see_through,
                                                    bool camera_ignore, bool shoot_through);

  using CPlaceable__SetHeading = int(__thiscall*)(CPlaceable* placeable, float heading);

  using IDirect3DDevice9_Present = HRESULT(__stdcall*)(IDirect3DDevice9* device, const RECT* source,
                                                       const RECT* dest, HWND window,
                                                       const RGNDATA* dirty_region);

  using CPad__UpdatePads = bool(__cdecl*)();
  using CEventDamage__ComputeDamageAnim = void(__thiscall*)(CEventDamage* event, CPed* ped,
                                                            bool flag);

 public:
  kthook::kthook_signal<CHud__DrawAfterFade_t> main_loop{0x58D490};
  kthook::kthook_signal<CBirds__HandleGunShot_t> gun_shot{0x712E40};
  kthook::kthook_signal<CPlayerPed__Compute3rdPersonMouseTarget> compute_mouse_target{0x60B650};
  kthook::kthook_signal<IDirect3DDevice9_Present> present{};
  kthook::kthook_signal<CPad__UpdatePads> update_pads{0x541DD0};
  kthook::kthook_simple<CWorld__ProcessLineOfSight> aim_point{0x56BA00};
  kthook::kthook_simple<CPlaceable__SetHeading> set_heading{0x43E0C0};
  kthook::kthook_simple<CEventDamage__ComputeDamageAnim> compute_damage_anim{0x4B3FC0};

  void single_shot(auto func) {
    main_loop.after += [func](const auto& hook) {
      static auto done = false;
      if (!done) {
        if constexpr (!std::is_void_v<decltype(func())>) {
          if (!func()) return;
        } else {
          func();
        }
        done = true;
      }
    };
  }

  void loop(auto func) {
    single_shot([func]() {
      func();
      return false;
    });
  }

  const std::uintptr_t aim_point_return_address{0x60B83D};
  const std::uintptr_t set_heading_return_address{0x522C48};
};
}  // namespace modification::client

#endif  // ARCANE_CLIENT_SRC_GAME_LOGIC_SIGNALS_HPP
