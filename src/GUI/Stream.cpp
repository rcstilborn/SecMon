/*
 * Stream.cpp
 *
 *  Created on: Aug 8, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#include "Stream.h"

#include <boost/thread/lock_guard.hpp>
#include <glog/logging.h>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>

Stream::Stream(ScenePublisher::image_ready_signal& signal, int stream_id)
    : signal_(signal),
      stream_id_(stream_id),
      connection_list_(),
      connection_list_mtx_() {
}

Stream::~Stream() {
  connection_list_.clear();
}

void Stream::registerNewConnection(http::connection_ptr conn) {
  // Create a new StreamConnection from the old one
  http::stream_connection_ptr str_conn(new http::StreamConnection(conn, *this));

  // Add it to our list
  {
    boost::lock_guard<boost::mutex> guard(connection_list_mtx_);
    connection_list_.push_back(str_conn);
  }

  // Tell it to start
  str_conn->start();

  //    if (stream_id_ == 3) {
  //
  //        cv::Mat image(cv::imread("/home/richard/Dev/OpenCV/opencv-2.4.8/samples/c/lena.jpg"));
  //        std::shared_ptr<std::vector<unsigned char>> lastFrame(new std::vector<unsigned char>);
  //        if(!cv::imencode(".jpg",image,*lastFrame))
  //            std::cerr << "Stream::Stream() - Problem converting to jpeg" << std::endl;
  //        str_conn->sendFrame(lastFrame);
  //        str_conn->sendFrame(lastFrame);
  //        // Create a test image, convert to jpg and send to browser
  //        //    std::ifstream is("/home/richard/Dev/OpenCV/opencv-2.4.8/samples/c/lena.jpg",
  //                               std::ios::in | std::ios::binary);
  //        //    std::istream_iterator<unsigned char> start(is), end;
  //        //    std::shared_ptr<std::vector<unsigned char>> frame(new std::vector<unsigned char>(start, end));
  //        ////    std::shared_ptr<Image> frame(new Image(start, end));
  //        //    std::cout << "Sending image of size... " << frame->size() << std::endl;
  //        //    str_conn->sendFrame(frame);
  //
  //        //    std::shared_ptr<std::vector<unsigned char>> frame2(frame);
  //        //    str_conn->sendFrame(frame2);
  //    } else if (stream_id_ == 4 ) {
  //        // Create a test image, convert to jpg and send to browser
  //        cv::Mat image2(cv::Size(640, 480), CV_8UC3,
  //                       cv::Scalar(std::rand() % 255, std::rand() % 255, std::rand() % 255));
  //        std::shared_ptr<std::vector<unsigned char>> lastFrame2(new std::vector<unsigned char>);
  //        if(!cv::imencode(".jpg",image2,*lastFrame2))
  //            std::cerr << "Stream::Stream() - Problem converting to jpeg" << std::endl;
  //        str_conn->sendFrame(lastFrame2);
  //
  //        cv::Mat image3(cv::Size(640, 480), CV_8UC3,
  //                       cv::Scalar(std::rand() % 255, std::rand() % 255, std::rand() % 255));
  //        std::shared_ptr<std::vector<unsigned char>> lastFrame3(new std::vector<unsigned char>);
  //        if(!cv::imencode(".jpg",image3,*lastFrame3))
  //            std::cerr << "Stream::Stream() - Problem converting to jpeg" << std::endl;
  //        str_conn->sendFrame(lastFrame3);
  //    }
}

boost::signals2::connection Stream::connectToSignal(const ScenePublisher::image_ready_signal::slot_type &subscriber) {
  return signal_.connect(subscriber);
}

void Stream::deregisterConnection(http::stream_connection_ptr conn) {
  auto it = std::find(connection_list_.begin(), connection_list_.end(), conn);
  if (it != connection_list_.end()) {
    {
      boost::lock_guard<boost::mutex> guard(connection_list_mtx_);
      connection_list_.erase(it);
    }
    DLOG(INFO)<< "Stream::deregisterConnection - deregistering " << connection_list_.size();
  }
}

int Stream::getStreamId() const {
  return stream_id_;
}
