#include "polar_vector.h"

#include "cmath.h"

using namespace psdk_utils;

polar_vector::polar_vector(float a, float r) : a_{a}, r_{r} {
}

float polar_vector::x() const {
  return r_ * math::cos(a_);
}

float polar_vector::y() const {
  return r_ * math::sin(a_);
}

local_vector polar_vector::local() const {
  return local_vector{x(), y(), 0.0f};
}

polar_vector psdk_utils::polar_vector::canonical() const {
  return polar_vector{math::heading(r_ < 0.0f ? a_ + 180.0f : a_), r_ < 0.0f ? -r_ : r_};
}

polar_vector polar_vector::operator+() const {
  return *this;
}

polar_vector polar_vector::operator-() const {
  return polar_vector{a_, -r_};
}

polar_vector& polar_vector::operator*=(float rhs) {
  r_ *= rhs;
  return *this;
}

polar_vector polar_vector::operator*(float rhs) const {
  return polar_vector{*this} *= rhs;
}

polar_vector& polar_vector::operator/=(float rhs) {
  r_ /= rhs;
  return *this;
}

polar_vector polar_vector::operator/(float rhs) const {
  return polar_vector{*this} /= rhs;
}

polar_vector operator*(float lhs, const polar_vector& rhs) {
  return rhs * lhs;
}

psdk_utils::polar_vector_3d::polar_vector_3d(float a_xy, float a_z, float r)
    : a_xy_{a_xy}, a_z_{a_z}, r_{r} {
}

float psdk_utils::polar_vector_3d::x() const {
  return local().x();
}

float psdk_utils::polar_vector_3d::y() const {
  return local().y();
}

float psdk_utils::polar_vector_3d::z() const {
  return local().z();
}

local_vector psdk_utils::polar_vector_3d::local() const {
  const auto z_polar = psdk_utils::polar_vector{a_z_, r_};
  auto local = psdk_utils::polar_vector{a_xy_, z_polar.x()}.local();
  local.z() += z_polar.y();
  return local;
}

polar_vector_3d polar_vector_3d::operator+() const {
  return *this;
}

polar_vector_3d polar_vector_3d::operator-() const {
  return polar_vector_3d{a_xy_, a_z_, -r_};
}

polar_vector_3d& polar_vector_3d::operator*=(float rhs) {
  r_ *= rhs;
  return *this;
}

polar_vector_3d polar_vector_3d::operator*(float rhs) const {
  return polar_vector_3d{*this} *= rhs;
}

polar_vector_3d& polar_vector_3d::operator/=(float rhs) {
  r_ /= rhs;
  return *this;
}

polar_vector_3d polar_vector_3d::operator/(float rhs) const {
  return polar_vector_3d{*this} /= rhs;
}

polar_vector_3d operator*(float lhs, const polar_vector_3d& rhs) {
  return rhs * lhs;
}
