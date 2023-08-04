#ifndef PSDK_UTILS_LOCAL_VECTOR_H
#define PSDK_UTILS_LOCAL_VECTOR_H

#include <iostream>
#include <rw/rwplcore.h>

class CVector;

namespace psdk_utils {
class polar_vector;
class polar_vector_3d;

class local_vector {
 public:
  local_vector(float x = 0.0f, float y = 0.0f, float z = 0.0f);
  local_vector(const CVector& vec);
  local_vector(const RwV3d& vec);

  float a_xy() const;
  float a_z() const;
  float r() const;

  float x() const;
  float y() const;
  float z() const;

  float& x();
  float& y();
  float& z();

  polar_vector polar() const;

  polar_vector direct(float r = 1.0f) const;
  polar_vector normal(float r = 1.0f) const;

  polar_vector transition(const local_vector& vec) const;
  local_vector transit(const polar_vector& transition) const;
  local_vector transit(const polar_vector_3d& transition) const;

  local_vector project(const local_vector& begin, const local_vector& end) const;

  float dot(const local_vector& rhs) const;
  local_vector cross(const local_vector& rhs) const;
  float angle(const local_vector& rhs) const;

  local_vector to_screen() const;

 public:
  operator CVector() const;
  operator RwV3d() const;

  bool operator==(const local_vector& other) const;

  local_vector operator+() const;
  local_vector operator-() const;

  local_vector& operator+=(const local_vector& rhs);
  local_vector operator+(const local_vector& rhs) const;

  local_vector& operator-=(const local_vector& rhs);
  local_vector operator-(const local_vector& rhs) const;

  local_vector& operator*=(float rhs);
  local_vector operator*(float rhs) const;

  local_vector& operator/=(float rhs);
  local_vector operator/(float rhs) const;

 private:
  float x_;
  float y_;
  float z_;
};
}  // namespace psdk_utils

psdk_utils::local_vector operator*(float lhs, const psdk_utils::local_vector& rhs);
std::ostream& operator<<(std::ostream& os, const psdk_utils::local_vector& vec);

#endif  // PSDK_UTILS_LOCAL_VECTOR_H
