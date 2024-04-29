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

#ifndef LINKEDINTLIST_H_
#define LINKEDINTLIST_H_

#include <memory>
#include "IntList.h"

using std::shared_ptr;
using std::weak_ptr;

namespace intlist333 {

// LinkedIntList is an implementation of
// the IntList abstract class. Implemented using
// a doubly linked list that only contains integers
// as payloads
class LinkedIntList : public IntList {
 public:
  // Default constructor that initializes number of elements to 0
  // and both the head and tail to null
  LinkedIntList() : IntList(), num_elements_(0), head_(NULL), tail_(NULL) {}

  // Deletes copy constructor
  LinkedIntList(const LinkedIntList& copy) = delete;

  // Deletes assignment operator
  LinkedIntList& operator=(const LinkedIntList& v) = delete;

  // Destructor should be kept default
  // since smart pointers should clean
  // everything up for us
  virtual ~LinkedIntList() = default;

  // Returns the number of elements in the LinkedIntList
  int num_elements() const override;

  // Pushes the given integer on to the front of the List
  void Push(const int payload) override;

  // Pushes the given integer on to the back of the List
  void Append(const int payload) override;

  // Removes the first element in the List and returns it through payload_ptr.
  // Returns true if removal is successful and false if an element cannot be
  // removed.
  bool Pop(int* const payload_ptr) override;

  // Removes the last element in the List
  // that element is returned through payload_ptr
  // returns true if removal is successful
  // false if an element cannot be removed
  bool Slice(int* const payload_ptr) override;

 private:
  // The number of elements in the linked list
  int num_elements_;

  // Node struct for the linked list
  struct Node {
    int payload;
    shared_ptr<Node> next;
    weak_ptr<Node> prev;
  };

  // The head and tail nodes of the linked list
  shared_ptr<Node> head_;
  shared_ptr<Node> tail_;
};

}  // namespace intlist333

#endif  // LINKEDINTLIST_H_
