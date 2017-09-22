/*
 * Frame.h
 *
 *  Created on: Jul 27, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#ifndef FRAME_H_
#define FRAME_H_

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#ifndef CPU_ONLY
#include <opencv2/core/cuda.hpp>
#endif  // CPU_ONLY

#include <string>
#include <vector>
#include <queue>
#include <chrono>

// TODO(richard): refactor away
#include "ScenePublisher.h"

const int kMaxImagesPerFrame = 10;

class Frame {
 public:
  Frame() = delete;
  Frame(const int width, const int height);
//  Frame(const int frame_id, const int width, const int height);
  virtual ~Frame();

//  void set_start_time();

  cv::Mat& get_original_image();
  cv::Mat& get_overlay_image();
  cv::Mat& get_new_or_existing_image(const std::string& name);
  cv::Mat& get_new_image(const std::string& name);
  cv::Mat& get_new_blank_image(const std::string& name);
  cv::Mat& get_image(const std::string& name);
  ScenePublisher::image_ptr get_image_as_jpg(const std::string& name);
  ScenePublisher::image_ptr get_image_as_jpg(const std::string& name1, const std::string& name2);
  ScenePublisher::image_ptr get_original_image_as_jpg();
  ScenePublisher::image_ptr get_original_with_overlay_image_as_jpg();
  const std::vector<cv::Rect>& getRoIs() const;
  std::vector<cv::Rect>& get_regions_of_interest();
  void set_display_time(const std::string time) {display_time_ = time; }
  void set_display_name(const std::string name) {display_name_ = name; }
  void set_display_stats(const std::string stats) {display_stats_ = stats; }
  //void set_record_indicator();
 private:
  // Make sure we can't copy them
  Frame(const Frame&) = delete;
  Frame& operator=(Frame) = delete;
  cv::Size frame_size_;
  cv::Mat images_[kMaxImagesPerFrame];
#ifndef CPU_ONLY
  cv::cuda::GpuMat gpu_images_[kMaxImagesPerFrame];
#endif  // CPU_ONLY

  std::string image_names_[kMaxImagesPerFrame];
//  std::chrono::system_clock::time_point start_time_;
  std::vector<cv::Rect> regions_of_interest_;

  const int border_height_;
  const double text_size_;
  const int text_from_top_;

  std::string display_time_;
  std::string display_name_;
  std::string display_stats_;
};

#endif  // FRAME_H_
