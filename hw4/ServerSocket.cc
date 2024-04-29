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

#include <stdio.h>       // for snprintf()
#include <unistd.h>      // for close(), fcntl()
#include <sys/types.h>   // for socket(), getaddrinfo(), etc.
#include <sys/socket.h>  // for socket(), getaddrinfo(), etc.
#include <arpa/inet.h>   // for inet_ntop()
#include <netdb.h>       // for getaddrinfo()
#include <errno.h>       // for errno, used by strerror()
#include <string.h>      // for memset, strerror()
#include <iostream>      // for std::cerr, etc.

#include "./ServerSocket.h"

#define PORT_BYTES 10

extern "C" {
  #include "libhw1/CSE333.h"
}

namespace hw4 {

ServerSocket::ServerSocket(uint16_t port) {
  port_ = port;
  listen_sock_fd_ = -1;
}

ServerSocket::~ServerSocket() {
  // Close the listening socket if it's not zero.  The rest of this
  // class will make sure to zero out the socket if it is closed
  // elsewhere.
  if (listen_sock_fd_ != -1)
    close(listen_sock_fd_);
  listen_sock_fd_ = -1;
}

// Takes a clients file descriptor and its family
// and returns the connected to servers address,
// port number, and dns name.
void GetServerData(const int& fd,
                   const int& family,
                   std::string* const& dns_name,
                   uint16_t* const& port,
                   std::string* const& addr);

// Takes a sockets addr struct alongside its family
// and returns the clients address, port number,
// and dns name.
void GetClientData(struct sockaddr* const& addr,
                   const size_t& addrlen,
                   std::string* const& dns_name,
                   std::string* const& ret_addr);


bool ServerSocket::BindAndListen(int ai_family, int* const listen_fd) {
  // Use "getaddrinfo," "socket," "bind," and "listen" to
  // create a listening socket on port port_.  Return the
  // listening socket through the output parameter "listen_fd"
  // and set the ServerSocket data member "listen_sock_fd_"

  // STEP 1:
  // Populate the "hints" addrinfo structure for getaddrinfo().
  // ("man addrinfo")
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = ai_family;
  hints.ai_socktype = SOCK_STREAM;  // stream
  hints.ai_flags = AI_PASSIVE;      // use wildcard "in6addr_any" address
  if (ai_family == AF_INET6)
    hints.ai_flags |= AI_V4MAPPED;    // use v4-mapped v6 if no v6 found
  hints.ai_protocol = IPPROTO_TCP;  // tcp protocol
  hints.ai_canonname = nullptr;
  hints.ai_addr = nullptr;
  hints.ai_next = nullptr;

  // getaddrinfo() returns a list of
  // address structures via the output parameter "result".
  struct addrinfo* result;

  char port_num[PORT_BYTES];
  snprintf(port_num, sizeof(port_num), "%hu", port_);

  int res = getaddrinfo(nullptr, port_num, &hints, &result);

  if (res != 0) {
    return false;
  }

  // Loop through the returned address structures until we are able
  // to create a socket and bind to one.  The address structures are
  // linked in a list through the "ai_next" field of result.
  int fd = -1;
  for (struct addrinfo* rp = result; rp != nullptr; rp = rp->ai_next) {
    fd = socket(rp->ai_family,
                       rp->ai_socktype,
                       rp->ai_protocol);

    if (fd == -1) {
      continue;
    }

    // Configure the socket; we're setting a socket "option."  In
    // particular, we set "SO_REUSEADDR", which tells the TCP stack
    // so make the port we bind to available again as soon as we
    // exit, rather than waiting for a few tens of seconds to recycle it.
    int optval = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
               &optval, sizeof(optval));

    // Try binding the socket to the address and port number returned
    // by getaddrinfo().
    if (bind(fd, rp->ai_addr, rp->ai_addrlen) == 0) {
      // Bind worked!
      sock_family_ = ai_family;
      break;
    }

    // The bind failed.  Close the socket, then loop back around and
    // try the next address/port returned by getaddrinfo().
    close(fd);
    fd = -1;
  }

  // Free the structure returned by getaddrinfo() and check fd works.
  freeaddrinfo(result);

  // Failed to get socket at port_
  if (fd == -1)
    return false;

  // Success. Tell the OS that we want this to be a listening socket.
  if (listen(fd, SOMAXCONN) != 0) {
    close(fd);
    return false;
  }

  listen_sock_fd_ = fd;
  *listen_fd = fd;
  return true;
}

