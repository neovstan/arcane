#ifndef PSDK_UTILS_MATH_H
#define PSDK_UTILS_MATH_H

#include <CVector.h>
#include <CVector2D.h>

namespace psdk_utils {
namespace math {
constexpr float pi() { return 3.1415927f; }
constexpr float deg2rad(const float deg) { return deg / (180.0f / pi()); }

CVector2D angle_between_points(const CVector& a, const CVector& b);
float distance_between_points(const CVector& a, const CVector& b);
float distance_between_points(const CVector2D& a, const CVector2D& b);
}  // namespace math
}  // namespace psdk_utils

#endif  // PSDK_UTILS_MATH_H
