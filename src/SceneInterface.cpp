/*
 * SceneInterface.cpp
 *
 *  Created on: Aug 13, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#include "SceneInterface.h"

#include <boost/thread/lock_guard.hpp>

#include <string>
#include <memory>

#include "Frame.h"
#include "FrameSequence.h"
#include "GUI/GUI_Interface.h"

SceneInterface::SceneInterface(const std::string& display_name, const std::string& description, GUI_Interface& gui,
                               FrameSequence& frame_sequence)
    : display_name_(display_name),
      description_(description),
      gui_(gui),
      frame_sequence_(frame_sequence),
      streams_list_mtx_(),
      streams_() {
  add_stream("main");
}

SceneInterface::~SceneInterface() {
}

const std::string& SceneInterface::get_description() const {
  return description_;
}

const std::string& SceneInterface::get_display_name() const {
  return display_name_;
}

void SceneInterface::add_stream(const std::string& name) {
  boost::lock_guard<boost::mutex> guard(streams_list_mtx_);
  std::shared_ptr<SceneInterface::Stream> stream_ptr(new SceneInterface::Stream(name));
  streams_.push_back(stream_ptr);
  // TODO(richard): Should pass the shared_ptr
  gui_.registerNewStream(*stream_ptr);
  return;
}
// boost::signals2::connection SceneInterface::connect(const
// image_ready_signal::slot_type &subscriber)
//{
//    return image_ready.connect(subscriber);
//}
//
// void SceneInterface::trigger(image_ptr image) {
//    image_ready(image);
//}

void SceneInterface::publish(const int frameid) {
  for (auto stream : streams_)
    if (stream->image_ready.num_slots() > 0) {
      try {
        stream->image_ready(frame_sequence_.get_frame(frameid)->get_image_as_jpg(stream->name));
      } catch (std::exception& e) {
      }
    }
}