bool ServerSocket::Accept(int* const accepted_fd,
                          std::string* const client_addr,
                          uint16_t* const client_port,
                          std::string* const client_dns_name,
                          std::string* const server_addr,
                          std::string* const server_dns_name) const {
  // Accept a new connection on the listening socket listen_sock_fd_.
  // (Block until a new connection arrives.)  Return the newly accepted
  // socket, as well as information about both ends of the new connection,
  // through the various output parameters.

  socklen_t len;
  struct sockaddr* def;
  struct sockaddr_in v4;
  struct sockaddr_in6 v6;

  // IPV4
  if (sock_family_ == AF_INET) {
    len = sizeof(v4);
    def = reinterpret_cast<struct sockaddr*>(&v4);
  } else {  // IPV6
    len = sizeof(v6);
    def = reinterpret_cast<struct sockaddr*>(&v6);
  }

  // STEP 2:

  // Wait a while and listen... until a client accesses the socket
  int client_fd;
  while (1) {
    client_fd = accept(listen_sock_fd_,
                           def,
                           &len);
    if (client_fd < 0) {
      if ((errno == EINTR) || (errno == EAGAIN) || (errno == EWOULDBLOCK))
        continue;
      return false;
    }

    // Set the ouptut parameter accepted_fd equal to client_fd for returning
    *accepted_fd = client_fd;

    int optval = 1;
    setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR,
               &optval, sizeof(optval));

    // Obtain all required server information
    GetServerData(client_fd, sock_family_, server_dns_name,
                       client_port, server_addr);

    // Obtain all required client information
    GetClientData(def, len, client_dns_name, client_addr);

    break;
  }

  return true;
}

void GetServerData(const int& fd,
               const int& family,
               std::string* const& dns_name,
               uint16_t* const& port,
               std::string* const& addr) {
  char hname[1024];
  hname[0] = '\0';

  if (family == AF_INET) {
    // The server is using an IPv4 address.
    struct sockaddr_in srvr;
    socklen_t srvrlen = sizeof(srvr);
    char addrbuf[INET_ADDRSTRLEN];

    // Obtain sock information and stores it in srvr
    Verify333(getsockname(fd,
                reinterpret_cast<struct sockaddr*>(&srvr),
                &srvrlen) == 0);
    inet_ntop(AF_INET, &srvr.sin_addr, addrbuf, INET_ADDRSTRLEN);

    // Obtains port and addr
    *addr = addrbuf;
    *port = ntohs(srvr.sin_port);

    // Get the server's dns name, or return it's IP address as
    // a substitute if the dns lookup fails.
    Verify333(getnameinfo(reinterpret_cast<struct sockaddr*>(&srvr),
               srvrlen, hname, 1024, nullptr, 0, 0) == 0);

    *dns_name = hname;
  } else {
    // The server is using an IPv6 address.
    struct sockaddr_in6 srvr;
    socklen_t srvrlen = sizeof(srvr);
    char addrbuf[INET6_ADDRSTRLEN];

    // Obtain sock information and stores it in srvr
    Verify333(getsockname(fd, (struct sockaddr *) &srvr, &srvrlen) == 0);
    inet_ntop(AF_INET6, &srvr.sin6_addr, addrbuf, INET6_ADDRSTRLEN);

    // Obtains Port and Addr
    *addr = addrbuf;
    *port = ntohs(srvr.sin6_port);

    // Get the server's dns name, or return it's IP address as
    // a substitute if the dns lookup fails.
    Verify333(getnameinfo(reinterpret_cast<struct sockaddr*>(&srvr),
                srvrlen, hname, 1024, nullptr, 0, 0) == 0);
    *dns_name = hname;
  }
}

void GetClientData(struct sockaddr* const& addr,
                   const size_t& addrlen,
                   std::string* const& dns_name,
                   std::string* const& ret_addr) {
  char hname[1024];
  hname[0] = '\0';

  if (addr->sa_family == AF_INET) {
    // Print out the IPV4 address and port

    char astring[INET_ADDRSTRLEN];
    struct sockaddr_in* in4 = reinterpret_cast<struct sockaddr_in*>(addr);
    inet_ntop(AF_INET, &(in4->sin_addr), astring, INET_ADDRSTRLEN);
    *ret_addr = astring;

    // Get the server's dns name, or return it's IP address as
    // a substitute if the dns lookup fails.
    Verify333(getnameinfo(addr,
                addrlen, hname, 1024, nullptr, 0, 0) == 0);

    *dns_name = hname;

  } else {
    // Print out the IPV6 address and port

    char astring[INET6_ADDRSTRLEN];
    struct sockaddr_in6* in6 = reinterpret_cast<struct sockaddr_in6*>(addr);
    inet_ntop(AF_INET6, &(in6->sin6_addr), astring, INET6_ADDRSTRLEN);
    *ret_addr = astring;

    // Get the server's dns name, or return it's IP address as
    // a substitute if the dns lookup fails.
    Verify333(getnameinfo(addr,
                addrlen, hname, 1024, nullptr, 0, 0) == 0);
    *dns_name = hname;
  }
}

}  // namespace hw4
