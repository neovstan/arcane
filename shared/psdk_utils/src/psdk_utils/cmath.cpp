#include "cmath.h"

#include "camera.h"

using namespace psdk_utils;

float math::abs(float value) {
  return std::abs(value);
}

float math::sign(float value) {
  return value < 0.0f ? -1.0f : 1.0f;
}

float math::min(float lhs, float rhs) {
  return std::min(lhs, rhs);
}

float math::max(float lhs, float rhs) {
  return std::max(lhs, rhs);
}

float math::clamp(float min, float value, float max) {
  return std::max(min, std::min(value, max));
}

float math::sqrt(float value) {
  return std::sqrt(value);
}

float math::pow(float lhs, float rhs) {
  return std::pow(lhs, rhs);
}

float math::deg2rad(float a) {
  return a * pi() / 180.0f;
}

float math::rad2deg(float a) {
  return a / pi() * 180.0f;
}

float math::cos(float a) {
  return std::cos(math::deg2rad(a));
}

float math::sin(float a) {
  return std::sin(math::deg2rad(a));
}

float psdk_utils::math::acos(float a) {
  return math::rad2deg(std::acos(a));
}

float psdk_utils::math::asin(float a) {
  return math::rad2deg(std::asin(a));
}

float math::angle(float x, float y) {
  return math::rad2deg(std::atan2(y, x));
}

float math::hypot(float x, float y) {
  return std::hypot(x, y);
}

float math::hypot(float x, float y, float z) {
  return std::hypot(x, y, z);
}

float math::heading(float a) {
  a = std::fmod(a, 360.0f);
  return a < 0.0f ? a + 360.0f : a;
}

float math::difference(float a) {
  a = std::fmod(a + 180.0f, 360.0f);
  return a < 0.0f ? a + 180.0f : a - 180.0f;
}
