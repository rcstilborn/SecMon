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

#include <opencv2/core/cvdef.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/mat.inl.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <chrono>

Frame::Frame(const int width, const int height)
    : frame_size_(width, height),
//      start_time_(),
      regions_of_interest_(),
      border_height_(height * 0.04),
      text_size_(height/1000.0),
      text_from_top_(border_height_ - 12),
      display_time_(),
      display_name_(),
      display_stats_() {
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

ScenePublisher::image_ptr Frame::get_image_as_jpg(const std::string& name) {
  // TODO(richard): make this hack more elegant
  if (name == "main")
    return get_original_with_overlay_image_as_jpg();
  cv::Mat& image = get_image(name);
  if (!image.empty()) {
    ScenePublisher::image_ptr buffer(new std::vector<unsigned char>());
    cv::imencode(".jpg", image, *buffer);
    return buffer;
  }
  throw std::invalid_argument("No data in image with name " + name);
}

ScenePublisher::image_ptr Frame::get_original_image_as_jpg() {
  if (!images_[0].empty()) {
    ScenePublisher::image_ptr buffer(new std::vector<unsigned char>());
//    cv::imencode(".jpg", images_[0], *buffer);
    cv::imencode(".jpg", images_[0], *buffer, std::vector<int>({cv::IMWRITE_JPEG_QUALITY, 90}));
    return buffer;
  }
  throw std::invalid_argument("No data in original image");
}

ScenePublisher::image_ptr Frame::get_image_as_jpg(const std::string& name1, const std::string& name2) {
  cv::Mat& image1 = get_image(name1);
  if (image1.empty())
    throw std::invalid_argument("Frame::getImageAsJPG - No data in image with name " + name1);

  cv::Mat& image2 = get_image(name2);
  if (!image2.empty()) {
    image2.copyTo(image1);
  }
  ScenePublisher::image_ptr buffer(new std::vector<unsigned char>());
  cv::imencode(".jpg", image1, *buffer);
  return buffer;
}

ScenePublisher::image_ptr Frame::get_original_with_overlay_image_as_jpg() {
  if (images_[0].empty())
    throw std::invalid_argument("No data in original image");

  cv::Mat display_image;
  cv::copyMakeBorder(images_[0], display_image, border_height_, 0, 0, 0, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
//  images_[0].copyTo(display_image);

  images_[1].copyTo(display_image(cv::Rect(0, border_height_, frame_size_.width, frame_size_.height)), images_[1]);

  if (!display_time_.empty()) {
    cv::putText(display_image, display_time_, cv::Point(40, border_height_ - 15),
                cv::FONT_HERSHEY_SIMPLEX, text_size_, cv::Scalar(255, 255, 255), 2);
  }

  if (!display_name_.empty()) {
    int baseline;
    cv::Size text_size = cv::getTextSize(display_name_, cv::FONT_HERSHEY_SIMPLEX, text_size_, 2, &baseline);
    cv::putText(display_image, display_name_, cv::Point(frame_size_.width/2 - text_size.width/2, border_height_ - 15),
                cv::FONT_HERSHEY_SIMPLEX, text_size_, cv::Scalar(255, 255, 255), 2);
  }

  if (!display_stats_.empty()) {
    int baseline;
    cv::Size text_size = cv::getTextSize(display_stats_, cv::FONT_HERSHEY_SIMPLEX, text_size_, 2, &baseline);
    cv::putText(display_image, display_stats_, cv::Point(frame_size_.width - text_size.width - 40, border_height_ - 15),
                cv::FONT_HERSHEY_SIMPLEX, text_size_, cv::Scalar(255, 255, 255), 2);
  }

  ScenePublisher::image_ptr buffer(new std::vector<unsigned char>());
  cv::imencode(".jpg", display_image, *buffer);
  return buffer;
}

const std::vector<cv::Rect>& Frame::getRoIs() const {
  return regions_of_interest_;
}

std::vector<cv::Rect>& Frame::get_regions_of_interest() {
  return regions_of_interest_;
}
