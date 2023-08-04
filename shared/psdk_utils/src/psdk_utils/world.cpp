#include "world.h"

#include <CWorld.h>

bool psdk_utils::world::is_line_of_sight_clear(const local_vector& origin,
                                               const local_vector& target, bool buildings,
                                               bool vehicles, bool peds, bool objects, bool dummies,
                                               bool do_see_through_check,
                                               bool do_camera_ignore_check) {
  return CWorld::GetIsLineOfSightClear(origin, target, buildings, vehicles, peds, objects, dummies,
                                       do_see_through_check, do_camera_ignore_check);
}
