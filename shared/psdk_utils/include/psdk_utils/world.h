#ifndef PSDK_UTILS_WORLD_H
#define PSDK_UTILS_WORLD_H

#include <CWorld.h>

namespace psdk_utils {
namespace world {
inline bool is_line_of_sight_clear(const CVector& origin, const CVector& target,
                                   const bool buildings, const bool vehicles,
                                   const bool peds, const bool objects,
                                   const bool dummies,
                                   const bool do_see_through_check,
                                   const bool do_camera_ignore_check) {
  return CWorld::GetIsLineOfSightClear(
      origin, target, buildings, vehicles, peds, objects, dummies,
      do_see_through_check, do_camera_ignore_check);
}
}  // namespace world
}  // namespace psdk_utils

#endif  // PSDK_UTILS_WORLD_H
