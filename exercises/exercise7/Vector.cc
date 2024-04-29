/*
 * Copyright @2023 Christopher Roy. All rights reserved. Permission is
 * hereby granted to students registered for Unversity of Washgnton
 * CSE 333 for use solely during Fall Quarter 2023 for purposes of
 * the course. No other use, copying, distributon, or modification
 * is permitted wthout prior written consent. Copyrights for
 * third-party components of this work must be honored. Instructors
 * interested in reusing these course materials should contact the
 * author.
 *
 * Name: Christopher Roy
 * Email: cr0y@uw.edu
 */

#include "Vector.h"
#include <stdlib.h>

namespace vector333 {

// Destructor
Vector::~Vector() {
  delete [] degrees_;
}

// Assignments
Vector& Vector::operator=(const Vector& v) {
  if (this != &v) {
    degrees_[0] = v.degrees_[0];
    degrees_[1] = v.degrees_[1];
    degrees_[2] = v.degrees_[2];
  }

  return *this;
}

Vector& Vector::operator+=(const Vector& v) {
  if (this != &v) {
    degrees_[0] = degrees_[0] + v.degrees_[0];
    degrees_[1] = degrees_[1] + v.degrees_[1];
    degrees_[2] = degrees_[2] + v.degrees_[2];
  }

  return *this;
}

Vector& Vector::operator-=(const Vector& v) {
  if (this != &v) {
    degrees_[0] = degrees_[0] - v.degrees_[0];
    degrees_[1] = degrees_[1] - v.degrees_[1];
    degrees_[2] = degrees_[2] - v.degrees_[2];
  }

  return *this;
}

float operator*(const Vector& leftvect, const Vector& rightvect) {
  return ((leftvect.degrees_[0] * rightvect.degrees_[0])
        + (leftvect.degrees_[1] * rightvect.degrees_[1])
        + (leftvect.degrees_[2] * rightvect.degrees_[2]));
}

ostream& operator<<(ostream& out, const Vector& v) {
  out << "(" << v.degrees_[0] << "," << v.degrees_[1] << "," <<
  v.degrees_[2] << ")";
  return out;
}

float operator*(const Vector& leftvect, const float k) {
  return (leftvect * Vector(k, k, k));
}

float operator*(const float k, const Vector& rightvect) {
  return (rightvect * k);
}

Vector operator+(const Vector& leftvect, const Vector& rightvect) {
  Vector tmp = leftvect;
  tmp += rightvect;
  return tmp;
}

Vector operator-(const Vector& leftvect, const Vector& rightvect) {
  Vector tmp = leftvect;
  tmp -= rightvect;
  return tmp;
}

}  // namespace vector333
