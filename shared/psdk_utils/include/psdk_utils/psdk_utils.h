#ifndef PSDK_UTILS_PSDK_UTILS_H
#define PSDK_UTILS_PSDK_UTILS_H

#include <vector>

#include <CAutomobile.h>
#include <CPlayerPed.h>
#include <CPed.h>

#include "local_vector.h"
#include "polar_vector.h"
#include "cmath.h"
#include "camera.h"
#include "key.h"
#include "weapon.h"
#include "world.h"

struct _D3DPRESENT_PARAMETERS_;

namespace psdk_utils {
enum game_key_state { no, down = 128 };

CPlayerPed* player();
std::vector<CPed*> peds_around();
bool is_ped_playing_anim(CPed* ped, const char* anim);
bool is_ped_stunned(CPed* ped);
local_vector bone_position(CPed* ped, int bone, bool update = true);
local_vector calc_screen_coors(const local_vector& in);
_D3DPRESENT_PARAMETERS_* d3d_present_params();
HWND hwnd();
local_vector resolution();

struct nearest_bone {
  bool existing;
  float angle_to_sight;
  local_vector desired_angle;
  local_vector world_position;
};

nearest_bone find_bone_making_minimum_angle_with_camera(
    CPed* ped, bool check_for_obstacles, bool check_for_distance, bool divide_angle_by_distance,
    float max_angle_in_degrees, float min_distance,
    bool use_target_range_instead_of_weapons = false, bool head = true, bool neck = true,
    bool right_shoulder = true, bool left_shoulder = true, bool right_elbow = true,
    bool left_elbow = true, bool stomach = true, bool right_knee = true, bool left_knee = true);
}  // namespace psdk_utils

#endif  // PSDK_UTILS_PSDK_UTILS_H
