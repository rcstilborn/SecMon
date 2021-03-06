/*
 * Request.h
 *
 *  Created on: Aug 8, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#ifndef GUI_REQUEST_H_
#define GUI_REQUEST_H_

#include <string>
#include <vector>

#include "Header.h"

namespace http {
// I think this came from the Boost::asio demo code.  It will all get refactored away when we move to libsourcey
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
/// A request received from a client.
struct Request {
  std::string method;
  std::string uri;
  int http_version_major;
  int http_version_minor;
  std::vector<Header> headers;
};
#pragma GCC diagnostic pop

}  // namespace http

#endif // GUI_REQUEST_H_
