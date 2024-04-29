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

#include <boost/algorithm/string.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <filesystem>

#include "./FileReader.h"
#include "./HttpConnection.h"
#include "./HttpRequest.h"
#include "./HttpUtils.h"
#include "./HttpServer.h"
#include "./libhw3/QueryProcessor.h"
#include "libhw1/CSE333.h"


using std::cerr;
using std::cout;
using std::endl;
using std::list;
using std::map;
using std::string;
using std::stringstream;
using std::unique_ptr;
using std::filesystem::path;
using std::to_string;
using boost::split;
using boost::to_lower;
using boost::token_compress_on;
using boost::is_any_of;

#define SUCCESS_CODE 200
#define ERROR_CODE 404


namespace hw4 {
///////////////////////////////////////////////////////////////////////////////
// Constants, internal helper functions
///////////////////////////////////////////////////////////////////////////////
static const char* kThreegleStr =
  "<html><head><title>333gle</title></head>\n"
  "<body>\n"
  "<center style=\"font-size:500%;\">\n"
  "<span style=\"position:relative;bottom:-0.33em;color:orange;\">3</span>"
    "<span style=\"color:red;\">3</span>"
    "<span style=\"color:gold;\">3</span>"
    "<span style=\"color:blue;\">g</span>"
    "<span style=\"color:green;\">l</span>"
    "<span style=\"color:red;\">e</span>\n"
  "</center>\n"
  "<p>\n"
  "<div style=\"height:20px;\"></div>\n"
  "<center>\n"
  "<form action=\"/query\" method=\"get\">\n"
  "<input type=\"text\" size=30 name=\"terms\" />\n"
  "<input type=\"submit\" value=\"Search\" />\n"
  "</form>\n"
  "</center><p>\n";

// static
const int HttpServer::kNumThreads = 100;

// This is the function that threads are dispatched into
// in order to process new client connections.
static void HttpServer_ThrFn(ThreadPool::Task* t);

// Given a request, produce a response.
static HttpResponse ProcessRequest(const HttpRequest& req,
                            const string& base_dir,
                            const list<string>& indices);

// Process a file request.
static HttpResponse ProcessFileRequest(const string& uri,
                                const string& base_dir);

// Process a query request.
static HttpResponse ProcessQueryRequest(const string& uri,
                                 const list<string>& indices);


///////////////////////////////////////////////////////////////////////////////
// HttpServer
///////////////////////////////////////////////////////////////////////////////
bool HttpServer::Run(void) {
  // Create the server listening socket.
  int listen_fd;
  cout << "  creating and binding the listening socket..." << endl;
  if (!socket_.BindAndListen(AF_INET6, &listen_fd)) {
    cerr << endl << "Couldn't bind to the listening socket." << endl;
    return false;
  }

  // Spin, accepting connections and dispatching them.  Use a
  // threadpool to dispatch connections into their own thread.
  cout << "  accepting connections..." << endl << endl;
  ThreadPool tp(kNumThreads);
  while (1) {
    HttpServerTask* hst = new HttpServerTask(HttpServer_ThrFn);
    hst->base_dir = static_file_dir_path_;
    hst->indices = &indices_;
    if (!socket_.Accept(&hst->client_fd,
                    &hst->c_addr,
                    &hst->c_port,
                    &hst->c_dns,
                    &hst->s_addr,
                    &hst->s_dns)) {
      // The accept failed for some reason, so quit out of the server.
      // (Will happen when kill command is used to shut down the server.)
      break;
    }
    // The accept succeeded; dispatch it.
    tp.Dispatch(hst);
  }
  return true;
}

static void HttpServer_ThrFn(ThreadPool::Task* t) {
  // Cast back our HttpServerTask structure with all of our new
  // client's information in it.
  unique_ptr<HttpServerTask> hst(static_cast<HttpServerTask*>(t));
  cout << "  client " << hst->c_dns << ":" << hst->c_port << " "
       << "(IP address " << hst->c_addr << ")" << " connected." << endl;

  // Read in the next request, process it, and write the response.

  // Use the HttpConnection class to read and process the next
  // request from our current client, then write out our response.  If
  // the client sends a "Connection: close\r\n" header, then shut down
  // the connection -- we're done.
  //
  // Hint: the client can make multiple requests on our single connection,
  // so we should keep the connection open between requests rather than
  // creating/destroying the same connection repeatedly.
  // STEP 1:

  // Create connection
  HttpConnection hc(hst.get()->client_fd);
  HttpRequest hr;
  while (1) {
    // Get next request from client. If it fails we close the connection
    if (!hc.GetNextRequest(&hr)) {
      break;
    }

    // Process request into a response
    HttpResponse hre = ProcessRequest(hr, hst.get()->base_dir,
                                      *hst.get()->indices);

    // Write the response and close connection if it fails
    if (!hc.WriteResponse(hre)) {
      break;
    }

    // If the request asked us to close the client connection we do
    if (hr.GetHeaderValue("connection") == "close") {
      break;
    }
  }
}

static HttpResponse ProcessRequest(const HttpRequest& req,
                            const string& base_dir,
                            const list<string>& indices) {
  // Is the user asking for a static file?
  if (req.uri().substr(0, 8) == "/static/") {
    return ProcessFileRequest(req.uri(), base_dir);
  }

  // The user must be asking for a query.
  return ProcessQueryRequest(req.uri(), indices);
}

static HttpResponse ProcessFileRequest(const string& uri,
                                const string& base_dir) {
  // The response we'll build up.
  HttpResponse ret;
  string file_name = "";

  // STEP 2:

  // 1. Use the URLParser class to figure out what file name
  //    the user is asking for. Note that we identify a request
  //    as a file request if the URI starts with '/static/'
  URLParser urlp;
  urlp.Parse(uri);
  file_name = urlp.path();
  file_name.erase(0, 8);

  // 2. Use the FileReader class to read the file into memory
  FileReader fr(base_dir, EscapeHtml(file_name));
  string filecontents;
  if (!fr.ReadFile(&filecontents)) {
    // If you couldn't find the file, return an HTTP 404 error.
    ret.set_protocol("HTTP/1.1");
    ret.set_response_code(ERROR_CODE);
    ret.set_message("Not Found");
    ret.AppendToBody("<html><body>Couldn't find file \""
                    + EscapeHtml(file_name)
                    + "\"</body></html>\n");
    return ret;
  }

  // 3. Copy the file content into the ret.body
  ret.AppendToBody(filecontents);

  // 4. Depending on the file name suffix, set the response
  //    Content-type header as appropriate, e.g.,:
  //      --> for ".html" or ".htm", set to "text/html"
  //      --> for ".jpeg" or ".jpg", set to "image/jpeg"
  //      --> for ".png", set to "image/png"
  //      etc.
  //    You should support the file types mentioned above,
  //    as well as ".txt", ".js", ".css", ".xml", ".gif",
  //    and any other extensions to get bikeapalooza
  //    to match the solution server.
  //
  // be sure to set the response code, protocol, and message
  // in the HttpResponse as well
  path p(file_name);
  string type = p.extension().string();
  if (!type.empty()) {
    // remove '.' from content type
    type.erase(0, 1);
  }

  // Finds content type of file
  if (type.compare("jpeg") == 0
     || type.compare("png") == 0
     || type.compare("gif") == 0) {
    ret.set_content_type("image/" + type);
  } else if (type.compare("jpg") == 0) {
    ret.set_content_type("image/jpeg");
  } else if (type.compare("html") == 0
           || type.compare("htm") == 0
           || type.compare("js") == 0
           || type.compare("css") == 0
           || type.compare("xml") == 0) {
    ret.set_content_type("text/" + type);
  } else if (type.compare("htm") == 0) {
    ret.set_content_type("image/html");
  } else {
    ret.set_content_type("text/plain");
  }

  // We succeeded so set all ret fields to a success state
  ret.set_response_code(SUCCESS_CODE);
  ret.set_protocol("HTTP/1.1");
  ret.set_message("OK");

  return ret;
}

static HttpResponse ProcessQueryRequest(const string& uri,
                                 const list<string>& indices) {
  // The response we're building up.
  HttpResponse ret;

  // Your job here is to figure out how to present the user with
  // the same query interface as our solution_binaries/http333d server.
  // A couple of notes:
  //

  // STEP 3:

  // 1. The 333gle logo and search box/button should be present on the site.
  string body = kThreegleStr;

  // 2. If the user had previously typed in a search query, you also need
  //    to display the search results.
  // 3. you'll want to use the URLParser to parse the uri and extract
  //    search terms from a typed-in search query.  convert them
  //    to lower case.
  URLParser urlp;
  urlp.Parse(uri);
  vector<string> args;
  string escaped_args = EscapeHtml(urlp.args()["terms"]);
  to_lower(escaped_args);
  split(args, escaped_args,
        is_any_of(" "), token_compress_on);

  // 4. Initialize and use hw3::QueryProcessor to process queries with the
  //    search indices.
  hw3::QueryProcessor qp(indices);
  vector<hw3::QueryProcessor::QueryResult> processed;

  // If a any terms were searched either say no files exist
  // that contain the space separated words in args, say only
  // one file exists that contain the words, or say
  // the number of fields that contain the words in args
  if (urlp.args().count("terms")) {
    processed = qp.ProcessQuery(args);
    body += "<p><br> ";
    if (processed.size() == 0) {
      body += "No results ";
    } else if (processed.size() == 1) {
      body += "1 result ";
    } else {
      body += (to_string(processed.size()) + " results ");
    }

    body += " found for <b>";
    body += escaped_args;
    body += "</b></p><p> </p>";

    // 5. With your results, try figuring out how to hyperlink results to file
    //    contents, like in solution_binaries/http333d. (Hint: Look into HTML
    //    tags!)
    if (processed.size() != 0) {
      body += "<ul>";
      for (auto tag : processed) {
        body += "<li><a href=\"";
        if (tag.document_name.substr(0, 4).compare("http") != 0) {
          body += "/static/";
        }
        body += tag.document_name + "\"> "
        + tag.document_name + "</a> [" + std::to_string(tag.rank) + "]"
        + "</li>";
      }
      body += "</ul>";
    }
  }

  // Append successfuly to ret and set rets fields to a success state
  ret.AppendToBody(body);
  ret.set_content_type("text/html");
  ret.set_response_code(SUCCESS_CODE);
  ret.set_protocol("HTTP/1.1");
  ret.set_message("Ok");

  return ret;
}

}  // namespace hw4
