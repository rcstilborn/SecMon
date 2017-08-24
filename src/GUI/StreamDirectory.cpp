/*
 * HTTPStreamServer.cpp
 *
 *  Created on: Aug 8, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#include "StreamDirectory.h"

#include <boost/thread/lock_guard.hpp>
#include <glog/logging.h>
#include <cstddef>
#include <exception>
#include <iostream>
#include <utility>
#include <string>

StreamDirectory::StreamDirectory(boost::asio::io_service& io_service)
    : io_service_(io_service),
      stream_id(0),
      stream_list_(),
      stream_list_mtx_() {
  //    std::cout << "StreamDirectory::StreamDirectory() - constructing" << std::endl;
}

StreamDirectory::~StreamDirectory() {
  boost::lock_guard<boost::mutex> guard(stream_list_mtx_);
  stream_list_.clear();
}

void StreamDirectory::addStream(SceneInterface::Stream& stream) {
  boost::lock_guard<boost::mutex> guard(stream_list_mtx_);
  boost::shared_ptr<Stream> stream_ptr(new Stream(stream.image_ready, ++stream_id));
  DLOG(INFO)<< "StreamDirectory::addStream() - " << stream.name << " mapped to " << stream_id << ".mjpeg";
  stream_list_.insert(std::pair<const int, boost::shared_ptr<Stream>>(stream_id, stream_ptr));
}

//void StreamDirectory::addStream(const std::string& name,
//                                boost::signals2::signal<void (boost::shared_ptr<std::vector<unsigned char>>)> signal){
//    boost::lock_guard<boost::mutex> guard(stream_list_mtx_);
//    boost::shared_ptr<Stream> stream_ptr(new Stream(signal, ++stream_id));
//    std::cout << "StreamDirectory::addStream() - " << name << " mapped to " << stream_id << ".mjpeg" << std::endl;
//    stream_list_.insert(std::pair<const int,boost::shared_ptr<Stream>>(stream_id,stream_ptr));
//}

bool StreamDirectory::handleValidStream(const std::string& streamRequest, http::connection_ptr conn) {
  // Strip file name down to int
  std::size_t last_dot_pos = streamRequest.find_last_of(".");
  if (last_dot_pos == std::string::npos) {
    LOG(WARNING)<< "StreamDirectory::handleValidStream() - No extension found in requested stream " << streamRequest;
    return false;
  }
  std::string filename = streamRequest.substr(1, last_dot_pos + 1);
  int stream_id;
  try {
    stream_id = std::stoi(filename);
  } catch (std::exception& e) {
    LOG(WARNING)<< "StreamDirectory::handleValidStream() - Could not convert filename to integer " <<
                   filename << ". " << e.what();
    return false;
  }

  //    std::cout << "StreamDirectory::handle_stream() - got request for stream id " << stream_id << std::endl;
  auto it = stream_list_.find(stream_id);
  if (it != stream_list_.end()) {
    //        std::cout << "StreamDirectory::handle_stream() - found the correct stream" << std::endl;
    it->second->registerNewConnection(conn);
    return true;
  } else {
    LOG(WARNING)<< "StreamDirectory::handleValidStream() - Could not find stream with id " << stream_id;
    return false;
  }
}

