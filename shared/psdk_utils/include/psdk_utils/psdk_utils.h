#ifndef PSDK_UTILS_PSDK_UTILS_H
#define PSDK_UTILS_PSDK_UTILS_H

#include <CPlayerPed.h>
#include <d3d9.h>

#include <vector>

#include "camera.h"
#include "cmath.h"
#include "key.h"
#include "weapon.h"
#include "world.h"

namespace psdk_utils {
enum game_key_state { no, down = 128 };

CPlayerPed* player();
std::vector<CPed*> peds_around();
bool is_ped_playing_anim(CPed* const ped, const char* const anim);
bool is_ped_stunned(CPed* const ped);
CVector bone_position(CPed* const ped, const int bone,
                      const bool update = true);
CVector calc_screen_coors(const CVector& in);
D3DPRESENT_PARAMETERS& d3d_present_params();
CVector2D resolution();

struct nearest_bone {
  bool existing{};
  float angle_to_sight{};
  CVector2D desired_angle{};
  CVector world_position{};
};

nearest_bone find_bone_making_minimum_angle_with_camera(
    CPed* ped, bool check_for_obstacles, bool check_for_distance,
    bool divide_angle_by_distance, float max_angle_in_degrees,
    float min_distance, bool use_target_range_instead_of_weapons = false,
    bool head = true, bool neck = true, bool right_shoulder = true,
    bool left_shoulder = true, bool right_elbow = true, bool left_elbow = true,
    bool stomach = true, bool right_knee = true, bool left_knee = true);
}  // namespace psdk_utils

#endif  // PSDK_UTILS_PSDK_UTILS_H
