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

#include <iostream>
#include <stdlib.h>
#include "Vector.h"

static const float ZERO = 0;
static const float ONE_POINT_ONE = 1.156;
static const float TWO = 2.56;
static const float THREE = 3;
static const float FOUR = 4;
static const float FIVE = 5;
static const float SIX_POINT_SEVEN = 6.7;
static const float POINT_ONE = .5;
static const float TWO_POINT_ONE = 2.1541;
static const float THREE_POINT_FOUR = 3.4;

using std::cerr;
using std::endl;
using std::string;
using std::cout;

// Takes in a vector, its expected field values, and an error message to be
// displayed if the vector's fields do not equal the expected fields
// values. Exits the program with failure if the fields don't match
static void VectorEquals(const Vector& v, float x, float y, float z,
                         const string& msg);

// Accepts an instance of a Vector as an
// argument and increments the values stored in the vector by one.
static void Increment(Vector v);

// Takes in a reference to a vector and a generic void pointer.
// The functon checks if the address of the reference is equal
// to the generic pointer or not with true or false being returned
// respectively.
static bool VerifyAddress(const Vector& v, void* ptr);

// Standard main function
int main(int argc, char **argv) {
  if (argc != 1) {
    cerr << "There should be no arguments ran with this executable" << endl;
    return EXIT_FAILURE;
  }

  // Note, all tests implicitly test the getter
  Vector origin;

  // Tests default constuctor
  VectorEquals(origin, ZERO, ZERO, ZERO,
    "Calling vector's defacult constructor did not set all fields to 0");

  // Tests float constructor
  Vector initvect(ONE_POINT_ONE, TWO, THREE);
  VectorEquals(initvect, ONE_POINT_ONE, TWO, THREE,
    "Calling the vector's 3 parameter constructor does not set it");

  // Tests copy constructor
  Vector cpyvect(initvect);
  VectorEquals(cpyvect, ONE_POINT_ONE, TWO, THREE,
    "Calling the vector's copy constructor does not correctly copy values");

  // Tests += operator
  Vector assignmentvec(FOUR, FIVE, SIX_POINT_SEVEN);
  cpyvect+=assignmentvec;
  VectorEquals(cpyvect, FOUR+ONE_POINT_ONE, FIVE + TWO, SIX_POINT_SEVEN + THREE,
    "Vector's '+=' operator does not correctly add two vectors");

  assignmentvec+=assignmentvec;
  VectorEquals(assignmentvec, FOUR, FIVE, SIX_POINT_SEVEN,
    "Vector's '+=' operator does not correctly add vector to itself");

  // Tests = operator
  cpyvect = origin;
  VectorEquals(cpyvect, ZERO, ZERO, ZERO,
    "Vector's '=' operator did not correctly set a vector's values");

  cpyvect = cpyvect;
  VectorEquals(cpyvect, ZERO, ZERO, ZERO,
    "Vector's '=' operator did not do nothing when something equaled itself");

  // Tests -= operator
  cpyvect -= assignmentvec;
  VectorEquals(cpyvect, -FOUR, -FIVE, -SIX_POINT_SEVEN,
    "Vector's '-=' operator did not correctly subtract a vector's values");

  cpyvect -= cpyvect;
  VectorEquals(cpyvect, -FOUR, -FIVE, -SIX_POINT_SEVEN,
    "Vector's '-=' operator did not do nothing when subtracted by itself");

  // Tests * operator
  float dot = ((FOUR * -FOUR) + (FIVE * -FIVE) + (SIX_POINT_SEVEN * -SIX_POINT_SEVEN));
  if ((cpyvect * assignmentvec) != dot) {
    cerr << "Vector's '*' operator did not compute the dot product" << endl;
    return EXIT_FAILURE;
  }

  Vector complicatedvec(POINT_ONE, TWO_POINT_ONE, THREE_POINT_FOUR);
  VectorEquals(complicatedvec, POINT_ONE, TWO_POINT_ONE, THREE_POINT_FOUR,
  "Vector constructor with values does not correctly store decimal values");

  float dottwo = ((POINT_ONE * -FOUR) + (TWO_POINT_ONE * -FIVE)
                 + (THREE_POINT_FOUR * -SIX_POINT_SEVEN));
  if ((complicatedvec * cpyvect) != dottwo) {
    cerr << "Vector's '*' operator did not compute the " <<
         "dot product for decimal values" << endl;
    return EXIT_FAILURE;
  }

  // Tests pass-by-reference
  Vector testvec;
  Increment(testvec);
  if (testvec.get_x() == 1 && testvec.get_y() == 1 && testvec.get_z() == 1) {
    cout << "Vector: pass-by-reference" << endl;
  } else {
    cout << "Vector: pass-by-value" << endl;
  }

  // Tests reference has same address
  if (VerifyAddress(testvec, &testvec)) {
    cout << "Ref: same address" << endl;
  } else {
    cout << "Ref: dfferent address" << endl;
  }

  return EXIT_SUCCESS;
}

static void VectorEquals(const Vector& v, float x, float y,
                         float z, const string& msg) {
  if (v.get_x() != x || v.get_y() != y || v.get_z() != z) {
    cerr << msg << endl;
    exit(EXIT_FAILURE);
  }
}

static void Increment(Vector v) {
  Vector adder(1, 1, 1);
  v += adder;
}

static bool VerifyAddress(const Vector& v, void* ptr) {
  return (&v == reinterpret_cast<Vector*>(ptr));
}
