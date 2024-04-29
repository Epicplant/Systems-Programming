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

#include <stdio.h>    // for snprintf
#include <stdlib.h>   // for EXIT_SUCCESS, NULL
#include <string.h>   // for strrchr, strcmp, strlen
#include <stdbool.h>  // for bool
#include <dirent.h>   // analyze directories

#include "ro_file.h"

static const int BUFF_SIZE = 128;  // do not modify

/*** HELPER FUNCTION DECLARATIONS ******************************************/

// Returns whether or not a filename ends in ".txt".
bool IsTxtFile(char* filename);

// Concatenate the directory and file names into a full path. The caller is
// responsible for freeing the allocated string. Exits if an error occurs.
char* Concatenate(char* dirname, char* filename);


/*
 * This program:
 * - Accepts a directory name as a command-line argument.
 * - Scans through the directory looking for all files whose names end with
 *   the four characters ".txt".
 * - For every such file found, write the contents of those files to stdout
 *   without adding any additional characters or formatting.
 *   Eventually reading the files with ro_file module.
 */
int main(int argc, char** argv) {
  DIR* dir;
  struct dirent* entry;
  size_t readlen;
  char readbuf[BUFF_SIZE+1];

  // If there isn't exactly two arguments (the first
  // being the executable ex4.c and the second being
  // the dir name) then return as a failure as we
  // either have a parameter we know nothing about
  // or no parameter at all.
  if (argc != 2) {
    fprintf(stderr,
    "Exactly one parameter, adirectory path/name, is required\n");
    return EXIT_FAILURE;
  }

  // Tries to open the directory. If the dir variable
  // is null it means no such dir name exists so we return
  // as a failure.
  dir = opendir(argv[1]);
  if (dir == NULL) {
    fprintf(stderr,
            "The directory path/name does not exist\n");
    return EXIT_FAILURE;
  }

  // Iterates through every file in a directory.
  while ((entry = readdir(dir)) != NULL) {
    char* save = entry->d_name;

    // If the file is a txt file, then it is entered
    // and that file is opened.
    if (IsTxtFile(save)) {
      char* path = Concatenate(argv[1], entry->d_name);
      RO_FILE* filein = ro_open(path);
      free(path);

      // If a propbably occurs with opening the file, then an error is thrown
      // and the file closed.
      if (filein == NULL) {
        fprintf(stderr,
                "Failed to open the file %s\n", save);
        ro_close(filein);
        closedir(dir);
        return EXIT_FAILURE;
      }

      // If the file successfully opens, reads through every character
      // byte by byte in the opened file.
      off_t offset = 0;
      while ((readlen = ro_read(readbuf, BUFF_SIZE, filein)) > 0) {
          // Checks that the number of bytes read is the expected size
          // and if not an error is thrown.
          if (readlen == -1) {
            fprintf(stderr,
                "Failed to open the file %s\n", save);
            ro_close(filein);
            closedir(dir);
            return EXIT_FAILURE;
          }

          // prints new string to stdout based
          // on the strings length
          readbuf[readlen] = '\0';
          printf("%s", readbuf);

          // Increments the the offset variable and moves us
          // forward in the file we are reading
          offset += readlen;
          if (ro_seek(filein, offset, SEEK_SET)) {
            fprintf(stderr,
                    "Failed to progress readng %s\n", save);
            closedir(dir);
            ro_close(filein);
            return EXIT_FAILURE;
          }
      }
      // Closes the current file when we are done with it.
      ro_close(filein);
    }
  }

  // Closes the directory when we are done with it.
  closedir(dir);

  return EXIT_SUCCESS;
}


/*** HELPER FUNCTION DEFINITIONS *******************************************/

bool IsTxtFile(char* filename) {
  char* dot = strrchr(filename, '.');
  return dot && !strcmp(dot, ".txt");
}

char* Concatenate(char* dirname, char* filename) {
  bool has_trailing_slash = dirname[strlen(dirname) - 1] == '/';
  // Concatenate directory and file name.
  size_t dlen = strlen(dirname);
  size_t flen = strlen(filename);
  // Malloc space for full path name:
  // dlen + strlen("/") + flen + strlen('\0') = dlen + flen + 2
  int size_to_malloc = has_trailing_slash ? dlen + flen + 1 : dlen + flen + 2;
  char* fullpath = (char*) malloc(sizeof(char) * (size_to_malloc));
  if (fullpath == NULL) {
    fprintf(stderr, "Error on malloc.\n");
    exit(EXIT_FAILURE);
  }
  if (has_trailing_slash) {
    snprintf(fullpath, size_to_malloc, "%s%s", dirname, filename);
  } else {
    snprintf(fullpath, size_to_malloc, "%s/%s", dirname, filename);
  }
  return fullpath;
}
