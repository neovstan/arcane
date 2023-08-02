#ifndef PSDK_UTILS_KEY_H
#define PSDK_UTILS_KEY_H

#include <CCamera.h>
#include <CVector2D.h>

namespace psdk_utils::key {
void update();
bool down(unsigned int key);
bool pressed(unsigned int key);
bool released(unsigned int key);
bool down_delayed(unsigned int key, unsigned int time);
}  // namespace psdk_utils::key

#endif  // PSDK_UTILS_KEY_H
