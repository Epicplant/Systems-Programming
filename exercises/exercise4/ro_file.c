/*
 * Copyright ©2023 Chris Thachuk.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Fall Quarter 2023 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 * 
 * Name: Christopher Roy
 * Email: cr0y@uw.edu
 */

#include "ro_file.h"
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>


/*** INTERNAL DATA TYPES AND CONSTANTS **************************************/

static const int RO_FILE_BUF_LEN = 512;  // do not modify

struct ro_file_st {
  int fd;         // The file descriptor we are currently managing.

  char* buf;      // Pointer to our internal buffer for this file.

  off_t buf_pos;  // The position in the file that the beginning of our
                  // internal buffer currently corresponds to.

  int buf_index;  // The index in our internal buffer that corresponds to the
                  // user's current position in the file.

  int buf_end;    // How many bytes currently in our internal buffer are
                  // actually from the file.
                  // To understand why this is important to track, consider
                  // the case when the file length is < RO_FILE_BUF_LEN.
};


/*** STATIC HELPER FUNCTION DECLARATIONS ************************************/

// Copy up to 'amount' bytes from the RO_FILE's internal buffer to 'out'.
// Returns the number of bytes copied.
static size_t flush_buffer(RO_FILE* file, char* out, int amount);

// Fill in the RO_FILE's internal buffer with as many bytes as can be read
// from the file.
// Returns the number of bytes copied into the buffer, or -1 on any error.
static ssize_t fill_buffer(RO_FILE* file);


/*** FUNCTION DEFINITIONS ***************************************************/

RO_FILE* ro_open(char* filename) {
  // 1. Allocate a new RO_FILE
  RO_FILE* fileptr = (RO_FILE*) malloc(sizeof(RO_FILE));

  if (fileptr != NULL && filename != NULL) {
    // 2. Get the file descriptor for the file
    fileptr->fd = open(filename, O_RDONLY);

    if (fileptr->fd == -1) {
      if (errno != EINTR && errno != EAGAIN) {
        return NULL;
      }
    }

    // 3. Allocate the internal buffer
    fileptr->buf = (char*) malloc(RO_FILE_BUF_LEN);

    if (fileptr->buf == NULL) {
      return NULL;
    }

    // 4. Initialize the other fields (no reading done yet)
    fileptr->buf_pos = 0;
    fileptr->buf_index = 0;
    fileptr->buf_end = 0;
  }

  return fileptr;
}

ssize_t ro_read(char* ptr, size_t len, RO_FILE* file) {
  // 1. If we have bytes in our internal buffer, flush as many as we can to
  //    'ptr'.
  size_t num_copied_out = flush_buffer(file, ptr, len);

  while (num_copied_out != len) {
    // 2. If we haven't provided the bytes requested, repopulate our buffer
    //    with bytes from the file.
    ssize_t num_filled = fill_buffer(file);
    if (num_filled == 0) {
      // reached EOF; no more bytes to copy
      break;
    } else if (num_filled < 0) {
      // pass error back to client
      return num_filled;
    }

    // 3. Copy filled bytes into 'ptr'.
    num_copied_out += flush_buffer(file, ptr+num_copied_out,
                                   len-num_copied_out);

    // 4. Repeat steps 2-3 until request is fulfilled.
  }

  return num_copied_out;
}

off_t ro_tell(RO_FILE* file) {
  if (file == NULL) {
    return -1;
  }
  return file->buf_pos + file->buf_index;
}

int ro_seek(RO_FILE* file, off_t offset, int whence) {
  // 1. Check validity of arguments, where applicable.
  if (file == NULL) {
    return EXIT_FAILURE;
  }

  // 2. Seek to specified offset from specified whence using lseek.
  //    No need to check if new position is already in our buffer.
  off_t off = lseek(file->fd, offset, whence);

  // 3. Update our buffer indicators (if off is -1 return error)
  if (off == -1) {
    if (errno != EINTR && errno != EAGAIN) {
      return EXIT_FAILURE;
    }
  }

  file->buf_pos = off;
  file->buf_end = 0;
  file->buf_index = 0;

  return EXIT_SUCCESS;
}

int ro_close(RO_FILE* file) {
  if (file == NULL) {
    return 1;
  }

  free(file->buf);
  close(file->fd);
  free(file);

  // Clean up all RO_FILE resources, returns non-zero on error
  return 0;
}


/*** STATIC HELPER FUNCTION DEFINITIONS *************************************/

size_t flush_buffer(RO_FILE* file, char* out, int amount) {
  // 1. Copy/flush bytes to 'out' starting from the buffer index. The amount
  //    flushed should be the min of 'amount' and the remaining unflushed bytes
  //    in the buffer.

  // iterate through all of buffer or until we run out of amount
  // and move the amount to the out pointer
  ssize_t index = 0;
  while (index < file->buf_end && amount != 0) {
    out[index] = file->buf[file->buf_index+index];
    index++;
    amount--;
  }

  // 2. Advance buffer index by the number of bytes flushed.
  file->buf_index += (index);

  // 3. Return the number of bytes flushed.
  return index;
}

ssize_t fill_buffer(RO_FILE* file) {
  // NOTES:
  // - For maximum buffering benefit, we are "resetting" the buffer and then
  //   filling it with as much file data as possible, starting from the current
  //   file position -- how does this change the buffer indicators?
  // - For simplicity, you should ignore any unflushed bytes that started in
  //   the buffer (i.e., it's okay to re-read them from the file).
  // - You will need to implement a POSIX read loop with all appropriate
  //   return value checking.

  if (file == NULL) {
    return -1;
  }

  // Loop until we are only reading 0 bytes (at which point the buffer)
  // is full.
  char* ptr = file->buf;
  ssize_t result;
  ssize_t count = 0;
  while ((result = read(file->fd, ptr, file->buf + RO_FILE_BUF_LEN - ptr))
  != 0) {
    // If result is -1 then an error has lkely occurred and we
    // test to see if the value is bad
    if (result == -1) {
      if (errno != EINTR && errno != EAGAIN) {
        return result;
      }
    }

    // increment count and ptr by result
    count += result;
    ptr += result;
  }

  file->buf_end += count;
  return count;
}
