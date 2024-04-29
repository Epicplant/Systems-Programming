/*
 * Copyright ©2023 Chris Thachuk.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Autumn Quarter 2023 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include <iostream>   // for cout, cerr, endl
#include <cstdlib>    // for EXIT_SUCCESS, EXIT_FAILURE

#include "IntList.h"
#include "LinkedIntList.h"

static const int kOne = 1;
static const int kTwo = 2;
static const int kThree = 3;
static const int kFour = 4;
static const int kFive = 5;

using intlist333::IntList;
using intlist333::LinkedIntList;
using std::cerr;
using std::cout;
using std::endl;

template <typename T>
static void CheckEquals(T expected, T actual);

int main() {
  IntList* list = new LinkedIntList;
  int output;

  // verify the empty list case
  CheckEquals(0, list->num_elements());
  CheckEquals(false, list->Pop(&output));
  CheckEquals(false, list->Slice(&output));

  // insert an element
  list->Push(kOne);
  CheckEquals(1, list->num_elements());

  // Pop the element from the front
  CheckEquals(true, list->Pop(&output));
  CheckEquals(kOne, output);

  // re Push and Pop it from the back
  list->Push(kTwo);
  CheckEquals(1, list->num_elements());
  CheckEquals(true, list->Slice(&output));
  CheckEquals(kTwo, output);
  CheckEquals(0, list->num_elements());

  // Try to Pop an empty list, and fail
  CheckEquals(false, list->Pop(&output));
  CheckEquals(false, list->Slice(&output));

  // Insert five elements
  list->Push(kThree);
  list->Push(kTwo);
  list->Push(kOne);
  list->Append(kFour);
  list->Append(kFive);
  CheckEquals(5, list->num_elements());

  // remove the back
  CheckEquals(true, list->Slice(&output));
  CheckEquals(kFive, output);
  CheckEquals(4, list->num_elements());

  // remove the front
  CheckEquals(true, list->Pop(&output));
  CheckEquals(kOne, output);
  CheckEquals(3, list->num_elements());

  CheckEquals(true, list->Pop(&output));
  CheckEquals(kTwo, output);
  CheckEquals(2, list->num_elements());

  CheckEquals(true, list->Pop(&output));
  CheckEquals(kThree, output);
  CheckEquals(1, list->num_elements());

  CheckEquals(true, list->Pop(&output));
  CheckEquals(kFour, output);
  CheckEquals(0, list->num_elements());

  list->Push(kThree);
  list->Append(kTwo);
  list->Push(kOne);
  list->Append(kFour);
  list->Push(kFive);

  CheckEquals(true, list->Slice(&output));
  CheckEquals(kFour, output);
  CheckEquals(4, list->num_elements());

  CheckEquals(true, list->Slice(&output));
  CheckEquals(kTwo, output);
  CheckEquals(3, list->num_elements());

  CheckEquals(true, list->Slice(&output));
  CheckEquals(kThree, output);
  CheckEquals(2, list->num_elements());

  CheckEquals(true, list->Slice(&output));
  CheckEquals(kOne, output);
  CheckEquals(1, list->num_elements());

  CheckEquals(true, list->Slice(&output));
  CheckEquals(kFive, output);
  CheckEquals(0, list->num_elements());


  // delete the list
  delete list;

  cout << "All tests passed! be sure to run under valgrind!" << endl;

  return EXIT_SUCCESS;
}

template <typename T>
void CheckEquals(T expected, T actual) {
  if (expected != actual) {
    cerr << "ERROR" << endl;
    cerr << "expected value: " << expected << endl;
    cerr << "actual value: " << actual << endl;
    exit(EXIT_FAILURE);
  }
}
