#include "psdk_utils.h"

#include <CPools.h>
#include <CSprite.h>
#include <Patch.h>
#include <common.h>
#include <ePedBones.h>

CPlayerPed* psdk_utils::player() {
  return reinterpret_cast<CPlayerPed*>(CPools::ms_pPedPool->GetAt(0));
}

std::vector<CPed*> psdk_utils::peds_around() {
  std::vector<CPed*> peds{};
  auto& ped_pool = CPools::ms_pPedPool;
  for (int i{1}; i < ped_pool->m_nSize; ++i) {
    if (auto ped = ped_pool->GetAt(i)) {
      peds.push_back(ped);
    }
  }
  return peds;
}

bool psdk_utils::is_ped_playing_anim(CPed* const ped, const char* const anim) {
  return RpAnimBlendClumpGetAssociation(ped->m_pRwClump, anim);
}

bool psdk_utils::is_ped_stunned(CPed* const ped) {
  for (const auto& i :
       {"DAM_armL_frmBK", "DAM_armL_frmFT", "DAM_armL_frmLT", "DAM_armR_frmBK",
        "DAM_armR_frmFT", "DAM_armR_frmRT", "DAM_LegL_frmBK", "DAM_LegL_frmFT",
        "DAM_LegL_frmLT", "DAM_LegR_frmBK", "DAM_LegR_frmFT", "DAM_LegR_frmRT",
        "DAM_stomach_frmBK", "DAM_stomach_frmFT", "DAM_stomach_frmLT",
        "DAM_stomach_frmRT"}) {
    if (is_ped_playing_anim(ped, i)) {
      return true;
    }
  }

  return false;
}

CVector psdk_utils::bone_position(CPed* const ped, const int bone,
                                  const bool update) {
  RwV3d vec{};
  ped->GetBonePosition(vec, bone, update);
  return vec;
}

CVector psdk_utils::calc_screen_coors(const CVector& in) {
  RwV3d out{};
  float w{}, h{};
  if (CSprite::CalcScreenCoors({in.x, in.y, in.z}, &out, &w, &h, true, true)) {
    return {out.x, out.y, 0.0f};
  }
  return {-1.0f, -1.0f, -1.0f};
}

D3DPRESENT_PARAMETERS& psdk_utils::d3d_present_params() {
  return *reinterpret_cast<D3DPRESENT_PARAMETERS*>(0xC9C040);
}

CVector2D psdk_utils::resolution() {
  const auto& params = d3d_present_params();
  return {static_cast<float>(params.BackBufferWidth),
          static_cast<float>(params.BackBufferHeight)};
}

psdk_utils::nearest_bone psdk_utils::find_bone_making_minimum_angle_with_camera(
    CPed* const ped, const bool check_for_obstacles,
    const bool check_for_distance, const bool divide_angle_by_distance,
    const float max_angle_in_degrees, const float min_distance,
    const bool use_target_range_instead_of_weapons, const bool head,
    const bool neck, const bool right_shoulder, const bool left_shoulder,
    const bool right_elbow, const bool left_elbow, const bool stomach,
    const bool right_knee, const bool left_knee) {
  const auto& cam = get_active_cam();

  nearest_bone data{};
  data.angle_to_sight = math::deg2rad(max_angle_in_degrees);

  auto bone = [](const bool enabled = false, const int id = 0) {
    return enabled ? id : 0;
  };

  for (const auto i :
       {bone(head, BONE_HEAD), bone(neck, BONE_NECK),
        bone(right_shoulder, BONE_RIGHTSHOULDER),
        bone(left_shoulder, BONE_LEFTSHOULDER),
        bone(right_elbow, BONE_RIGHTELBOW), bone(left_elbow, BONE_LEFTELBOW),
        bone(stomach, BONE_SPINE1), bone(right_knee, BONE_RIGHTKNEE),
        bone(left_knee, BONE_LEFTKNEE)}) {
    if (i == bone()) continue;

    const auto bone_pos = bone_position(ped, i),
               &cam_pos = get_camera().GetPosition(),
               &ped_pos = ped->GetPosition(),
               &player_pos = player()->GetPosition();

    if (check_for_obstacles &&
        !world::is_line_of_sight_clear(cam_pos, bone_pos, true, true, false,
                                       true, true, true, true)) {
      continue;
    }

    const float distance_between_cam_and_bone{
        math::distance_between_points(cam_pos, bone_pos)};

    const float distance_between_cam_and_player{
        math::distance_between_points(cam_pos, player_pos)};

    if (distance_between_cam_and_bone < distance_between_cam_and_player) {
      continue;
    }

    const float distance{math::distance_between_points(player_pos, ped_pos)};

    if ((check_for_distance &&
         distance >
             (use_target_range_instead_of_weapons
                  ? weapon::get_info(weapon_in_hand()).m_fTargetRange
                  : weapon::get_info(weapon_in_hand()).m_fWeaponRange)) ||
        (distance < min_distance)) {
      continue;
    }

    const auto angle = math::angle_between_points(cam_pos, bone_pos);

    const float difference{math::distance_between_points(
        angle, CVector2D{cam.m_fHorizontalAngle, cam.m_fVerticalAngle})};

    const float max_angle{[&]() {
      float angle{math::deg2rad(max_angle_in_degrees)};
      if (divide_angle_by_distance) {
        angle /= distance_between_cam_and_bone;
      }
      return angle;
    }()};

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
