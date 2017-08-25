/*
 * FrameSequence.cpp
 *
 *  Created on: Jul 27, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#include "FrameSequence.h"

#include <boost/thread/lock_guard.hpp>
#include <boost/thread/pthread/mutex.hpp>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>
#include <memory>

#include "Frame.h"

FrameSequence::FrameSequence()
    : current_frame_id_(0),
      frames_(),
      frame_list_mtx_() {
}

FrameSequence::~FrameSequence() {
  boost::lock_guard<boost::mutex> guard(frame_list_mtx_);
  while (!this->frames_.empty()) {
    this->frames_.erase(this->frames_.begin());
  }
}

void FrameSequence::setSize(const int width, const int height) {
  width_ = width;
  height_ = height;
}

std::shared_ptr<Frame> FrameSequence::get_new_frame() {
  const int frame_id = current_frame_id_++;
  std::shared_ptr<Frame> newFrame(new Frame(frame_id, width_, height_));
  {
    boost::lock_guard<boost::mutex> guard(frame_list_mtx_);
    this->frames_.insert(std::pair<const int, std::shared_ptr<Frame>>(frame_id, newFrame));
  }
  if (frames_.size() > frame_maximum) {
    delete_earliest_frame();
  }
  return newFrame;
}

void FrameSequence::delete_frame(const int frame_id) {
  boost::lock_guard<boost::mutex> guard(frame_list_mtx_);
  auto it = frames_.find(frame_id);
  if (it == frames_.end())
    return;
  frames_.erase(it);
}

std::shared_ptr<Frame> FrameSequence::get_frame(const int frame_id) {
  boost::lock_guard<boost::mutex> guard(frame_list_mtx_);
  auto it = frames_.find(frame_id);
  if (it == frames_.end())
    throw std::invalid_argument("No such frame");
  return it->second;
}

std::vector<std::shared_ptr<Frame>> FrameSequence::get_frame_list(const int frame_id, const int listSize) {
  boost::lock_guard<boost::mutex> guard(frame_list_mtx_);
  std::vector<std::shared_ptr<Frame>> frameList;
  auto it = frames_.find(frame_id);
  if (it == frames_.end())
    throw std::invalid_argument("No such frame");
  frameList.push_back(it->second);
  for (int i = 0; i < listSize; i++) {
    if (--it == frames_.begin())
      break;
    frameList.push_back(it->second);
  }
  return frameList;
}

void FrameSequence::delete_earliest_frame() {
  boost::lock_guard<boost::mutex> guard(frame_list_mtx_);
  if (!frames_.empty())
    frames_.erase(frames_.begin());
}
