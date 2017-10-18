/*
 * ScenePublisher.cpp
 *
 *  Created on: Aug 13, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#include "ScenePublisher.h"

#include <boost/thread/lock_guard.hpp>

#include <string>
#include <memory>
#include <iostream>
#include "Frame.h"
#include "GUI/GUI_Interface.h"

ScenePublisher::ScenePublisher(const std::string& display_name, const std::string& description, GUI_Interface& gui)
    : display_name_(display_name),
      description_(description),
      gui_(gui),
      streams_list_mtx_(),
      streams_(),
      performance_monitor_() {
  add_stream("main");
  add_stream("overlay");
  add_stream("foreground");
}

ScenePublisher::~ScenePublisher() {
}

const std::string& ScenePublisher::get_description() const {
  return description_;
}

const std::string& ScenePublisher::get_display_name() const {
  return display_name_;
}

void ScenePublisher::add_stream(const std::string& name) {
  boost::lock_guard<boost::mutex> guard(streams_list_mtx_);
  std::shared_ptr<ScenePublisher::Stream> stream_ptr(new ScenePublisher::Stream(name));
  streams_.push_back(stream_ptr);
  // TODO(richard): Should pass the shared_ptr
  gui_.registerNewStream(*stream_ptr);
  return;
}
// boost::signals2::connection ScenePublisher::connect(const
// image_ready_signal::slot_type &subscriber)
//{
//    return image_ready.connect(subscriber);
//}
//
// void ScenePublisher::trigger(image_ptr image) {
//    image_ready(image);
//}

void ScenePublisher::process_next_frame(std::shared_ptr<Frame>& frame) {
  frame->set_display_name(display_name_);
  frame->set_display_stats(performance_monitor_.get_performance_string());
  for (auto stream : streams_)
    if (stream->image_ready.num_slots() > 0) {
      try {
        stream->image_ready(frame->get_image_as_jpg(stream->name));
      } catch (std::exception& e) {
      }
    }
}
