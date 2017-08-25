/*
 * ROI_Detector.h
 *
 *  Created on: Aug 16, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#ifndef IMAGEPROCESSING_ROI_DETECTOR_H_
#define IMAGEPROCESSING_ROI_DETECTOR_H_

#include <opencv2/core/types.hpp>
#include <cstdbool>
#include <vector>
#include <memory>

#include "../Frame.h"

class ROI_Detector {
 public:
  ROI_Detector();
  virtual ~ROI_Detector();

  void processFrame(std::shared_ptr<Frame> frame0);

//private:
  static bool is_too_small_(std::vector<cv::Point> contour);
  static bool compare_rect_size_(const cv::Rect& a, const cv::Rect& b);
  static bool is_inside_(const cv::Rect& small, const cv::Rect& large);
  static int overlap_percentage_(const cv::Rect& small, const cv::Rect& large);
  void remove_nested(std::vector<cv::Rect>& rects);
  void consolidate_rectangles(std::vector<cv::Rect>& rects);
};

#endif // IMAGEPROCESSING_ROI_DETECTOR_H_
