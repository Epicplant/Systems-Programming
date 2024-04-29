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
static const float ONE_POINT_ONE = 1.1;
static const float TWO = 2;
static const float THREE = 3;
static const float FOUR = 4;
static const float FIVE = 5;
static const float SIX_POINT_SEVEN = 6.7;
static const float POINT_FIVE = .5;
static const float ONE_PONT_FOUR = 1.4;
static const float POINT_SEVEN_FIVE = .75;
static const float POINT_TWO_FIVE = .25;
static const float TWO_POINT_FIVE_SIX = 2.56;

using std::cerr;
using std::endl;
using std::string;
using std::cout;
using vector333::Vector;

// Standard main function
int main(int argc, char **argv) {
  // Return as a failure if an unneeded command lne argument is found
  if (argc != 1) {
    cerr << "There should be no arguments ran with this executable" << endl;
    return EXIT_FAILURE;
  }

  // Note: The '<<' is implicitly tested through its usage in testing other
  // parts of the vectors' functionality

  // Tests Default Constructor
  Vector origin;
  cout << "Vector should be (0,0,0) and is: " << origin << endl;

  // Tests Variable Constructor
  Vector initvect(ONE_POINT_ONE, TWO_POINT_FIVE_SIX, THREE);
  cout << "Vector should be (1.1,2.56,3) and is: " << initvect << endl;

  // Tests Copy Constructor
  Vector cpyvect(initvect);
  cout << "Vector should be (1.1,2.56,3) and is: " << cpyvect << endl;

  // Tests '+=' operator
  Vector assignmentvec(FOUR, FIVE, SIX_POINT_SEVEN);
  cpyvect+=assignmentvec;
  cout << "Vector should be (5.1,7.56,9.7) and is: " << cpyvect << endl;

  assignmentvec+=assignmentvec;
  cout << "Vector should be (4,5,6.7) and is: " << assignmentvec << endl;

  // Tests '=' operator
  cpyvect = origin;
  cout << "Vector should be (0,0,0) and is: " << cpyvect << endl;

  cpyvect = cpyvect;
  cout << "Vector should be (0,0,0) and is: " << cpyvect << endl;

  // Tests '-=' operator
  cpyvect -= assignmentvec;
  cout << "Vector should be (-4,-5,-6.7) and is: " << cpyvect << endl;

  cpyvect -= cpyvect;
  cout << "Vector should be (-4,-5,-6.7) and is: " << cpyvect << endl;

  // Tests '*' operator
  float dot = ((FOUR * -FOUR) + (FIVE * -FIVE) +
              (SIX_POINT_SEVEN * -SIX_POINT_SEVEN));
  if ((cpyvect * assignmentvec) != dot) {
    cerr << "Vector's '*' operator did not compute the dot product" << endl;
    return EXIT_FAILURE;
  }

  dot = ((FOUR * -FOUR) + (FIVE * -FOUR) + (SIX_POINT_SEVEN * -FOUR));
  if ((assignmentvec * -FOUR) != dot) {
    cerr << "Vector's '*' operator did not compute the dot product for"
    << " vector and float" << endl;
    return EXIT_FAILURE;
  }

  if ((-FOUR * assignmentvec) != dot) {
    cerr << "Vector's '*' operator did not compute the dot product for"
    << " vector and float" << endl;
    return EXIT_FAILURE;
  }

  // Tests '+' operator
  cout << "Vector should be (" << POINT_FIVE + ZERO << ","
  << ONE_PONT_FOUR + ONE_PONT_FOUR << "," << -POINT_SEVEN_FIVE + -POINT_TWO_FIVE
  << ") and is: " << (Vector(POINT_FIVE, ONE_PONT_FOUR, -POINT_SEVEN_FIVE)
  + Vector(ZERO, ONE_PONT_FOUR, -POINT_TWO_FIVE)) << endl;

  // Tests '-' operator
  cout << "Vector should be (" << TWO - POINT_FIVE << ","
  << -POINT_SEVEN_FIVE - -POINT_SEVEN_FIVE << "," <<
   -THREE - FOUR << ") and is: " << (Vector(TWO, -POINT_SEVEN_FIVE, -THREE)
  - Vector(POINT_FIVE, -POINT_SEVEN_FIVE, FOUR)) << endl;

  return EXIT_SUCCESS;
}
