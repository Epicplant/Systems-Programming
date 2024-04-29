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

#include <iostream>
#include <fstream>
#include <map>

using std::cerr;
using std::endl;
using std::string;
using std::cout;
using std::ifstream;
using std::map;

// Takes a token from the ifstream and then, if possible, converts
// it into a value of type T. If the conversion fails we return false;
// if it succeeds, we return true and have out point to the converted value.
// The output is invalid if returned false (and not eof).
template <typename T>
bool ReadValue(ifstream& in, T* const output);

// Standard main function
int main(int argc, char **argv) {
  // Check that there are only 2 arguments
  if (argc != 2) {
    cerr << "There should be no arguments ran with this executable" << endl;
    return EXIT_FAILURE;
  }

  // Create variables
  ifstream fin;
  string word;
  map<string, int> wordcount;

  // Open the file (ifstream automatically closes the file in destructor)
  fin.open(argv[1]);

  // Check to see if file failed to open
  if (!fin) {
    cerr << "Could not open file with given file name" << endl;
    return EXIT_FAILURE;
  }

  // Execute a loop until an error occurs while reading values
  while (ReadValue(fin, &word)) {
    // Place the tokens into the map
    wordcount[word]++;
  }

  // Check to see if the loop ended because of a failed conversion
  // or because it reached eof
  if (!fin.eof()) {
    cerr << "Error in conversion from file token to type T" << endl;
    return EXIT_FAILURE;
  }

  // Iterate through the every value in the map
  // and print out the key-value pairs. Map
  // has keys lexicograhically sorted for iterator.
  for (const auto& kv : wordcount) {
    cout << kv.first << " " << kv.second << endl;
  }

  // Ran successfully!
  return EXIT_SUCCESS;
}

template <typename T>
bool ReadValue(ifstream& in, T* const output) {
  in>>*output;
  if (in.fail()) {
    return false;
  }
  return true;
}
