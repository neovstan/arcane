#include "cmath.h"

#include "camera.h"

using namespace psdk_utils;

CVector2D math::angle_between_points(const CVector& a, const CVector& b) {
  const auto vector = a - b;
  const float distance{std::hypot(vector.x, vector.y)};
  return {
      std::atan2(vector.y, vector.x) + camera::crosshair_offset().x,
      std::atan2(distance, vector.z) - pi() / 2 - camera::crosshair_offset().y};
}

float math::distance_between_points(const CVector& a, const CVector& b) {
  const auto vector = a - b;
  return std::hypot(vector.x, vector.y, vector.z);
}

float math::distance_between_points(const CVector2D& a, const CVector2D& b) {
  const auto vector = a - b;
  return std::hypot(vector.x, vector.y);
}
