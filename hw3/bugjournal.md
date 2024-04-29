# Bug 1

## A) How is your program acting differently than you expect it to?
- The CRC being calculated for an index file in WriteIndex keeps
calculating the wrong value for its file 

## B) Brainstorm a few possible causes of the bug
- The crc may be reading from the wrong place in the files (i.e. skipping some bytes,
starting in the wrong place, etc.)
- I might not be reading enough bytes with fread (potentially reading less than what I want
it to read)
- I may be only getting the byte data wrong (noticed I was usng char whereas elsewhere
int8_t was used)

## C) How you fixed the bug and why the fix was necessary
- I ended up fixing the bug by realizing that I was doing the sizeof(char*) (the
size of the buffer array I was using) instead of sizeof(char). I also realized that
I was using the wrong type of fseek (SEEK_CUR instead of SEEK_SET) which was messing up
the position where I read from the file. As a result, I was only reading some of the bytes
as well as reading in the wrong position every iteration which ended up screwing the crc calculations
(due to it needed to read every byte in the file exactly once). 


# Bug 2

## A) How is your program acting differently than you expect it to?
- DocIDTableReader kept returning me the wrong values (failings tests in 
the DocIDTableReader test) when LookupDocID was called (line 60 in particular of
the test suite)

## B) Brainstorm a few possible causes of the bug
- I may be reading the values at wrong increments (seeking incorrectly, not reading the
  proper number of bytes for the desired data, etc.)
- I may be converting the DocIDElementPositions incorrectly (Host, Network, etc.)
- I may not be adding every piece of data into the positionslist I made due to failing
  checks I have for whether a file read is a failure or not (which results in skipping)
  data and closing the file.

## C) How you fixed the bug and why the fix was necessary
- I found out that I wasn't accounting for the header of the DocIDElement I was at, in
multiple places throughout the code. This was causing me to read from the header as well
as the elements while also screwing up the offset of where I was seeking to in general to get
data. I also realized while debugging that I should be using verify333 to check for read errors
instead of closing the file since that would crash in a way that provided more information than
if I just closed the file in the iddle of the program.


# Bug 3

## A) How is your program acting differently than you expect it to?
- Whenever I utilize QueryProcessor to read from multiple index files
through processQuery, the only data that is actually processed and
returned is that from the first index file (all other index files
are ignored).

## B) Brainstorm a few possible causes of the bug
- I may be breaking from the loop to early because all the words I am looking
up don't exist (AKA I read them incorrectly) and I'm getting nullptr's
- I may be returnng to early (there is a test case that return an empty vector
if an index file doesn't contain any of the words)
- I may be using the same DocIDTableReader every single iteration even when
I want to be looking at other indexes

## C) How you fixed the bug and why the fix was necessary
- The second brainstormed issue turned out to be the problem. I thought
we were supposed to only return words that showed up in all index files
so the misinterpreation resulted in me continuously returning nothing
just because one index file didn't have any of the words within it. I fixed
it by instead simply breaking out of the current subloop I was in when I made
that check and instead just continued to a new index file without having added
anything to the return list for this iteration.
