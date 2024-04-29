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

#include <iostream>
using std::ostream;

namespace vector333 {

class Vector {
 public:
  // A default constructor for vector that
  // sets every degree equal to 0
  Vector() : degrees_(new float[3]()) {}

  // A constructor that sets every degree
  // to the rexpective inputted parameters (x = degrees_[0], etc.)
  Vector(const float x, const float y, const float z) : degrees_
        (new float[3]{x, y, z}) {}

  // A copy constructor that takes a separate vector
  // and takes all of this vector's degree values
  // and changes them to the copy vector's degree
  // values
  Vector(const Vector& copy) : degrees_
        (new float[3] { copy.degrees_[0], copy.degrees_[1], copy.degrees_[2]})
        {}

  // Intentionally deletes the allocated space for the
  // degree_ field.
  ~Vector();

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

  // Non-member function that defines that the assignment operator
  // "*" returns the dot product of
  // the "left vector" and the "right vector"
  friend float operator*(const Vector& leftvect, const Vector& rightvect);

  // Non-member function that defines that the assignment operator
  // "<<" will write Vector v to stream out as (a, b, c) (AKA the
  // degrees of the vector)
  friend ostream& operator<<(ostream& out, const Vector& v);

 private:
  float* degrees_;  // Every degree of the vector
};  // class Vector

  // Non-member function that defines that the assignment operator
  // "*" returns the dot product
  // of the "left vector" and the vector (k, k, k)
  float operator*(const Vector& leftvect, const float k);

  // Non-member function that defines that the assignment operator
  // "*" returns the dot product
  // of the "right vector" and the vector (k, k, k)
  float operator*(const float k, const Vector& rightvect);

  // Non-member function that defines that the assignment operator
  // "+" returns an addition between the degrees of
  // the "left vector" and the "right vector" and returns a new vector
  Vector operator+(const Vector& leftvect, const Vector& rightvect);

  // Non-member function that defines that the assignment operator
  // "-" returns a subtraction between the degrees of
  // the "left vector" and the "right vector" and returns a new vector
  Vector operator-(const Vector& leftvect, const Vector& rightvect);

}  // namespace vector333

#endif  // VECTOR_H_
