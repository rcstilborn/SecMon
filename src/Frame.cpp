/*
 * Frame.cpp
 *
 *  Created on: Jul 27, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#include "Frame.h"

#include <boost/date_time/microsec_time_clock.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <opencv2/core/cvdef.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/mat.inl.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

Frame::Frame(const int frame_id, const int width, const int height)
    : frame_id_(frame_id),
      frame_size_(width, height),
      start_time_(),
      regions_of_interest_() {
  image_names_[0] = "original";
  image_names_[1] = "overlay";
  images_[1] = cv::Mat::zeros(frame_size_, CV_8UC3);
}

Frame::~Frame() {
}

cv::Mat& Frame::get_original_image() {
  return images_[0];
}

cv::Mat& Frame::get_overlay_image() {
  return images_[1];
}

cv::Mat& Frame::get_new_or_existing_image(const std::string& name) {
  for (int i = 0; i < kMaxImagesPerFrame; i++)
    if (image_names_[i] == name)
      return images_[i];
  return get_new_image(name);
}

cv::Mat& Frame::get_new_image(const std::string& name) {
  for (int i = 0; i < kMaxImagesPerFrame; i++)
    if (image_names_[i] == name) {
      throw std::invalid_argument("Image already registered with name " + name);
    } else if (image_names_[i].empty()) {
      image_names_[i] = name;
      return images_[i];
    }
  throw std::invalid_argument("No available images");
}

cv::Mat& Frame::get_new_blank_image(const std::string& name) {
  for (int i = 0; i < kMaxImagesPerFrame; i++)
    if (image_names_[i] == name) {
      throw std::invalid_argument("Image already registered with name " + name);
    } else if (image_names_[i].empty()) {
      image_names_[i] = name;
      images_[i].zeros(frame_size_, CV_8UC3);
      return images_[i];
    }
  throw std::invalid_argument("No available images");
}

cv::Mat& Frame::get_image(const std::string& name) {
  for (int i = 0; i < kMaxImagesPerFrame; i++)
    if (image_names_[i] == name)
      return images_[i];
  throw std::invalid_argument("No image registered with name " + name);
}

SceneInterface::image_ptr Frame::get_image_as_jpg(const std::string& name) {
  // TODO(richard): make this hack more elegant
  if (name == "main")
    return get_original_with_overlay_image_as_jpg();
  cv::Mat& image = get_image(name);
  if (!image.empty()) {
    SceneInterface::image_ptr buffer(new std::vector<unsigned char>());
    cv::imencode(".jpg", image, *buffer);
    return buffer;
  }
  throw std::invalid_argument("No data in image with name " + name);
}

SceneInterface::image_ptr Frame::get_original_image_as_jpg() {
  if (!images_[0].empty()) {
    SceneInterface::image_ptr buffer(new std::vector<unsigned char>());
    cv::imencode(".jpg", images_[0], *buffer);
    return buffer;
  }
  throw std::invalid_argument("No data in original image");
}

SceneInterface::image_ptr Frame::get_image_as_jpg(const std::string& name1, const std::string& name2) {
  cv::Mat& image1 = get_image(name1);
  if (image1.empty())
    throw std::invalid_argument("Frame::getImageAsJPG - No data in image with name " + name1);

  cv::Mat& image2 = get_image(name2);
  if (!image2.empty()) {
    image2.copyTo(image1);
  }
  SceneInterface::image_ptr buffer(new std::vector<unsigned char>());
  cv::imencode(".jpg", image1, *buffer);
  return buffer;
}

SceneInterface::image_ptr Frame::get_original_with_overlay_image_as_jpg() {
  if (images_[0].empty())
    throw std::invalid_argument("No data in original image");

  images_[1].copyTo(images_[0], images_[1]);
  SceneInterface::image_ptr buffer(new std::vector<unsigned char>());
  cv::imencode(".jpg", images_[0], *buffer);
  return buffer;
}

void Frame::set_start_time() {
  this->start_time_ = boost::posix_time::microsec_clock::local_time();
}

int Frame::get_frame_id() const {
  return frame_id_;
}

const std::vector<cv::Rect>& Frame::getRoIs() const {
  return regions_of_interest_;
}

std::vector<cv::Rect>& Frame::get_regions_of_interest() {
  return regions_of_interest_;
}
