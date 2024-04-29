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

#include "SocketUtil.h"

#include <unistd.h>
#include <iostream>
#include <string.h>

using std::cerr;
using std::endl;

#define BUF_SIZE 512

// Create a listening socket, accept a connection from a client,
// and write all the data the client sends to stdout.
int main(int argc, char** argv) {
  int sock_family;
  int listening_fd = Listen(argv[1], &sock_family);
  if (listening_fd == -1) {
    // Errors printed within Listen function
    return EXIT_FAILURE;
  }

  // Loop forever, when a client connects to the
  // port read what they are writing to us and then
  int client_fd;
  while (1) {
    struct sockaddr_storage caddr;
    socklen_t caddr_len = sizeof(caddr);
    client_fd = accept(listening_fd,
                           reinterpret_cast<struct sockaddr*>(&caddr),
                           &caddr_len);
    if (client_fd < 0) {
      if ((errno == EINTR) || (errno == EAGAIN) || (errno == EWOULDBLOCK))
        continue;
      cerr << "Failure on accept: " << strerror(errno) << endl;
      break;
    }

    // Read 512 bytes from file until the file is empty. If read has an error
    // exit with failure.
    unsigned char buf[BUF_SIZE+1];
    int readlen;
    while ((readlen = WrappedRead(client_fd, buf, BUF_SIZE)) != 0) {
      if (readlen == -1) {
        close(listening_fd);
        close(client_fd);
        cerr << "Failed to read from local file" << endl;
        return EXIT_FAILURE;
      }

      buf[readlen] = '\0';

      // Write the 512 byte string to stdout
      if (WrappedWrite(1, buf, readlen) != readlen) {
        close(listening_fd);
        close(client_fd);
        cerr << "Failed to write across socket" << endl;
        return EXIT_FAILURE;
      }
    }

    // Close up shop.
    close(client_fd);
    break;
  }

  close(listening_fd);
  return EXIT_SUCCESS;
}
