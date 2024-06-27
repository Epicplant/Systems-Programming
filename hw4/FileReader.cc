/*
 * Copyright ©2023 Chris Thachuk.  All rights reserved.  Permission is
 * hereby granted to the students registered for University of Washington
 * CSE 333 for use solely during Fall Quarter 2023 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 *
 * 
 */


#include <stdio.h>
#include <string>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory>


extern "C" {
  #include "libhw1/CSE333.h"
}


extern "C" {
  #include "libhw2/FileParser.h"
}

#include "./HttpUtils.h"
#include "./FileReader.h"

using std::string;
using std::unique_ptr;

namespace hw4 {

bool FileReader::ReadFile(string* const contents) {
  string full_file = basedir_ + "/" + fname_;

  // Read the file into memory, and store the file contents in the
  // output parameter "contents."  Be careful to handle binary data
  // correctly; i.e., you probably want to use the two-argument
  // constructor to std::string (the one that includes a length as a
  // second argument).
  //
  // You might find ::ReadFileToString() from HW2 useful here. Remember that
  // this function uses malloc to allocate memory, so you'll need to use
  // free() to free up that memory after copying to the string output
  // parameter.
  //
  // Alternatively, you can use a unique_ptr with a malloc/free
  // deleter to automatically manage this for you; see the comment in
  // HttpUtils.h above the MallocDeleter class for details.

  // STEP 1:

  // Make sure path is safe before continuing (prevent directory
  // traversal attacks)
  if (!IsPathSafe(basedir_, full_file))
    return false;

  // Get the file contents
  int size;
  char* read_info = ::ReadFileToString(full_file.c_str(), &size);

  // if read_info is null then we failed so return false
  if (read_info == nullptr) {
    return false;
  }

  // Get return parameter and free malloced info from ReadFileToSTring
  *contents = string(read_info, size);
  free(read_info);

  return true;
}

}  // namespace hw4
