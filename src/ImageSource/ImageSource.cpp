/*
 * ImageSource.cpp
 *
 *  Created on: Oct 23, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#include "ImageSource.h"

#include <boost/asio/detail/wrapped_handler.hpp>
#include <boost/bind/arg.hpp>
#include <boost/bind/bind.hpp>
#include <boost/bind/placeholders.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <opencv2/core/mat.inl.hpp>
#include <glog/logging.h>
#include <exception>
#include <iostream>
#include <string>
#include <chrono>

#include "../Frame.h"
#include "../FrameSequence.h"

ImageSource::ImageSource(const std::string& name, const std::string& url, boost::asio::io_service& io_service,
                         SceneInterface& sceneIf, FrameSequence& frameSequence, boost::function<void(const int)> next,
                         const int fps)
    : name_(name),
      interval_(1000 / fps),
      camera_(url),
      scene_interface_(sceneIf),
      frame_sequence_(frameSequence),
      next_(next),
      timer_(io_service, std::chrono::milliseconds(interval_)),
      strand_(io_service),
      shutting_down_(false) {
  DLOG(INFO)<< "ImageSource(" << name << ") - constructed";
  frameSequence.setSize(camera_.get_width(), camera_.get_height());
  //    gui.registerNewScene(sceneIf);
  // Reset and start the timer
  start_timer();
}

void ImageSource::start_timer() {
  if (shutting_down_)
    return;
  // Reset the timer from now (restarting the camera takes time)
  this->timer_.expires_from_now(std::chrono::milliseconds(this->interval_));
  this->timer_.async_wait(strand_.wrap(boost::bind(&ImageSource::get_next_frame, this, _1)));
}

void ImageSource::restart_timer() {
  if (shutting_down_)
    return;
  // Reset the timer
  this->timer_.expires_at(this->timer_.expires_at() + std::chrono::milliseconds(this->interval_));
  this->timer_.async_wait(strand_.wrap(boost::bind(&ImageSource::get_next_frame, this, _1)));
}

void ImageSource::get_next_frame(const boost::system::error_code& ec) {
  // Check for errors.
  if (ec) {
    return;
  }

  // Check for paused
  if (is_paused_) {
    restart_timer();
    return;
  }
  boost::shared_ptr<Frame> frame = frame_sequence_.get_new_frame();
  try {
    if (!this->camera_.get_next_frame(frame->get_original_image(), frame->get_original_image())) {
      LOG(WARNING)<< "ImageSource::getNextFrame() \"" << name_ <<
          "\" - camera returned false so restarting";
      this->camera_.restart();

      // Try the camera again
      if (!this->camera_.get_next_frame(frame->get_original_image(), frame->get_original_image())) {
        // If it fails again then abort - delete the last frame and don't restart the timer
        LOG(WARNING) << "ImageSource::getNextFrame() \"" << name_ <<
            "\" - Camera failed to read after re-start.  Abort.";
        frame_sequence_.delete_frame(frame->get_frame_id());
        return;
      }
      // Reset the timer from now (restarting the camera takes time)
      start_timer();
    } else {
      // Reset the timer
      restart_timer();
    }
  } catch(std::exception& e) {
    LOG(WARNING) << "ImageSource::getNextFrame() \"" << name_ << "\" - caught exception from camera " << e.what();
    frame_sequence_.delete_frame(frame->get_frame_id());
    return;
  }

  // Check the frame isn't empty
  if (frame->get_original_image().empty()) {
    LOG(WARNING)<< "Empty frame!!!";
    frame_sequence_.delete_frame(frame->get_frame_id());
    return;
  }

  next_(frame->get_frame_id());
}

ImageSource::~ImageSource() {
  timer_.cancel();
}

Camera& ImageSource::get_camera() {
  return this->camera_;
}

const std::string& ImageSource::get_name() const {
  return this->name_;
}

void ImageSource::shutdown() {
  shutting_down_ = true;
  timer_.cancel();
}

void ImageSource::toggle_pause() {
  is_paused_ = !is_paused_;
}

void ImageSource::set_frames_per_second(const int fps) {
  interval_ = 1000 / fps;
}
