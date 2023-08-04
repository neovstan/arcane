#ifndef PSDK_UTILS_POLAR_VECTOR_H
#define PSDK_UTILS_POLAR_VECTOR_H

namespace psdk_utils {
class local_vector;

class polar_vector {
 public:
  polar_vector(float a = 0.0f, float r = 0.0f);

  float x() const;
  float y() const;

  local_vector local() const;
  polar_vector canonical() const;

 public:
  polar_vector operator+() const;
  polar_vector operator-() const;

  polar_vector& operator*=(float rhs);
  polar_vector operator*(float rhs) const;

  polar_vector& operator/=(float rhs);
  polar_vector operator/(float rhs) const;

 private:
  float a_;
  float r_;
};

class polar_vector_3d {
 public:
  polar_vector_3d(float a_xy = 0.0f, float a_z = 0.0f, float r = 0.0f);

  float x() const;
  float y() const;
  float z() const;

  local_vector local() const;

 public:
  polar_vector_3d operator+() const;
  polar_vector_3d operator-() const;

  polar_vector_3d& operator*=(float rhs);
  polar_vector_3d operator*(float rhs) const;

  polar_vector_3d& operator/=(float rhs);
  polar_vector_3d operator/(float rhs) const;

 private:
  float a_xy_;
  float a_z_;
  float r_;
};
}  // namespace psdk_utils

psdk_utils::polar_vector operator*(float lhs, const psdk_utils::polar_vector& rhs);

#endif  // PSDK_UTILS_POLAR_VECTOR_H
