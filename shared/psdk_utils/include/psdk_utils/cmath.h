#ifndef PSDK_UTILS_MATH_H
#define PSDK_UTILS_MATH_H

#include "local_vector.h"

namespace psdk_utils {
namespace math {
constexpr float pi() {
  return 3.1415927f;
}

float abs(float value);
float sign(float value);

float min(float lhs, float rhs);
float max(float lhs, float rhs);
float clamp(float min, float value, float max);

float sqrt(float value);
float pow(float lhs, float rhs);

float deg2rad(float a);
float rad2deg(float a);

float cos(float a);
float sin(float a);

float acos(float a);
float asin(float a);

float angle(float x, float y);
float hypot(float x, float y);
float hypot(float x, float y, float z);

float heading(float a);
float difference(float a);
}  // namespace math
}  // namespace psdk_utils

#endif  // PSDK_UTILS_MATH_H
