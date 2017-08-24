/*
 * RequestHandler.h
 *
 *  Created on: Aug 8, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#ifndef GUI_REQUESTHANDLER_H_
#define GUI_REQUESTHANDLER_H_

#include <boost/core/noncopyable.hpp>
#include <cstdbool>
#include <string>

#include "Connection.h"
#include "StreamDirectory.h"

class StreamDirectory;

namespace http {

struct Reply;
struct Request;

/// The common handler for all incoming requests.
class RequestHandler : private boost::noncopyable {
 public:
  /// Construct with a directory containing files to be served.
  explicit RequestHandler(const std::string& doc_root, StreamDirectory& streamDirectory);

  /// Handle a request and produce a reply.
  bool handle_request(const Request& req, Reply& rep, connection_ptr conn);

 private:
  /// The directory containing the files to be served.
  std::string doc_root_;

  // StreamDirectory
  StreamDirectory& streamDirectory_;

  /// Perform URL-decoding on a string. Returns false if the encoding was
  /// invalid.
  static bool url_decode(const std::string& in, std::string& out);
};

}  // namespace http

#endif // GUI_REQUESTHANDLER_H_
