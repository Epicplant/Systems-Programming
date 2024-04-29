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
#include <stdint.h>

#include "CSE333.h"
#include "HashTable.h"
#include "LinkedList.h"
#include "HashTable_priv.h"

///////////////////////////////////////////////////////////////////////////////
// Internal helper functions.
//
#define INVALID_IDX -1

// Grows the hashtable (ie, increase the number of buckets) if its load
// factor has become too high.
static void MaybeResize(HashTable *ht);

// A simple function to be passed into LLIterator_Remove
// that frees a payload (the one that is given).
static void FreePayload(LLPayload_t payload);

// A functon that takes an iterator and moves that iterator
// to an associated key-value pair if it exists (a key-value
// pair where newkeyvalue.key equals the key of the pair).
// reachElement returns true if such an element exists
// within iter and false if it doesn't. keyvalue represents the
// value that is being replaced/found/removed.
static bool ReachElement(LLIterator* iter, HTKey_t newkeyvalue,
                                    HTKeyValue_t* keyvalue);

int HashKeyToBucketNum(HashTable *ht, HTKey_t key) {
  return key % ht->num_buckets;
}

// Deallocation functions that do nothing.  Useful if we want to deallocate
// the structure (eg, the linked list) without deallocating its elements or
// if we know that the structure is empty.
static void LLNoOpFree(LLPayload_t freeme) { }
static void HTNoOpFree(HTValue_t freeme) { }


///////////////////////////////////////////////////////////////////////////////
// HashTable implementation.

HTKey_t FNVHash64(unsigned char *buffer, int len) {
  // This code is adapted from code by Landon Curt Noll
  // and Bonelli Nicola:
  //     http://code.google.com/p/nicola-bonelli-repo/
  static const uint64_t FNV1_64_INIT = 0xcbf29ce484222325ULL;
  static const uint64_t FNV_64_PRIME = 0x100000001b3ULL;
  unsigned char *bp = (unsigned char *) buffer;
  unsigned char *be = bp + len;
  uint64_t hval = FNV1_64_INIT;

  // FNV-1a hash each octet of the buffer.
  while (bp < be) {
    // XOR the bottom with the current octet.
    hval ^= (uint64_t) * bp++;
    // Multiply by the 64 bit FNV magic prime mod 2^64.
    hval *= FNV_64_PRIME;
  }
  return hval;
}

HashTable* HashTable_Allocate(int num_buckets) {
  HashTable *ht;
  int i;

  Verify333(num_buckets > 0);

  // Allocate the hash table record.
  ht = (HashTable *) malloc(sizeof(HashTable));
  Verify333(ht != NULL);

  // Initialize the record.
  ht->num_buckets = num_buckets;
  ht->num_elements = 0;
  ht->buckets = (LinkedList **) malloc(num_buckets * sizeof(LinkedList *));
  Verify333(ht->buckets != NULL);
  for (i = 0; i < num_buckets; i++) {
    ht->buckets[i] = LinkedList_Allocate();
  }

  return ht;
}

void HashTable_Free(HashTable *table,
                    ValueFreeFnPtr value_free_function) {
  int i;

  Verify333(table != NULL);

  // Free each bucket's chain.
  for (i = 0; i < table->num_buckets; i++) {
    LinkedList *bucket = table->buckets[i];
    HTKeyValue_t *kv;

    // Pop elements off the chain list one at a time.  We can't do a single
    // call to LinkedList_Free since we need to use the passed-in
    // value_free_function -- which takes a HTValue_t, not an LLPayload_t -- to
    // free the caller's memory.
    while (LinkedList_NumElements(bucket) > 0) {
      Verify333(LinkedList_Pop(bucket, (LLPayload_t *)&kv));
      value_free_function(kv->value);
      free(kv);
    }
    // The chain is empty, so we can pass in the
    // null free function to LinkedList_Free.
    LinkedList_Free(bucket, LLNoOpFree);
  }

  // Free the bucket array within the table, then free the table record itself.
  free(table->buckets);
  free(table);
}

int HashTable_NumElements(HashTable *table) {
  Verify333(table != NULL);
  return table->num_elements;
}


static bool ReachElement(LLIterator* iter, HTKey_t newkey, HTKeyValue_t* keyvalue) {
  // Loop through chain until I either reach the desired key
  // or the list is no longer valid
  while (LLIterator_IsValid(iter)) {
    HTKeyValue_t* payload;
    LLIterator_Get(iter, (LLPayload_t*) &payload);
    if (newkey == payload->key) {
      // Sets keyvalue equal to pointed to node
      // by iter
      HTKeyValue_t* payload;
      LLIterator_Get(iter, (LLPayload_t*) &payload);
      keyvalue->value = payload->value;
      keyvalue->key = payload->key;
      return true;
    }
    LLIterator_Next(iter);
  }
  return false;
}

