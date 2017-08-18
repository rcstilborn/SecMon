/*
 * HTTPServer.h
 *
 *  Created on: Aug 8, 2015
 *      Author: richard
 */

#ifndef GUI_HTTP_SERVER_H_
#define GUI_HTTP_SERVER_H_

#include <boost/asio/ip/tcp.hpp>
#include <boost/core/noncopyable.hpp>
#include <boost/system/error_code.hpp>
#include <string>

#include "Connection.h"
#include "RequestHandler.h"
#include "StreamDirectory.h"

namespace http {

/// The top-level class of the HTTP server.
class HTTP_Server
  : private boost::noncopyable
{
public:
  /// Construct the server to listen on the specified TCP address and port, and
  /// serve up files from the given directory.
  explicit HTTP_Server(boost::asio::io_service& io_service, StreamDirectory& streamDirectory);

  void start();

private:
  /// Initiate an asynchronous accept operation.
  void start_accept();

  /// Handle completion of an asynchronous accept operation.
  void handle_accept(const boost::system::error_code& e);

  /// Handle a request to stop the server.
  void handle_stop();

  /// The io_service used to perform asynchronous operations.
  boost::asio::io_service& io_service_;

  /// Acceptor used to listen for incoming connections.
  boost::asio::ip::tcp::acceptor acceptor_;

  /// The next connection to be accepted.
  connection_ptr new_connection_;

  /// The handler for all incoming requests.
  RequestHandler request_handler_;
};

} // namespace http

#endif /* GUI_HTTP_SERVER_H_ */
