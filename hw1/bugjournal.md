# Bug 1

## A) How is your program acting differently than you expect it to?
- Within LinkedList_Pop, my code is not entering my case for when there is only
a single element in the linked list.

## B) Brainstorm a few possible causes of the bug
- The value list->num_elements might be returning the wrong value(not correctly pointing to where I want num_elements to be)
- I may have messed up the conditional (not correctly checking if num_elements is equal to a single value or not using '==' correctly)
- I may be modifying num_elements somewhere else and that is messing up the comparison

## C) How you fixed the bug and why the fix was necessary
- I stepped through GDB and printed out num_elements when I got to the conditional. It turned out list->num_elements was 0
and not 1 when I had a listNode of size 1 because I was subtracting 1 from num_elements right before I checked list->num_elements == 1.
I fixed it by setting list->num_elements == 0 there instead.


# Bug 2

## A) How is your program acting differently than you expect it to?
- When I pop with 2 elements the new element doesn't become both the head and the tail 

## B) Brainstorm a few possible causes of the bug
- Me freeing the payload of head may be messing it up
- My duplication of list->head->next to new_head may be making a copy only
- I may be using new_head incorrectly

## C) How you fixed the bug and why the fix was necessary
- I found out that I forgot to use new_head instead of list->head->next
after I freed list->head. This made it so I was accessing freed dynamic
memory which was giving me the error I was getting.


# Bug 3

## A) How is your program acting differently than you expect it to?
- At one point when HTIterator_Get (from test_hashtable.cc line 226)
is called within test_hashtable.cc, the key-value pair
returned has the key and value datas swapped.



## B) Brainstorm a few possible causes of the bug
- Perhaps somewhere when I am accessing payloads with other
hashtable functions (such as HashTable_Insert), I swap the
values
-  Perhaps the pointer for that associated payload being
gotten is different than the one originally put into that
bucket (it was changed somehow).
- Perhaps the data within the keyvalue pair is being
corrupted by me casting it to a different pointer type

## C) How you fixed the bug and why the fix was necessary
-  I fixed the issue by going into HTIterator_Get, creating
a HTKeyValue_t pointer called keyer, and then passing a pointer
to that into LLIterator_Get instead of keyvalue. By doing this,
I am actually setting the payload (a pointer) equal to a pointer
value in LLIterator_Get (whereas before I was setting a pointer
equal to to a key-value pair). This ultimately fixed the bug
since I wasn't having bad type conversions anymore.
