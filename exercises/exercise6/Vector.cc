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

// Getters
float Vector::get_x() const { return x_; }  // inline member function
float Vector::get_y() const { return y_; }  // inline member function
float Vector::get_z() const { return z_; }  // inline member function

// Assignments
Vector& Vector::operator=(const Vector& v) {
  if (this != &v) {
    x_ = v.x_;
    y_ = v.y_;
    z_ = v.z_;
  }

  return *this;
}

Vector& Vector::operator+=(const Vector& v) {
  if (this != &v) {
    x_ = x_ + v.x_;
    y_ = y_ + v.y_;
    z_ = z_ + v.z_;
  }
  return *this;
}

Vector& Vector::operator-=(const Vector& v) {
  if (this != &v) {
    x_ = x_ - v.x_;
    y_ = y_ - v.y_;
    z_ = z_ - v.z_;
  }

  return *this;
}

float Vector::operator*(const Vector& v) const {
  return ((x_ * v.x_) + (y_ * v.y_) + (z_ * v.z_));
}
