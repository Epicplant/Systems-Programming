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

#include <stdint.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <map>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>

#include "./HttpRequest.h"
#include "./HttpUtils.h"
#include "./HttpConnection.h"

using std::map;
using std::string;
using std::vector;
using std::string;
using boost::split;
using boost::to_lower;
using boost::token_compress_on;
using boost::is_any_of;
using boost::trim;

namespace hw4 {

static const char* kHeaderEnd = "\r\n\r\n";
static const int kHeaderEndLen = 4;
#define BUF_SIZE 1024


bool HttpConnection::GetNextRequest(HttpRequest* const request) {
  // Use WrappedRead from HttpUtils.cc to read bytes from the files into
  // private buffer_ variable. Keep reading until:
  // 1. The connection drops
  // 2. You see a "\r\n\r\n" indicating the end of the request header.
  //
  // Hint: Try and read in a large amount of bytes each time you call
  // WrappedRead.
  //
  // After reading complete request header, use ParseRequest() to parse into
  // an HttpRequest and save to the output parameter request.
  //
  // Important note: Clients may send back-to-back requests on the same socket.
  // This means WrappedRead may also end up reading more than one request.
  // Make sure to save anything you read after "\r\n\r\n" in buffer_ for the
  // next time the caller invokes GetNextRequest()!

  // STEP 1:
  ssize_t readlen;
  size_t request_end;
  char buf[BUF_SIZE+1];

  while (1) {
    // May have eaten up a header, check to see if \r\n\r\n exists.
    // If so leave lop and process the header with the buf contaning the
    // data in that string
    request_end = buffer_.find(kHeaderEnd);
    if (request_end != std::string::npos) {
      break;
    }

    // Read 1024 bytes from fd
    readlen = WrappedRead(fd_, reinterpret_cast<unsigned char*>(buf), BUF_SIZE);
    if (readlen <= 0) {
      return false;
    }

    // Append read data to buffer_ for later use
    buffer_ += string(buf, readlen);
  }

  // Parse that Request and returning it through the output parameter
  *request = ParseRequest(buffer_.substr(0, request_end));

  // Dumping buffer_ up until requestend + 4 (getting rid of found request)
  if (buffer_.size() > request_end + kHeaderEndLen) {
    buffer_.erase(0, request_end + kHeaderEndLen);
  } else {
    buffer_ = "";
  }

  return true;  // You may want to change this.
}

bool HttpConnection::WriteResponse(const HttpResponse& response) const {
  string str = response.GenerateResponseString();
  int res = WrappedWrite(fd_,
                         reinterpret_cast<const unsigned char*>(str.c_str()),
                         str.length());
  if (res != static_cast<int>(str.length()))
    return false;
  return true;
}

HttpRequest HttpConnection::ParseRequest(const string& request) const {
  HttpRequest req("/");  // by default, get "/".

  // Plan for STEP 2:
  // 1. Split the request into different lines (split on "\r\n").
  // 2. Extract the URI from the first line and store it in req.URI.
  // 3. For the rest of the lines in the request, track the header name and
  //    value and store them in req.headers_ (e.g. HttpRequest::AddHeader).
  //
  // Hint: Take a look at HttpRequest.h for details about the HTTP header
  // format that you need to parse.
  //
  // You'll probably want to look up boost functions for:
  // - Splitting a string into lines on a "\r\n" delimiter
  // - Trimming whitespace from the end of a string
  // - Converting a string to lowercase.
  //
  // Note: If a header is malformed, skip that line.

  // STEP 2:

  // Break the request into lines
  vector<string> strs;
  string copy = request;
  split(strs, copy, is_any_of("\r\n"), token_compress_on);

  // Obtain every space separated word from the first line
  // and place them into an array
  string lineone = strs.front();
  vector<string> line_one_data;
  trim(lineone);
  split(line_one_data, lineone, is_any_of(" "),
        token_compress_on);

  // Check if request had incorrect formatting
  if (line_one_data.size() != 3) {
    return req;
  }

  // Obtain the URI from the middle of the first line
  line_one_data.pop_back();
  string lower_case_ui = line_one_data.back();
  req.set_uri(lower_case_ui);

  // Remove first line from vector of request lines
  // as we are done with it
  strs.erase(strs.begin(), strs.begin()+1);

  // Iterate through header line
  for (string header_line : strs) {
    // Receive line
    vector<string> data;
    trim(copy);
    split(data, header_line, is_any_of(":"),
          token_compress_on);

    // Obtain the lines headerval
    string val = data.back();
    trim(val);
    to_lower(val);

    data.pop_back();

    // Obtain the lines headername
    string name = data.back();
    to_lower(name);

    // Add header to req
    req.AddHeader(name, val);
  }


  return req;
}

}  // namespace hw4
