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

#include "LinkedIntList.h"

namespace intlist333 {

int LinkedIntList::num_elements() const {
  return num_elements_;
}

void LinkedIntList::Push(const int payload) {
  // Empty list edge case
  if (num_elements_ == 0) {
    // Set the head_ and tail_ equal to the new node
    shared_ptr<Node> nd(new Node());
    nd->payload = payload;
    nd->next.reset();
    nd->prev.reset();

    head_ = nd;
    tail_ = nd;
  } else {  // There are already elements
    shared_ptr<Node> nd(new Node());
    nd->payload = payload;
    nd->prev.reset();

    // Unhook old head and make it the new node's next
    head_->prev = nd;
    nd->next = head_;
    head_ = nd;
  }

  num_elements_++;
}

void LinkedIntList::Append(const int payload) {
  // Empty list edge case
  if (num_elements_ == 0) {
    // Set the head_ and tail_ equal to the new node
    shared_ptr<Node> nd(new Node());
    nd->payload = payload;
    nd->next.reset();
    nd->prev.reset();

    head_ = nd;
    tail_ = nd;
  } else {  // There are already elements
    shared_ptr<Node> nd(new Node());
    nd->payload = payload;
    nd->next.reset();

    // Unhook old tail and make it the new node's prev
    nd->prev = tail_;
    tail_->next = nd;
    tail_ = nd;
  }

  num_elements_++;
}

bool LinkedIntList::Pop(int* const payload_ptr) {
  // Empty list edge case
  if (num_elements_ == 0) {
    return false;
  } else if (num_elements_ == 1) {  // One element edge case
    // Remove single element from the list and make it empty
    *payload_ptr = head_->payload;
    head_.reset();
    tail_.reset();
  } else {  // More than one element
    //  Remove head and make its next the new head
    *payload_ptr = head_->payload;
    head_ = head_->next;
    head_->prev.reset();
  }

  num_elements_--;
  return true;
}

bool LinkedIntList::Slice(int* const payload_ptr) {
  // Empty list edge case
  if (num_elements_ == 0) {
    return false;
  } else if (num_elements_ == 1) {  // One element edge case
    // Remove single element from the list and make it empty
    *payload_ptr = tail_->payload;
    head_.reset();
    tail_.reset();
  } else {  // More than one element
    //  Remove tail and make its prev the new tail
    *payload_ptr = tail_->payload;
    tail_ = tail_->prev.lock();
    tail_->next.reset();
  }

  num_elements_--;
  return true;
}

}   // namespace intlist333
