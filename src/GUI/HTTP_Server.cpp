/*
 * HTTPServer.cpp
 *
 *  Created on: Aug 8, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#include "HTTP_Server.h"

#include <boost/asio/basic_socket_acceptor.hpp>
#include <boost/asio/detail/socket_option.hpp>
#include <boost/asio/ip/basic_endpoint.hpp>
#include <boost/asio/ip/basic_resolver.hpp>
#include <boost/asio/ip/basic_resolver_iterator.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/socket_base.hpp>
#include <boost/bind/bind.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <gflags/gflags.h>
#include <glog/logging.h>

#include <cstdbool>
#include <iostream>
#include <string>

DEFINE_int32(port, 12345, "Port for webserver");
DEFINE_string(ip_address, "", "IP Address of web server");
DEFINE_string(doc_root, "public", "Document root for web server");
// This is needed because of compiler warning for release builds
#ifdef DEBUG
static bool ValidateIPaddress(const char* flagname, const std::string& value) {
  if (value.find(".") != std::string::npos)   // value is ok
  return true;
  std::cerr << "Invalid value for --" << flagname << ": " << value << std::endl;
  return false;
}
DEFINE_validator(ip_address, &ValidateIPaddress);
#endif // DEBUG

namespace http {

HTTP_Server::HTTP_Server(boost::asio::io_service& io_service, StreamDirectory& streamDirectory)
    : io_service_(io_service),
      acceptor_(io_service_),
      new_connection_(),
      request_handler_(FLAGS_doc_root, streamDirectory) {
  DLOG(INFO) << "HTTP_Server::HTTP_Server() - constructing with address=" <<
                FLAGS_ip_address << " and port=" << FLAGS_port <<
  " and doc_root=" << FLAGS_doc_root;
  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  boost::asio::ip::tcp::resolver resolver(io_service_);
  boost::asio::ip::tcp::resolver::query query(FLAGS_ip_address, std::to_string(FLAGS_port));
  boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();
}

void HTTP_Server::start() {
  start_accept();
  DLOG(INFO) << "HTTP_Server accepting connections";
}

void HTTP_Server::start_accept() {
  new_connection_.reset(new Connection(io_service_, request_handler_));
  acceptor_.async_accept(new_connection_->socket(),
                         boost::bind(&HTTP_Server::handle_accept, this, boost::asio::placeholders::error));
}

void HTTP_Server::handle_accept(const boost::system::error_code& e) {
  if (!e) {
    new_connection_->start();
  }
  start_accept();
}

void HTTP_Server::handle_stop() {
  //  io_service_.stop();
}

}  // namespace http
