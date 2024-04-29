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

#include <cstdlib>  // for EXIT_SUCCESS, EXIT_FAILURE
#include <iostream>  // for std::cout, std::cerr, etc.
#include <sstream>
#include <algorithm>

#include "./QueryProcessor.h"

using std::cerr;
using std::cout;
using std::endl;
using std::istringstream;
using std::cin;
using std::getline;
using std::transform;

// Error usage message for the client to see
// Arguments:
// - prog_name: Name of the program
static void Usage(char* prog_name);

// Enquires the user for words to query. If ctrl+d is pressed
// then GetWords returns false, otherwise the command line input
// provided is processed into space separated strings which are
// placed within the output parameter return_query and we return
// true.
static bool GetWords(vector<string>* const return_query);

// Searches the provided index files (whose paths
// are stored within the indexes list) for all files
// the contain the every word entered via command line.
static void ProcessQueries(list<string> indexes);

// Your job is to implement the entire filesearchshell.cc
// functionality. We're essentially giving you a blank screen to work
// with; you need to figure out an appropriate design, to decompose
// the problem into multiple functions or classes if that will help,
// to pick good interfaces to those functions/classes, and to make
// sure that you don't leak any memory.
//
// Here are the requirements for a working solution:
//
// The user must be able to run the program using a command like:
//
//   ./filesearchshell ./foo.idx ./bar/baz.idx /tmp/blah.idx [etc]
//
// i.e., to pass a set of filenames of indices as command line
// arguments. Then, your program needs to implement a loop where
// each loop iteration it:
//
//  (a) prints to the console a prompt telling the user to input the
//      next query.
//
//  (b) reads a white-space separated list of query words from
//      std::cin, converts them to lowercase, and constructs
//      a vector of c++ strings out of them.
//
//  (c) uses QueryProcessor.cc/.h's QueryProcessor class to
//      process the query against the indices and get back a set of
//      query results.  Note that you should instantiate a single
//      QueryProcessor  object for the lifetime of the program, rather
//      than  instantiating a new one for every query.
//
//  (d) print the query results to std::cout in the format shown in
//      the transcript on the hw3 web page.
//
// Also, you're required to quit out of the loop when std::cin
// experiences EOF, which a user passes by pressing "control-D"
// on the console.  As well, users should be able to type in an
// arbitrarily long query -- you shouldn't assume anything about
// a maximum line length.  Finally, when you break out of the
// loop and quit the program, you need to make sure you deallocate
// all dynamically allocated memory.  We will be running valgrind
// on your filesearchshell implementation to verify there are no
// leaks or errors.
//
// You might find the following technique useful, but you aren't
// required to use it if you have a different way of getting the
// job done.  To split a std::string into a vector of words, you
// can use a std::stringstream to get the job done and the ">>"
// operator. See, for example, "gnomed"'s post on stackoverflow for
// their example on how to do this:
//
//   http://stackoverflow.com/questions/236129/c-how-to-split-a-string
//
// (Search for "gnomed" on that page. They use an istringstream, but
// a stringstream gets the job done too.)
//
// Good luck, and write beautiful code!
int main(int argc, char **argv) {
  if (argc < 2) {
    Usage(argv[0]);
  }

  // STEP 1:
  // Implement filesearchshell!
  // Probably want to write some helper methods ...

  // Receive all indexes from the command line
  list<string> indexes;
  for (int i = 1; i < argc; i++) {
    indexes.push_front(argv[i]);
  }

  ProcessQueries(indexes);

  return EXIT_SUCCESS;
}

static void ProcessQueries(list<string> indexes) {
    // Loop for as long as ctrl+d is not called
  hw3::QueryProcessor qr(indexes, true);
  while (1) {
    vector<string> query;
    bool returnornot = GetWords(&query);
    if (!returnornot) {
      break;
    } else if (query.size() == 0) {
      continue;
    }

    // Receives a vector containing files that contains all the query words
    // alongside the total number of occurrences for the words.
    vector<hw3::QueryProcessor::QueryResult> results = qr.ProcessQuery(query);

    if (results.size() == 0) {
      cout << "  [no results]" << endl;
    } else {
      // Print out the QueryResults
      for (auto result : results) {
        cout << "  " << result.document_name <<
        " ("  << result.rank << ")" << endl;
      }
    }
  }
}

static bool GetWords(vector<string>* const return_query) {
  // Read the next line of input.
  cout << "Enter query:" << endl;

  // Receive a string inputted in the terminal.
  // If ctrl+d is pressed getline returns false and
  // we return nullptr to indicate shutdown
  string line;
  if (!getline(cin, line)) {
    return false;
  }

  // Using istringstream to loop through every string
  // token (space separated word) found within the line
  // string. These string tokens are then added to the query
  // vector in lowercase form.
  string word;
  vector<string> query;
  istringstream iss(line, istringstream::in);
  while (iss >> word) {
    transform(word.begin(), word.end(), word.begin(), ::tolower);
    query.push_back(word);
  }
  *return_query = query;
  return true;
}

static void Usage(char *prog_name) {
  cerr << "Usage: " << prog_name << " [index files+]" << endl;
  exit(EXIT_FAILURE);
}
