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

#include <opencv2/core/mat.inl.hpp>
#include <glog/logging.h>
#include <exception>
#include <iostream>
#include <string>
#include <memory>

#include "../Frame.h"
#include "../Component.h"

ImageSource::ImageSource(const std::string& url)
    : camera_(url) ,
      width_(camera_.get_width()),
      height_(camera_.get_height()) {
  DLOG(INFO)<< "ImageSource(" << url << ") - constructed";
}

void ImageSource::process_next_frame(std::shared_ptr<Frame>& frame) {
  // Create a new frame and set the pointer to it
  frame = std::shared_ptr<Frame>(new Frame(width_, height_));

  try {
    if (!camera_.get_next_frame(frame->get_original_image(), frame->get_overlay_image())) {
      LOG(WARNING)<< "ImageSource::getNextFrame() - camera returned false so restarting";
      this->camera_.restart();

      // Try the camera again
      if (!this->camera_.get_next_frame(frame->get_original_image(), frame->get_original_image())) {
        // If it fails again then abort - delete the last frame and throw
        LOG(WARNING) << "ImageSource::getNextFrame() - Camera failed to read after re-start.  Abort.";
        throw "Camera failed to read after re-start";
      }
    } else {
    }
  } catch(std::exception& e) {
    LOG(WARNING) << "ImageSource::getNextFrame()  - caught exception from camera " << e.what();
    throw e;
  }

  // Check the frame isn't empty
  if (frame->get_original_image().empty()) LOG(WARNING)<< "Empty frame!!!";
}

ImageSource::~ImageSource() {
}
