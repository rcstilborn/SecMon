/*
 * Stream.h
 *
 *  Created on: Aug 8, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#ifndef GUI_STREAM_H_
#define GUI_STREAM_H_

#include <boost/core/noncopyable.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/thread/pthread/mutex.hpp>
#include <vector>
#include <memory>

#include "Connection.h"
#include "StreamConnection.h"

class Stream : private boost::noncopyable {
 public:
  Stream(boost::signals2::signal<void(std::shared_ptr<std::vector<unsigned char>>)>& signal, int stream_id);
  virtual ~Stream();
  void registerNewConnection(http::connection_ptr conn);
  void deregisterConnection(http::stream_connection_ptr conn);
  boost::signals2::connection connectToSignal(
      const boost::signals2::signal<void(std::shared_ptr<std::vector<unsigned char>>)>::slot_type &subscriber);
  int getStreamId() const;

 private:
  boost::signals2::signal<void(std::shared_ptr<std::vector<unsigned char>>)>& signal_;
  int stream_id_;
  std::vector<http::stream_connection_ptr> connection_list_;
  boost::mutex connection_list_mtx_;
};

#endif // GUI_STREAM_H_
