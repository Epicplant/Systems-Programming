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

#ifndef INTLIST_H_
#define INTLIST_H_

namespace intlist333 {

// An abstract class that describes a datastructure for containing integers.
// Integers can be added or removed from the front or back of the list.
class IntList {
 public:
  // Default constructor is fine since this is an abstract class
  IntList() = default;

  // Make the destructor virtual; this is good hygiene, since it guarantees
  // that the full class hierarchy below has virtual destructors.
  virtual ~IntList() { }

  // "Pure virtual functions" that need to be overwritten by any non-abstract
  // derived classes.

  // Returns the number of elements in the IntList
  virtual int num_elements() const = 0;

  // Pushes the given integer on to the front of the List
  virtual void Push(const int payload) = 0;

  // Pushes the given integer on to the back of the List
  virtual void Append(const int payload) = 0;

  // Removes the first element in the List and returns it through payload_ptr.
  // Returns true if removal is successful and false if an element cannot be
  // removed.
  virtual bool Pop(int* const payload_ptr) = 0;

  // removes the last element in the List
  // that element is returned through payload_ptr
  // returns true if removal is successful
  // false if an element cannot be removed
  virtual bool Slice(int* const payload_ptr) = 0;
};

}  // namespace intlist333

#endif  // INTLIST_H_