bool HashTable_Insert(HashTable *table,
                      HTKeyValue_t newkeyvalue,
                      HTKeyValue_t *oldkeyvalue) {
  int bucket;
  LinkedList *chain;

  Verify333(table != NULL);
  MaybeResize(table);

  // Calculate which bucket and chain we're inserting into.
  bucket = HashKeyToBucketNum(table, newkeyvalue.key);
  chain = table->buckets[bucket];

  // STEP 1: finish the implementation of InsertHashTable.
  // This is a fairly complex task, so you might decide you want
  // to define/implement a helper function that helps you find
  // and optionally remove a key within a chain, rather than putting
  // all that logic inside here.  You might also find that your helper
  // can be reused in steps 2 and 3.

  LLIterator* iter = LLIterator_Allocate(chain);

  // Loops through the chain of the bucket we are being put into
  // to see if the key we have already exists. If we reach
  // the end of the chain without finding it, we stop looping.
  // If we do find it and get insertsuccess = false,
  // we update the link in the chain and return
  // true.
  bool insertsuccess = reachElement(iter, newkeyvalue.key, oldkeyvalue);

  if (insertsuccess) {
    HTKeyValue_t* payload;
    LLIterator_Get(iter, (LLPayload_t*) &payload);
    payload->value = newkeyvalue.value;
    LLIterator_Free(iter);
    return true;
  }

  // key-value pair doesn't exist in the chain, so we allocate
  // space for its payload and add it.
  HTKeyValue_t* newvalue = (HTKeyValue_t*) malloc(sizeof(HTKeyValue_t));
  Verify333(newvalue != NULL);
  *newvalue = newkeyvalue;
  table->num_elements++;
  LinkedList_Append(chain, newvalue);
  LLIterator_Free(iter);
  return false;  // you may need to change this return value
}

bool HashTable_Find(HashTable *table,
                    HTKey_t key,
                    HTKeyValue_t *keyvalue) {
  Verify333(table != NULL);

  // Step 2:
  // Uses hashing to find the right bucket and its chain
  // for the key value pair.
  int bucket = HashKeyToBucketNum(table, key);
  LinkedList* chain = table->buckets[bucket];


  LLIterator* iter = LLIterator_Allocate(chain);

  // Loops through the associated chain for the bucket we are in.
  // If we find the key-value we are looking for, we return true
  // otherwise we exit the loop and return false.

  bool findsuccess = reachElement(iter, key, keyvalue);

  if (findsuccess) {
    LLIterator_Free(iter);
    return true;
  }

  LLIterator_Free(iter);
  return false;  // you may need to change this return value
}


static void FreePayload(LLPayload_t payload) {
  free(payload);
}

bool HashTable_Remove(HashTable *table,
                      HTKey_t key,
                      HTKeyValue_t *keyvalue) {
  Verify333(table != NULL);

  // STEP 3: implement HashTable_Remove.


  int bucket = HashKeyToBucketNum(table, key);
  LinkedList* chain = table->buckets[bucket];


  LLIterator* iter = LLIterator_Allocate(chain);

  // Loops through the associated chain for the bucket we are in.
  // If we find the key-value we are looking for, we return true
  // otherwise we exit the loop and return false.

  bool removesuccess = reachElement(iter, key, keyvalue);

  if (removesuccess) {
    LLIterator_Remove(iter, FreePayload);
    table->num_elements--;
    LLIterator_Free(iter);
    return true;
  }

  LLIterator_Free(iter);
  return false;  // you may need to change this return value
}

///////////////////////////////////////////////////////////////////////////////
// HTIterator implementation.

HTIterator* HTIterator_Allocate(HashTable *table) {
  HTIterator *iter;
  int         i;

  Verify333(table != NULL);

  iter = (HTIterator *) malloc(sizeof(HTIterator));
  Verify333(iter != NULL);

  // If the hash table is empty, the iterator is immediately invalid,
  // since it can't point to anything.
  if (table->num_elements == 0) {
    iter->ht = table;
    iter->bucket_it = NULL;
    iter->bucket_idx = INVALID_IDX;
    return iter;
  }

  // Initialize the iterator.  There is at least one element in the
  // table, so find the first element and point the iterator at it.
  iter->ht = table;
  for (i = 0; i < table->num_buckets; i++) {
    if (LinkedList_NumElements(table->buckets[i]) > 0) {
      iter->bucket_idx = i;
      break;
    }
  }
  Verify333(i < table->num_buckets);  // make sure we found it.
  iter->bucket_it = LLIterator_Allocate(table->buckets[iter->bucket_idx]);
  return iter;
}

