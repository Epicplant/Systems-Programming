# Bug 1

## A) How is your program acting differently than you expect it to?
- When I am reading my values from my table in FileParser, the words I am storing
aren't returning what I put in

## B) Brainstorm a few possible causes of the bug
- Something else may point to that address and be overriding it
- I put in the value incorrectly (wasn't put in the table, interpreted the data field wrong, etc.)
- The stack frame may break it when I leave the function

## C) How you fixed the bug and why the fix was necessary
- I realized I wasn't mallocing space for the string in the heap, which resulted in the heap
overwrintg part of the string at some point (due to the memory not being reserved for it).
I fixed it by mallocing space for the string and inserting that into the table instead. This
fixed it.


# Bug 2

## A) How is your program acting differently than you expect it to?
- There is a massive memory leak in CrawlFileTree that is resulting
in over 400 lost blocks of data. 

## B) Brainstorm a few possible causes of the bug
- I may be mallocing something that isn't being freed somewhere
- I may be referencing a memory index that is outside of memory that is allocated outside of an array
- I may be referencing something that was already freed or something that has yet
to be intalized

## C) How you fixed the bug and why the fix was necessary
- I read valgrind and traced it back to the general area in which the bug was occurring. I realized I was calling i
when it had yet to be initialized anywhere many times which was referencing data that wasn't allocated anywhere. Ended
up initializing i and nearly all memory issues disappeared (all except 2).


# Bug 3

## A) How is your program acting differently than you expect it to?
- The ranks for all the files aren't showing up after doing Memindex_Search and
I am only getting values of about 1-6 when I print it out in searchshell

## B) Brainstorm a few possible causes of the bug
- I may not be adding to the rankings correctly
- The data may be corrupted by a pointer that is changing t
- I may not be retrieving the ranking properly within searchshell (casting it incorrectly, using the wrong field, etc.)

## C) How you fixed the bug and why the fix was necessary
- Although I passed all the Memindex tests, it turned out that I misinterpreted what the function was supposed to do
when creating the linked lists with rankings. I was defaulting each to rank 0f 0 and then incrementing them later on
(which wasn't be getting me the big numbers I needed). I reread th spec and realized I needed the size of the linked
lists instead since that showed the number of times that word occurred in the file, so I modified my code to start
out as linkedlist size and to += linked list size later on instead of incrementing. This fixed the issue
