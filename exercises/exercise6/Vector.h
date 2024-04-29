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

#ifndef VECTOR_H_
#define VECTOR_H_

class Vector {
 public:
  // A default constructor for vector that
  // sets all fields equal to 0
  Vector() : x_(0), y_(0), z_(0) {}

  // A constructor that sets all of the Vector's fields
  // to the rexpective inputted parameters (x_ = x, etc.)
  Vector(const float x, const float y, const float z)
               : x_(x), y_(y), z_(z) {}

  // A copy constructor that takes a separate vector
  // and uses its fields to define what this vector's
  // fields should be
  Vector(const Vector& copy) : x_(copy.x_), y_(copy.y_), z_(copy.z_) {}

  // Default Destructor is the synthesized one
  ~Vector() = default;

  // Gets the x_ field of the vector
  float get_x() const;

  // Gets the y_ field of the vector
  float get_y() const;

  // Gets the z_ field of the vector
  float get_z() const;

  // Defines that the assignment operator
  // "=" sets all fields of this vector to
  // those found within vector v
  Vector& operator=(const Vector& v);

  // Defines that the assignment operator
  // "+=" sets all fields of this vector to
  // those found within vector v + those
  // found within this ((this.x_ = v.x_ + this.x_), etc)
  Vector& operator+=(const Vector& v);

  // Defines that the assignment operator
  // "-=" sets all fields of this vector to
  // those found within this minus those found within vector v
  // ((this.x_ = this.x_ - v.x_), etc)
  Vector& operator-=(const Vector& v);

  // Defnes that the assignment operator
  // "*" returns the dot product of
  // the vector "this" and vector "v"
  float operator*(const Vector& v) const;

 private:
  float x_;  // The x_ point of the vector
  float y_;  // The y_ point of the vector
  float z_;  // The z_ point of the vector
};  // class Vector

#endif  // VECTOR_H_