void HTIterator_Free(HTIterator *iter) {
  Verify333(iter != NULL);
  if (iter->bucket_it != NULL) {
    LLIterator_Free(iter->bucket_it);
    iter->bucket_it = NULL;
  }
  free(iter);
}

bool HTIterator_IsValid(HTIterator *iter) {
  Verify333(iter != NULL);

  // STEP 4: implement HTIterator_IsValid.
  return iter->bucket_it != NULL;  // you may need to change this return value
}

bool HTIterator_Next(HTIterator *iter) {
  Verify333(iter != NULL);

  // STEP 5: implement HTIterator_Next.

  // Check to see if the hashtable iterator is valid still
  if (!HTIterator_IsValid(iter)) {
    return false;
  }

  // Check if the current chain you are on is valid. if so simply progress
  // that chain
  if (LLIterator_IsValid(iter->bucket_it)) {
     LLIterator_Next(iter->bucket_it);
  }

  // If the chain is not valid, we must look for a new bucket_it
  // that is valid. Traverse the tables chanins until a linked
  // list is found chain = table->buckets[bucket];
  while (!LLIterator_IsValid(iter->bucket_it)) {
    LLIterator_Free(iter->bucket_it);
    if (iter->ht->num_buckets <= iter->bucket_idx+1) {
      iter->bucket_it = NULL;
      iter->bucket_idx = -1;
      return false;
    }
    iter->bucket_idx++;
    iter->bucket_it = LLIterator_Allocate(iter->ht->buckets[iter->bucket_idx]);
  }

  return true;
}

bool HTIterator_Get(HTIterator *iter, HTKeyValue_t *keyvalue) {
  Verify333(iter != NULL);

  // STEP 6: implement HTIterator_Get.

  // If the iterator actually works, then search in the provided
  // bucket iterator (which points at the current
  // hashtable index chain) for the key provided.
  if (HTIterator_IsValid(iter) && iter->ht->num_buckets > 0) {
    HTKeyValue_t *keyer;
    LLIterator_Get(iter->bucket_it, (LLPayload_t*) &keyer);
    *keyvalue = *keyer;
    return true;  // you may need to change this return value
  }
  return false;
}

bool HTIterator_Remove(HTIterator *iter, HTKeyValue_t *keyvalue) {
  HTKeyValue_t kv;

  Verify333(iter != NULL);

  // Try to get what the iterator is pointing to.
  if (!HTIterator_Get(iter, &kv)) {
    return false;
  }

  // Advance the iterator.  Thanks to the above call to
  // HTIterator_Get, we know that this iterator is valid (though it
  // may not be valid after this call to HTIterator_Next).
  HTIterator_Next(iter);

  // Lastly, remove the element.  Again, we know this call will succeed
  // due to the successful HTIterator_Get above.
  Verify333(HashTable_Remove(iter->ht, kv.key, keyvalue));
  Verify333(kv.key == keyvalue->key);
  Verify333(kv.value == keyvalue->value);

  return true;
}

static void MaybeResize(HashTable *ht) {
  HashTable *newht;
  HashTable tmp;
  HTIterator *it;

  // Resize if the load factor is > 3.
  if (ht->num_elements < 3 * ht->num_buckets)
    return;

  // This is the resize case.  Allocate a new hashtable,
  // iterate over the old hashtable, do the surgery on
  // the old hashtable record and free up the new hashtable
  // record.
  newht = HashTable_Allocate(ht->num_buckets * 9);

  // Loop through the old ht copying its elements over into the new one.
  for (it = HTIterator_Allocate(ht);
       HTIterator_IsValid(it);
       HTIterator_Next(it)) {
    HTKeyValue_t item, unused;

    Verify333(HTIterator_Get(it, &item));
    HashTable_Insert(newht, item, &unused);
  }

  // Swap the new table onto the old, then free the old table (tricky!).  We
  // use the "no-op free" because we don't actually want to free the elements;
  // they're owned by the new table.
  tmp = *ht;
  *ht = *newht;
  *newht = tmp;

  // Done!  Clean up our iterator and temporary table.
  HTIterator_Free(it);
  HashTable_Free(newht, &HTNoOpFree);
}
