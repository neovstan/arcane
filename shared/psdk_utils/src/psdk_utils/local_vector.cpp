#include "local_vector.h"

#include <CVector.h>

#include "cmath.h"
#include "psdk_utils.h"
#include "polar_vector.h"

using namespace psdk_utils;

local_vector::local_vector(float x, float y, float z) : x_{x}, y_{y}, z_{z} {
}

local_vector::local_vector(const CVector& vec) {
  x_ = vec.x;
  y_ = vec.y;
  z_ = vec.z;
}

local_vector::local_vector(const RwV3d& vec) {
  x_ = vec.x;
  y_ = vec.y;
  z_ = vec.z;
}

float local_vector::a_xy() const {
  return math::angle(x_, y_);
}

float local_vector::a_z() const {
  return math::angle(z_, math::hypot(x_, y_)) - 90.0f;
}

float local_vector::r() const {
  return math::hypot(x_, y_, z_);
}

float local_vector::x() const {
  return x_;
}

float local_vector::y() const {
  return y_;
}

float local_vector::z() const {
  return z_;
}

float& local_vector::x() {
  return x_;
}

float& local_vector::y() {
  return y_;
}

float& local_vector::z() {
  return z_;
}

polar_vector local_vector::polar() const {
  return polar_vector{a_xy(), r()};
}

polar_vector local_vector::direct(float r) const {
  return polar_vector{a_xy(), r};
}

polar_vector local_vector::normal(float r) const {
  return polar_vector{a_xy() + 90.0f, r};
}

polar_vector local_vector::transition(const local_vector& vec) const {
  const auto delta = vec - *this;
  return polar_vector{delta.a_xy(), delta.r()};
}

local_vector local_vector::transit(const polar_vector& transition) const {
  const auto delta = local_vector{transition.x(), transition.y()};
  return *this + delta;
}

local_vector local_vector::transit(const polar_vector_3d& transition) const {
  const auto delta = local_vector{transition.x(), transition.y(), transition.z()};
  return *this + delta;
}

local_vector local_vector::project(const local_vector& begin, const local_vector& end) const {
  const auto u = end - begin;
  const auto v = *this - begin;
  const auto lambda = math::clamp(0.0f, v.dot(u) / u.dot(u), 1.0f);
  return begin + lambda * u;
}

float local_vector::dot(const local_vector& rhs) const {
  return x_ * rhs.x_ + y_ * rhs.y_ + z_ * rhs.z_;
}

local_vector local_vector::cross(const local_vector& rhs) const {
  return local_vector{y_ * rhs.z_ - z_ * rhs.y_, z_ * rhs.x_ - x_ * rhs.z_,
                      x_ * rhs.y_ - y_ * rhs.x_};
}

float local_vector::angle(const local_vector& rhs) const {
  return math::acos(dot(rhs) / (r() * rhs.r()));
}

local_vector local_vector::to_screen() const {
  return calc_screen_coors(*this);
}

local_vector::operator CVector() const {
  return CVector{x_, y_, z_};
}

local_vector::operator RwV3d() const {
  return RwV3d{x_, y_, z_};
}

bool local_vector::operator==(const local_vector& other) const {
  return x_ == other.x_ && y_ == other.y_ && z_ == other.z_;
}

local_vector local_vector::operator+() const {
  return *this;
}

local_vector local_vector::operator-() const {
  return local_vector{-x_, -y_, -z_};
}

local_vector& local_vector::operator+=(const local_vector& rhs) {
  x_ += rhs.x_;
  y_ += rhs.y_;
  z_ += rhs.z_;
  return *this;
}

local_vector local_vector::operator+(const local_vector& rhs) const {
  return local_vector{*this} += rhs;
}

local_vector& local_vector::operator-=(const local_vector& rhs) {
  x_ -= rhs.x_;
  y_ -= rhs.y_;
  z_ -= rhs.z_;
  return *this;
}

local_vector local_vector::operator-(const local_vector& rhs) const {
  return local_vector{*this} -= rhs;
}

local_vector& local_vector::operator*=(float rhs) {
  x_ *= rhs;
  y_ *= rhs;
  z_ *= rhs;
  return *this;
}

local_vector local_vector::operator*(float rhs) const {
  return local_vector{*this} *= rhs;
}

local_vector& local_vector::operator/=(float rhs) {
  x_ /= rhs;
  y_ /= rhs;
  z_ /= rhs;
  return *this;
}

local_vector local_vector::operator/(float rhs) const {
  return local_vector{*this} /= rhs;
}

local_vector operator*(float lhs, const local_vector& rhs) {
  return rhs * lhs;
}

std::ostream& operator<<(std::ostream& os, const psdk_utils::local_vector& vec) {
  os << "local_vector(" << vec.x() << ", " << vec.y() << ", " << vec.z() << ")";
  return os;
}
