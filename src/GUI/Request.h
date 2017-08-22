/*
 * Request.h
 *
 *  Created on: Aug 8, 2015
 *      Author: richard
 */

#ifndef GUI_REQUEST_H_
#define GUI_REQUEST_H_

#include <string>
#include <vector>

#include "Header.h"

namespace http {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
/// A request received from a client.
struct Request
{
  std::string method;
  std::string uri;
  int http_version_major;
  int http_version_minor;
  std::vector<Header> headers;
};
#pragma GCC diagnostic pop

} // namespace http

#endif /* GUI_REQUEST_H_ */
