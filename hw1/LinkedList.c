/*
 * Copyright ©2023 Chris Thachuk.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Fall Quarter 2023 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include <stdio.h>
#include <stdlib.h>

#include "CSE333.h"
#include "LinkedList.h"
#include "LinkedList_priv.h"


///////////////////////////////////////////////////////////////////////////////
// LinkedList implementation.

LinkedList* LinkedList_Allocate(void) {
  // Allocate the linked list record.
  LinkedList *ll = (LinkedList *) malloc(sizeof(LinkedList));
  Verify333(ll != NULL);

  // STEP 1: initialize the newly allocated record structure.
  // Initialize head, tail, and num of elements
  // Caller takes responsibility for freeing the data
  ll->num_elements = 0;
  ll->head = NULL;
  ll->tail = NULL;

  // Return our newly minted linked list.
  return ll;
}

void LinkedList_Free(LinkedList *list,
                     LLPayloadFreeFnPtr payload_free_function) {
  Verify333(list != NULL);
  Verify333(payload_free_function != NULL);

  // STEP 2: sweep through the list and free all of the nodes' payloads
  // (using the payload_free_function supplied as an argument) and
  // the nodes themselves.

  // 1. Get head ListNode from the passed in Linked List if it exists
  LinkedListNode* curr = list->head;

  // 2. Use while loop to progress from head to tail
  while (curr != NULL) {
    // 3. At each iteraton of the while loop, use the payload_free_functon
    // on its payload to free it and then free the node itself
    // using malloc on the pointer to it
    payload_free_function(curr->payload);
    LinkedListNode* prior_node = curr;
    curr = curr->next;
    free(prior_node);
  }

  // free the LinkedList
  free(list);
}

int LinkedList_NumElements(LinkedList *list) {
  Verify333(list != NULL);
  return list->num_elements;
}

void LinkedList_Push(LinkedList *list, LLPayload_t payload) {
  Verify333(list != NULL);

  // Allocate space for the new node.
  LinkedListNode *ln = (LinkedListNode *) malloc(sizeof(LinkedListNode));
  Verify333(ln != NULL);

  // Set the payload
  ln->payload = payload;

  if (list->num_elements == 0) {
    // Degenerate case; list is currently empty
    Verify333(list->head == NULL);
    Verify333(list->tail == NULL);
    ln->next = ln->prev = NULL;
    list->head = list->tail = ln;
    list->num_elements = 1;
  } else {
    // STEP 3: typical case; list has >=1 elements
    // ln is the node we are modifying, it already has the payload added to it,
    // so now there just needs to be surgery done to to append it make it the
    // new head and update the head value wthin list to make the head there ln

    // Simply connect the old head to the new head
    ln->next = list->head;
    ln->prev = NULL;
    list->num_elements++;
    list->head = ln;
    ln->next->prev = ln;
  }
}

bool LinkedList_Pop(LinkedList *list, LLPayload_t *payload_ptr) {
  Verify333(payload_ptr != NULL);
  Verify333(list != NULL);

  // STEP 4: implement LinkedList_Pop.  Make sure you test for
  // an empty list and fail.  If the list is non-empty, there
  // are two cases to consider: (a) a list with a single element in it
  // and (b) the general case of a list with >=2 elements in it.
  // Be sure to call free() to deallocate the memory that was
  // previously allocated by LinkedList_Push().

  // Can't remove data due to no elements, so return false.
  if (list->num_elements <= 0) {
    return false;
  }

  list->num_elements--;

  // Only one element in linked list
  if (list->num_elements == 0) {
    // Set the head AND tail pointer in list to null
    // Free the ListNode
    *payload_ptr = list->head->payload;
    free(list->head);
    list->tail = NULL;
    list->head = NULL;
  } else {
    // Set head->next->prev = NULL;
    list->head->next->prev = NULL;

    // Set the head pointer equal to head->next;
    LinkedListNode* new_head = list->head->next;
    // free the ListNode
    *payload_ptr = list->head->payload;
    free(list->head);
    list->head = new_head;
  }

  return true;  // you may need to change this return value
}

void LinkedList_Append(LinkedList *list, LLPayload_t payload) {
  Verify333(list != NULL);

  // STEP 5: implement LinkedList_Append.  It's kind of like
  // LinkedList_Push, but obviously you need to add to the end
  // instead of the beginning.

  // Allocate space for the new node.
  LinkedListNode* ln = (LinkedListNode *) malloc(sizeof(LinkedListNode));
  Verify333(ln != NULL);

  // Set the payload
  ln->payload = payload;

  if (list->num_elements == 0) {
    // Degenerate case; list is currently empty
    ln->next = ln->prev = NULL;
    list->head = list->tail = ln;
    list->num_elements = 1;
  } else {
    // ln is the node we are modifying, it already has the payload added to it,
    // so now there just needs to be surgery done to to append it make it the
    // new head and update the tail value wthin list to make the tail there ln
    ln->prev = list->tail;
    ln->next = NULL;
    list->num_elements++;
    list->tail = ln;
    ln->prev->next = ln;
  }
}

void LinkedList_Sort(LinkedList *list, bool ascending,
                     LLPayloadComparatorFnPtr comparator_function) {
  Verify333(list != NULL);
  if (list->num_elements < 2) {
    // No sorting needed.
    return;
  }

  // We'll implement bubblesort! Nnice and easy, and nice and slow :)
  int swapped;
  do {
    LinkedListNode *curnode;

    swapped = 0;
    curnode = list->head;
    while (curnode->next != NULL) {
      int compare_result = comparator_function(curnode->payload,
                                               curnode->next->payload);
      if (ascending) {
        compare_result *= -1;
      }
      if (compare_result < 0) {
        // Bubble-swap the payloads.
        LLPayload_t tmp;
        tmp = curnode->payload;
        curnode->payload = curnode->next->payload;
        curnode->next->payload = tmp;
        swapped = 1;
      }
      curnode = curnode->next;
    }
  } while (swapped);
}


///////////////////////////////////////////////////////////////////////////////
// LLIterator implementation.

LLIterator* LLIterator_Allocate(LinkedList *list) {
  Verify333(list != NULL);

  // OK, let's manufacture an iterator.
  LLIterator *li = (LLIterator *) malloc(sizeof(LLIterator));
  Verify333(li != NULL);

  // Set up the iterator.
  li->list = list;
  li->node = list->head;

  return li;
}

void LLIterator_Free(LLIterator *iter) {
  Verify333(iter != NULL);
  free(iter);
}

bool LLIterator_IsValid(LLIterator *iter) {
  Verify333(iter != NULL);
  Verify333(iter->list != NULL);

  return (iter->node != NULL);
}

bool LLIterator_Next(LLIterator *iter) {
  Verify333(iter != NULL);
  Verify333(iter->list != NULL);
  Verify333(iter->node != NULL);

  // STEP 6: try to advance iterator to the next node and return true if
  // you succeed, false otherwise
  // Note that if the iterator is already at the last node,
  // you should move the iterator past the end of the list

  iter->node = iter->node->next;
  return LLIterator_IsValid(iter);  // you may need to change this return value
}

void LLIterator_Get(LLIterator *iter, LLPayload_t *payload) {
  Verify333(iter != NULL);
  Verify333(iter->list != NULL);
  Verify333(iter->node != NULL);

  *payload = iter->node->payload;
}

bool LLIterator_Remove(LLIterator *iter,
                       LLPayloadFreeFnPtr payload_free_function) {
  Verify333(iter != NULL);
  Verify333(iter->list != NULL);
  Verify333(iter->node != NULL);

  // STEP 7: implement LLIterator_Remove.  This is the most
  // complex function you'll build.  There are several cases
  // to consider:
  // - degenerate case: the list becomes empty after deleting.
  // - degenerate case: iter points at head
  // - degenerate case: iter points at tail
  // - fully general case: iter points in the middle of a list,
  //                       and you have to "splice".
  //
  // Be sure to call the payload_free_function to free the payload
  // the iterator is pointing to, and also free any LinkedList
  // data structure element as appropriate.

  iter->list->num_elements--;
  LinkedListNode* nd = iter->node;

  // The list becomes empty after deleting node
  if (iter->list->num_elements == 0) {
    // Set all iter values to null and free the node that was there
    iter->list->tail = NULL;
    iter->list->head = NULL;
    iter->node = NULL;

    payload_free_function(nd->payload);
    free(nd);

    return false;
  } else if (iter->node == iter->list->head) {  // iter points to head
    // Simply disconnect from front of chain and make the old
    // heads next the new head
    nd->next->prev = NULL;
    iter->node = nd->next;
    iter->list->head = nd->next;
  } else if (iter->node == iter->list->tail) {  // iter points to tail
    // Simply disconnect from end of chain and make the old
    // tails prev the new tail
    nd->prev->next = NULL;
    iter->node = nd->prev;
    iter->list->tail = nd->prev;
  } else {  // iter points to middle of list, splicing occurs
    // Splice the node out of the liked list
    nd->next->prev = nd->prev;
    nd->prev->next = nd->next;
    iter->node = nd->next;
  }

  // Free all malloced data from the disconnected node.
  payload_free_function(nd->payload);
  free(nd);
  return true;  // you may need to change this return value
}


///////////////////////////////////////////////////////////////////////////////
// Helper functions

bool LLSlice(LinkedList *list, LLPayload_t *payload_ptr) {
  Verify333(payload_ptr != NULL);
  Verify333(list != NULL);

  // STEP 8: implement LLSlice.

  // Can't remove data due to no elements, so return false.
  if (list->num_elements <= 0) {
    return false;
  }

  list->num_elements--;

  // Only one element in linked list
  if (list->num_elements == 0) {
    // Set the head AND tail pointer in list to null
    // Free the ListNode
    *payload_ptr = list->tail->payload;
    free(list->tail);
    list->tail = NULL;
    list->head = NULL;
  } else {
    // Set tail->prev->next = NULL;
    list->tail->prev->next = NULL;

    // Set the tail pointer equal to tail->prev;
    LinkedListNode* new_tail = list->tail->prev;
    // free the ListNode
    *payload_ptr = list->tail->payload;
    free(list->tail);
    list->tail = new_tail;
  }

  return true;  // you may need to change this return value
}

void LLIteratorRewind(LLIterator *iter) {
  iter->node = iter->list->head;
}
