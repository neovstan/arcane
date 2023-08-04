#include "psdk_utils.h"

#include <d3d9.h>
#include <CPools.h>
#include <CSprite.h>
#include <common.h>
#include <ePedBones.h>

#include "cmath.h"
#include "camera.h"
#include "world.h"
#include "weapon.h"

CPlayerPed* psdk_utils::player() {
  return reinterpret_cast<CPlayerPed*>(CPools::ms_pPedPool->GetAt(0));
}

std::vector<CPed*> psdk_utils::peds_around() {
  std::vector<CPed*> peds{};
  auto& ped_pool = CPools::ms_pPedPool;
  for (auto i = 1; i < ped_pool->m_nSize; ++i) {
    if (auto ped = ped_pool->GetAt(i)) {
      peds.push_back(ped);
    }
  }
  return peds;
}

bool psdk_utils::is_ped_playing_anim(CPed* ped, const char* anim) {
  return RpAnimBlendClumpGetAssociation(ped->m_pRwClump, anim);
}

bool psdk_utils::is_ped_stunned(CPed* ped) {
  for (const auto& i :
       {"DAM_armL_frmBK", "DAM_armL_frmFT", "DAM_armL_frmLT", "DAM_armR_frmBK", "DAM_armR_frmFT",
        "DAM_armR_frmRT", "DAM_LegL_frmBK", "DAM_LegL_frmFT", "DAM_LegL_frmLT", "DAM_LegR_frmBK",
        "DAM_LegR_frmFT", "DAM_LegR_frmRT", "DAM_stomach_frmBK", "DAM_stomach_frmFT",
        "DAM_stomach_frmLT", "DAM_stomach_frmRT"}) {
    if (is_ped_playing_anim(ped, i)) {
      return true;
    }
  }

  return false;
}

psdk_utils::local_vector psdk_utils::bone_position(CPed* ped, int bone, bool update) {
  RwV3d vec{};
  ped->GetBonePosition(vec, bone, update);
  return vec;
}

psdk_utils::local_vector psdk_utils::calc_screen_coors(const local_vector& in) {
  RwV3d out{};
  float w{}, h{};
  if (CSprite::CalcScreenCoors(in, &out, &w, &h, true, true)) {
    return {out.x, out.y, 0.0f};
  }
  return {-1.0f, -1.0f, -1.0f};
}

_D3DPRESENT_PARAMETERS_* psdk_utils::d3d_present_params() {
  return reinterpret_cast<_D3DPRESENT_PARAMETERS_*>(0xC9C040);
}

HWND psdk_utils::hwnd() {
  return RsGlobal.ps->window;
}

psdk_utils::local_vector psdk_utils::resolution() {
  const auto params = d3d_present_params();
  return {static_cast<float>(params->BackBufferWidth), static_cast<float>(params->BackBufferHeight),
          0.0f};
}

psdk_utils::nearest_bone psdk_utils::find_bone_making_minimum_angle_with_camera(
    CPed* ped, bool check_for_obstacles, bool check_for_distance, bool divide_angle_by_distance,
    float max_angle_in_degrees, float min_distance, bool use_target_range_instead_of_weapons,
    bool head, bool neck, bool right_shoulder, bool left_shoulder, bool right_elbow,
    bool left_elbow, bool stomach, bool right_knee, bool left_knee) {
  const auto& cam = get_active_cam();

  nearest_bone data{};
  data.angle_to_sight = max_angle_in_degrees;

  auto bone = [](const bool enabled = false, const int id = 0) {
    return enabled ? id : 0;
  };

  for (const auto i :
       {bone(head, BONE_HEAD), bone(neck, BONE_NECK), bone(right_shoulder, BONE_RIGHTSHOULDER),
        bone(left_shoulder, BONE_LEFTSHOULDER), bone(right_elbow, BONE_RIGHTELBOW),
        bone(left_elbow, BONE_LEFTELBOW), bone(stomach, BONE_SPINE1),
        bone(right_knee, BONE_RIGHTKNEE), bone(left_knee, BONE_LEFTKNEE)}) {
    if (i == bone()) continue;

    const local_vector bone_pos{bone_position(ped, i)}, &cam_pos{get_camera().GetPosition()},
        &ped_pos{ped->GetPosition()}, &player_pos{player()->GetPosition()};

    if (check_for_obstacles && !world::is_line_of_sight_clear(cam_pos, bone_pos, true, true, false,
                                                              true, true, true, true)) {
      continue;
    }

    const auto difference_vector = cam_pos - bone_pos;

    if (difference_vector.r() < (cam_pos - player_pos).r()) {
      continue;
    }

    const auto distance = (player_pos - ped_pos).r();

    if ((check_for_distance &&
         distance > (use_target_range_instead_of_weapons
                         ? weapon::get_info(weapon_in_hand()).m_fTargetRange
                         : weapon::get_info(weapon_in_hand()).m_fWeaponRange)) ||
        (distance < min_distance)) {
      continue;
    }

    const auto angle = local_vector{difference_vector.a_xy(), difference_vector.a_z()} +
                       camera::crosshair_offset();

    const auto difference =
        math::difference((angle - local_vector{math::rad2deg(cam.m_fHorizontalAngle),
                                               math::rad2deg(cam.m_fVerticalAngle)})
                             .r());

    const auto max_angle = [&]() {
      auto angle = max_angle_in_degrees;
      if (divide_angle_by_distance) {
        angle /= difference_vector.r();
      }
      return angle;
    }();

    if (difference > max_angle) continue;

    if (difference < data.angle_to_sight) {
      data.angle_to_sight = difference;
      data.desired_angle = angle;
      data.world_position = bone_pos;
      data.existing = true;
    }
  }

  return data;
}
