#ifndef PSDK_UTILS_WORLD_H
#define PSDK_UTILS_WORLD_H

#include "local_vector.h"

namespace psdk_utils {
namespace world {
bool is_line_of_sight_clear(const local_vector& origin, const local_vector& target, bool buildings,
                            bool vehicles, bool peds, bool objects, bool dummies,
                            bool do_see_through_check, bool do_camera_ignore_check);
}  // namespace world
}  // namespace psdk_utils

#endif  // PSDK_UTILS_WORLD_H
