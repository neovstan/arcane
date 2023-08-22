#ifndef ARCANE_CLIENT_SRC_GAME_LOGIC_SIGNALS_HPP
#define ARCANE_CLIENT_SRC_GAME_LOGIC_SIGNALS_HPP

#include <kthook/kthook.hpp>
#include <psdk_utils/local_vector.h>

class CPed;
class CCam;
class CPlayerPed;
class CEventDamage;
class CColPoint;
class CEntity;
class CWeapon;
class CPlaceable;
class CAutomobile;
class IDirect3DDevice9;

namespace modification::client {
class game_logic_signals {
  using CHud_DrawAfterFade_t = void(__cdecl*)();
  using CBirds_HandleGunShot_t = void(__cdecl*)(psdk_utils::local_vector* start,
                                                psdk_utils::local_vector* end);

  using CPlayerPed_Compute3rdPersonMouseTarget = void(__thiscall*)(CPlayerPed* player_ped,
                                                                   bool melee);

  using CWorld_ProcessLineOfSight = bool(__cdecl*)(psdk_utils::local_vector* start,
                                                   psdk_utils::local_vector* end,
                                                   CColPoint* colpoint, CEntity** entity,
                                                   bool buildings, bool vehicles, bool peds,
                                                   bool objects, bool dummies, bool see_through,
                                                   bool camera_ignore, bool shoot_through);

  using CPlaceable_SetHeading = int(__thiscall*)(CPlaceable* placeable, float heading);

  using IDirect3DDevice9_Present = HRESULT(__stdcall*)(IDirect3DDevice9* device, const RECT* source,
                                                       const RECT* dest, HWND window,
                                                       const RGNDATA* dirty_region);

  using CPad_UpdatePads = bool(__cdecl*)();
  using CEventDamage_ComputeDamageAnim = void(__thiscall*)(CEventDamage* event, CPed* ped,
                                                           bool flag);

  using CCam_Process_FollowPed = void(__thiscall*)(CCam* camera, float* a2, int a3, int a4, int a5,
                                                   bool a6);

  using CWeapon_Fire = bool(__thiscall*)(CWeapon* weapon,
                                          CPed* owner,
                                          psdk_utils::local_vector* target,
                                          psdk_utils::local_vector* start,
                                          CEntity* victim,
                                          psdk_utils::local_vector* pp_target,
                                          psdk_utils::local_vector* target_pos);

  using D3D9DeviceReleaseVideoMemory = void(__cdecl*)();
  using CAutomobile_NitrousControl = void(__thiscall*)(CAutomobile* automobile, char set_boosts);

 public:
  kthook::kthook_signal<CHud_DrawAfterFade_t> main_loop{0x58D490};
  kthook::kthook_signal<CBirds_HandleGunShot_t> gun_shot{0x712E40};
  kthook::kthook_signal<CPlayerPed_Compute3rdPersonMouseTarget> compute_mouse_target{0x60B650};
  kthook::kthook_signal<IDirect3DDevice9_Present> present{};
  kthook::kthook_signal<D3D9DeviceReleaseVideoMemory> reset{0x7F7F70};
  kthook::kthook_signal<CPad_UpdatePads> update_pads{0x541DD0};
  kthook::kthook_signal<CAutomobile_NitrousControl> nitrous_control{0x6A3EA0};
  kthook::kthook_signal<CWeapon_Fire> weapon_fire{0x742300};
  kthook::kthook_simple<CCam_Process_FollowPed> process_follow_ped{0x522D40};
  kthook::kthook_simple<CWorld_ProcessLineOfSight> aim_point{0x56BA00};
  kthook::kthook_simple<CPlaceable_SetHeading> set_heading{0x43E0C0};
  kthook::kthook_simple<CEventDamage_ComputeDamageAnim> compute_damage_anim{0x4B3FC0};

  void single_shot(auto func) {
    main_loop.after += [func](const auto& hook) {
      static auto done = false;
      if (!done) {
        if constexpr (std::is_same_v<bool, decltype(func())>) {
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
