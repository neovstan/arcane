#ifndef ARCANE_CLIENT_SRC_GAME_LOGIC_SIGNALS_HPP
#define ARCANE_CLIENT_SRC_GAME_LOGIC_SIGNALS_HPP

#include <kthook/kthook.hpp>
#include <psdk_utils/local_vector.h>

class CPed;
class CCam;
class CTask;
class RpClump;
class CMatrix;
class CVehicle;
class CPlayerPed;
class CEventDamage;
class CColPoint;
class CEntity;
class CWeapon;
class CPlaceable;
class CAutomobile;
class CTaskManager;
class CAnimBlendAssociation;
class CPedDamageResponseCalculator;
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

  using CPed_ProcessControl = void(__thiscall*)(CPed* ped);
  using CPlayerPed_HandleSprintEnergy = bool(__thiscall*)(CPlayerPed* ped, bool a2, float a3);
  using CWeapon_Fire = bool(__thiscall*)(CWeapon* weapon, CPed* owner,
                                         psdk_utils::local_vector* target,
                                         psdk_utils::local_vector* start, CEntity* victim,
                                         psdk_utils::local_vector* pp_target,
                                         psdk_utils::local_vector* target_pos);

  using D3D9DeviceReleaseVideoMemory = void(__cdecl*)();
  using CAutomobile_NitrousControl = void(__thiscall*)(CAutomobile* automobile, char set_boosts);
  using CAutomobile_PreRender = CMatrix*(__thiscall*)(CAutomobile* automobile);

  using CAnimManager_BlendAnimation = CAnimBlendAssociation*(__cdecl*)(RpClump* clump, int group,
                                                                       int id, float delta);

  using CPedDamageResponseCalculator_ComputeWillKillPed =
      void(__thiscall*)(CPedDamageResponseCalculator* calculator, CPed* ped, int unk1, bool unk2);

  using CVehicle_VehicleDamage = void(__thiscall*)(CVehicle* vehicle, float damage, uint16_t comp,
                                                   void* damager, RwV3d* col_coors, RwV3d* col_dir,
                                                   unsigned int weapon);

  using CVehicle_BurstTyre = bool(__thiscall*)(CVehicle* vehicle, uint8_t comp_id, bool unk);
  using CVehicle_CanVehicleBeDamaged = bool(__thiscall*)(CVehicle* vehicle, CEntity* damager,
                                                         unsigned int weapon, unsigned char* unk);

 public:
  kthook::kthook_signal<CHud_DrawAfterFade_t> main_loop{0x58D490};
  kthook::kthook_signal<CBirds_HandleGunShot_t> gun_shot{0x712E40};
  kthook::kthook_signal<CPlayerPed_Compute3rdPersonMouseTarget> compute_mouse_target{0x60B650};
  kthook::kthook_signal<IDirect3DDevice9_Present> present{};
  kthook::kthook_signal<D3D9DeviceReleaseVideoMemory> reset{0x7F7F70};
  kthook::kthook_signal<CPad_UpdatePads> update_pads{0x541DD0};
  kthook::kthook_signal<CAutomobile_NitrousControl> nitrous_control{0x6A3EA0};
  kthook::kthook_signal<CWeapon_Fire> weapon_fire{0x742300};
  kthook::kthook_simple<CPlayerPed_HandleSprintEnergy> handle_sprint_energy{0x60A550};
  kthook::kthook_simple<CWorld_ProcessLineOfSight> aim_point{0x56BA00};
  kthook::kthook_simple<CPlaceable_SetHeading> set_heading{0x43E0C0};
  kthook::kthook_simple<CEventDamage_ComputeDamageAnim> compute_damage_anim{0x4B3FC0};
  kthook::kthook_simple<CPedDamageResponseCalculator_ComputeWillKillPed> compute_will_kill_ped{
      0x4B3210};
  kthook::kthook_simple<CVehicle_CanVehicleBeDamaged> can_vehicle_be_damaged{0x6D1280};
  kthook::kthook_simple<CAnimManager_BlendAnimation> blend_animation{0x4D4610};
  kthook::kthook_simple<CVehicle_VehicleDamage> vehicle_damage_automobile{0x6A7650};
  kthook::kthook_simple<CVehicle_VehicleDamage> vehicle_damage_bike{0x6B8EC0};
  kthook::kthook_simple<CVehicle_BurstTyre> burst_tyre_automobile{0x6A32B0};
  kthook::kthook_simple<CVehicle_BurstTyre> burst_tyre_bike{0x6BEB20};
  kthook::kthook_simple<CAutomobile_PreRender> pre_render{0x6AAB50};

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
