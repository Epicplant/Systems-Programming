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

// Feature test macro for strtok_r (c.f., Linux Programming Interface p. 63)
#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "libhw1/CSE333.h"
#include "./CrawlFileTree.h"
#include "./DocTable.h"
#include "./MemIndex.h"

static const int BUF_SIZE = 2;
static const int MAX_ARG_NUM = 2;

//////////////////////////////////////////////////////////////////////////////
// Helper function declarations, constants, etc

// Prints ut errors when we don't have enough executable args
static void Usage(void);

// Initalizes the request input loop. Everytime the user provides terminal input
// the alphabetical words within it are parsed into an array of words which are
// then used to print out which files in the originally provided directory
// contain all words.
static void ProcessQueries(DocTable* dt, MemIndex* mi);

// Read a line of input from stdin using fgets() and return a a new string
// through the output parameter ret_str and the strings
// number of spaces, or -1 on error.
// Caller is responsible for free()'ing the returned string.  Does not
// include a newline at the end of the string.
static int GetNextLine(FILE* f, char** ret_str);


//////////////////////////////////////////////////////////////////////////////
// Main
int main(int argc, char** argv) {
  if (argc != MAX_ARG_NUM) {
    Usage();
  }

  // Implement searchshell!  We're giving you very few hints
  // on how to do it, so you'll need to figure out an appropriate
  // decomposition into functions as well as implementing the
  // functions.  There are several major tasks you need to build:
  //
  //  - Crawl from a directory provided by argv[1] to produce and index
  //  - Prompt the user for a query and read the query from stdin, in a loop
  //  - Split a query into words (check out strtok_r)
  //  - Process a query against the index and print out the results
  //
  // When searchshell detects end-of-file on stdin (cntrl-D from the
  // keyboard), searchshell should free all dynamically allocated
  // memory and any other allocated resources and then exit.
  //
  // Note that you should make sure the fomatting of your
  // searchshell output exactly matches our solution binaries
  // to get full points on this part.

  // Step 1: Obtain the inverted index table.

  // Obtain DocTable
  DocTable* dt;

  // Obtain MemIndex
  MemIndex* mi;

  fprintf(stdout, "Indexing '%s'\n", argv[1]);

  // If the directory path doesn't exist then we exit failure.
  // However, if it does exist, the DocTable and MemIndex
  // objects are filled with the contents of the directory
  if (!CrawlFileTree(argv[1], &dt, &mi)) {
    fprintf(stderr,
    "The directory path/name does not exist\n");
    DocTable_Free(dt);
    MemIndex_Free(mi);
    return EXIT_FAILURE;
  }

  // Requests user for input and begins the loop of processing
  // user input into queries.
  ProcessQueries(dt, mi);

  // Free everything and exit with success
  DocTable_Free(dt);
  MemIndex_Free(mi);
  return EXIT_SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////
// Helper function definitions

static void Usage(void) {
  fprintf(stderr, "Usage: ./searchshell <docroot>\n");
  fprintf(stderr,
          "where <docroot> is an absolute or relative " \
          "path to a directory to build an index under.\n");
  exit(EXIT_FAILURE);
}

static void ProcessQueries(DocTable* dt, MemIndex* mi) {
  char* input;
  char* token;
  char* saveptr;
  int numwords;

  while (1) {
    // Read the next line of input.
    fprintf(stdout, "enter query:\n");

    // Receives the inputted string from the user which we then
    // use to determine the number of words inputted
    numwords = GetNextLine(stdin, &input) + 1;

    // Ctrl + d was detected from keyboard so we exit
    if (numwords == 0) {
      // EOF
      fprintf(stdout, "shutting down...\n");
      break;
    }

    // Create the array based on the number of words
    char* queries[numwords];

    // Populate queries with all the words using strtok_r
    int index = 0;
    token = strtok_r(input, " ", &saveptr);
    while ((token != NULL)) {
      queries[index] = token;
      index++;
      token = strtok_r(NULL, " ", &saveptr);
    }

    // Use memindexes functions to return the desired data
    LinkedList* ll = MemIndex_Search(mi, queries, index);
    if (ll != NULL) {
      LLIterator* liter = LLIterator_Allocate(ll);
      Verify333(liter != NULL);

      // For every value in the returned memindex linked list,
      // print out its contents
      while (LLIterator_IsValid(liter)) {
        SearchResult* sr;

        LLIterator_Get(liter, (LLPayload_t*) &sr);
        fprintf(stdout, "  %s (%d)\n",
        DocTable_GetDocName(dt, sr->doc_id), sr->rank);
        LLIterator_Next(liter);
      }
      LLIterator_Free(liter);
      LinkedList_Free(ll, (LLPayloadFreeFnPtr) free);
    }
    free(input);
  }
}

static int GetNextLine(FILE *f, char **ret_str) {
  char *linestr = *ret_str;
  int count = 0;
  int numspaces = 0;

  // Initialize linestr to contain a NULL-terminated
  // string.  We'll grow it one character at a time
  // using realloc().  ("man realloc")
  linestr = (char *) malloc(1 * sizeof(char));
  if (linestr == NULL)
    return -1;
  linestr[0] = '\0';

  while (1) {
    // Allocate space for the next character + NULL. Yes, this
    // is not the most elegant use of realloc(), but it gets the
    // job done. ;)
    linestr = realloc(linestr, count+BUF_SIZE);
    if (linestr == NULL) {
      free(linestr);
      return -1;
    }

    // Read in the next character.  Note fgets reads (size-1)
    // characters and adds a '\0', so we need size=2.
    if (fgets(&(linestr[count]), BUF_SIZE, f) == NULL) {
      // We hit the end of file. Return our string, or null
      // if we didn't read anything.
      if (count == 0) {
        free(linestr);
        return -1;
      }
      *ret_str = linestr;
      return numspaces;
    }

    // Make every character lowercase where applicable
    linestr[count] = tolower(linestr[count]);

    // If the character is a space, then increment numspaces
    if (linestr[count] == ' ') {
      numspaces++;
    }

    if (linestr[count] == '\n') {
      // We hit the end of a line.  Return our string, but overwrite
      // the newline with '\0'.
      linestr[count] = '\0';
      *ret_str = linestr;
      return numspaces;
    }
    // fall through to next loop.
    count++;
  }
}
