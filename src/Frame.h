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

#include <boost/date_time/posix_time/ptime.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#ifndef CPU_ONLY
#include <opencv2/core/cuda.hpp>
#endif  // CPU_ONLY

#include <string>
#include <vector>

#include "SceneInterface.h"

const int kMaxImagesPerFrame = 10;

class Frame {
 public:
  Frame(const int frame_id, const int width, const int height);
  virtual ~Frame();

  void set_start_time();
  int get_frame_id() const;

  cv::Mat& get_original_image();
  cv::Mat& get_overlay_image();
  cv::Mat& get_new_or_existing_image(const std::string& name);
  cv::Mat& get_new_image(const std::string& name);
  cv::Mat& get_new_blank_image(const std::string& name);
  cv::Mat& get_image(const std::string& name);
  SceneInterface::image_ptr get_image_as_jpg(const std::string& name);
  SceneInterface::image_ptr get_image_as_jpg(const std::string& name1, const std::string& name2);
  SceneInterface::image_ptr get_original_image_as_jpg();
  SceneInterface::image_ptr get_original_with_overlay_image_as_jpg();
  const std::vector<cv::Rect>& getRoIs() const;
  std::vector<cv::Rect>& get_regions_of_interest();

 private:
  const int frame_id_;
  // Make sure we can't copy them
  Frame(const Frame&) = delete;
  Frame& operator=(Frame) = delete;
  cv::Size frame_size_;
  cv::Mat images_[kMaxImagesPerFrame];
#ifndef CPU_ONLY
  cv::cuda::GpuMat gpu_images_[kMaxImagesPerFrame];
#endif  // CPU_ONLY

  std::string image_names_[kMaxImagesPerFrame];
  boost::posix_time::ptime start_time_;
  std::vector<cv::Rect> regions_of_interest_;
};

#endif  // FRAME_H_
