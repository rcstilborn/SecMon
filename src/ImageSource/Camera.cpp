/*
 * Camera.cpp
 *
 *  Created on: Jul 27, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#include "Camera.h"

#include <boost/date_time/posix_time/time_formatters.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/mat.inl.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio/videoio_c.h>
#include <glog/logging.h>

#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <chrono>
#include <ctime>
#include <memory>

#include "../Frame.h"

Camera::Camera(const std::string& source)
    : source_(source),
      camera_() {

  // Connect to the camera
  DLOG(INFO)<< "Opening camera at " << source_;

  if (source.substr(0, 4) == "http")
  real_camera_ = true;
  else
  real_camera_ = false;

  auto start_time = std::chrono::high_resolution_clock::now();

  this->camera_.open(source_);
  auto end_time = std::chrono::high_resolution_clock::now();
  int dur = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

  if (!this->camera_.isOpened()) {
    LOG(ERROR) << "Could not open camera at: " << source_;
    throw std::runtime_error("Could not open camera at: " + source_);
  }

  cv::Mat img;
  if (!this->camera_.read(img) || img.empty()) {
    LOG(ERROR) << "Could not get first image from camera at: " << source_;
    throw std::runtime_error("Could not get first image from camera at: " + source_);
  }

  fps_ = this->camera_.get(CV_CAP_PROP_FPS);
  width_ = img.cols;  //(unsigned int) this->camera_.get(CV_CAP_PROP_FRAME_WIDTH);
  height_ = img.rows;//(unsigned int) this->camera_.get(CV_CAP_PROP_FRAME_HEIGHT);

  DLOG(INFO) << "Connected to camera in " << dur <<
                "ms. With size: [" << width_ << "x" << height_ << "]  FPS: " << this->fps_;
}

Camera::~Camera() {
}

bool Camera::get_next_frame(std::shared_ptr<Frame>& frame) {
  cv::Mat& img = frame->get_original_image();

  if (camera_.read(img)) {
    if (real_camera_) {
      //Couldn't get this to work!
      //std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
      //std::time_t now_c = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
      //const std::string now = std::put_time(std::localtime(&now_c), "%c");
//      const std::string now = boost::posix_time::to_simple_string(boost::posix_time::second_clock::local_time());
//      frame.set_display_time(now);
      frame->set_display_time(boost::posix_time::to_simple_string(boost::posix_time::second_clock::local_time()));
//      cv::putText(overlay, now, cvPoint(40, 25), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(255, 255, 255), 2);
    } else {
      frame->set_display_time(std::to_string((unsigned int) this->camera_.get(CV_CAP_PROP_POS_FRAMES)));
//      std::string frame_number = std::to_string((unsigned int) this->camera_.get(CV_CAP_PROP_POS_FRAMES));
//      cv::putText(overlay, frame_number, cvPoint(left_margin_, top_margin_), cv::FONT_HERSHEY_SIMPLEX, text_size_,
//                  cv::Scalar(255, 255, 255), 2);
    }
    return true;
  }
  return false;
}

int Camera::get_frames_per_second() const {
  return this->fps_;
}

const std::string& Camera::get_source_name() const {
  return this->source_;
}

unsigned int Camera::get_width() const {
  return this->width_;
}

unsigned int Camera::get_height() const {
  return this->height_;
}

void Camera::restart() {
  this->camera_.release();
  auto start_time = std::chrono::high_resolution_clock::now();
  this->camera_.open(source_);
  auto end_time = std::chrono::high_resolution_clock::now();
  int dur = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

  if (!this->camera_.isOpened()) {
    LOG(ERROR) << "Could not re-open camera at: " << source_;
    throw std::runtime_error("Could not re-open camera at: " + source_);
  }
  if (real_camera_)
    DLOG(INFO) << "Re-connected to camera at " << source_ << " in " << dur << "s";
}
