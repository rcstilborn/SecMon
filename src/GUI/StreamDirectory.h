/*
 * HTTPStreamServer.h
 *
 *  Created on: Aug 8, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#ifndef GUI_STREAMDIRECTORY_H_
#define GUI_STREAMDIRECTORY_H_

#include <boost/core/noncopyable.hpp>
#include <boost/thread/pthread/mutex.hpp>
#include <cstdbool>
#include <map>
#include <string>
#include <memory>

#include "../ScenePublisher.h"
#include "Connection.h"
#include "Stream.h"

class Stream;

class StreamDirectory : private boost::noncopyable {
 public:
  explicit StreamDirectory(boost::asio::io_service& io_service);
  virtual ~StreamDirectory();
  bool handleValidStream(const std::string& streamRequest, http::connection_ptr conn);
  void addStream(ScenePublisher::Stream& stream);
//    void addStream(const std::string& name,
//                   boost::signals2::signal<void (std::shared_ptr<std::vector<unsigned char>>)> signal);

 private:
  /// The io_service used to perform asynchronous operations.
  boost::asio::io_service& io_service_;
  int stream_id;
  std::map<const int, std::shared_ptr<Stream>> stream_list_;
  boost::mutex stream_list_mtx_;
};

#endif // GUI_STREAMDIRECTORY_H_
