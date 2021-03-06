/*
 * ROI_Detector.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#include "ROI_Detector.h"

#include <opencv2/core/mat.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc.hpp>
#include <algorithm>
#include <iterator>
#include <vector>
#include <memory>
#include "../Frame.h"

ROI_Detector::ROI_Detector() {
}

ROI_Detector::~ROI_Detector() {
}

bool ROI_Detector::is_too_small_(std::vector<cv::Point> contour) {
  return cv::contourArea(contour) < 8;
}

bool ROI_Detector::compare_rect_size_(const cv::Rect& a, const cv::Rect& b) {
  return (a.area() < b.area());
}

bool ROI_Detector::is_inside_(const cv::Rect& small, const cv::Rect& large) {
  // for small to be inside large the following can not be true
  // small.x < large.x...
  return !(small.x < large.x || small.y < large.y || small.x + small.width > large.x + large.width
      || small.y + small.height > large.y + large.height);
}

int ROI_Detector::overlap_percentage_(const cv::Rect& small, const cv::Rect& large) {
  cv::Rect overlap = small & large;
  return (overlap.area() / static_cast<double>(small.area())) * 100;
}

void ROI_Detector::remove_nested(std::vector<cv::Rect>& rects) {
  // Remove the nested rectangles
  // Start from the small end - only need to compare rectangles with >= ones.
  std::vector<cv::Rect>::iterator rect_it;
  std::vector<cv::Rect>::iterator rect_it2;
  for (rect_it = rects.begin(); rect_it != rects.end();) {
    for (rect_it2 = rects.end() - 1; rect_it2 > rect_it;) {
      if (is_inside_(*rect_it, *rect_it2)) {
        rect_it = rects.erase(rect_it);
        goto here;
      }
      int i = overlap_percentage_(*rect_it, *rect_it2);
      i++;
      --rect_it2;
    }
    ++rect_it;
    here: {}
  }
}

void ROI_Detector::consolidate_rectangles(std::vector<cv::Rect>& rects) {
  // Remove the nested rectangles
  // Consolidate highly overlapping rectanges
  // Start from the small end - only need to compare rectangles with >= ones.
  std::vector<cv::Rect>::iterator small_it;
  std::vector<cv::Rect>::iterator large_it;
  int nestedROIsDeleted = 0, smallerROIsDeleted = 0;
  for (small_it = rects.begin(); small_it != rects.end();) {
    for (large_it = rects.end() - 1; large_it > small_it;) {
      int overlap = overlap_percentage_(*small_it, *large_it);
      if (overlap == 100) {
        small_it = rects.erase(small_it);
        nestedROIsDeleted++;
        goto here;
      } else if (overlap > 85) {
        // Make the larger one larger and the delete the small one.
        large_it->width += std::max(0, (large_it->x - small_it->x));
        large_it->x = std::min(large_it->x, small_it->x);

        large_it->height += std::max(0, (large_it->y - small_it->y));
        large_it->y = std::min(large_it->y, small_it->y);

        large_it->width += std::max(0, (small_it->x + small_it->width) - (large_it->x + large_it->width));
        large_it->height += std::max(0, (small_it->y + small_it->height) - (large_it->y + large_it->height));
        small_it = rects.erase(small_it);
        smallerROIsDeleted++;
      }
      --large_it;
    }
    ++small_it;
    here: {}
  }
}

void ROI_Detector::process_next_frame(std::shared_ptr<Frame>& current_frame) {
  try {
    cv::Mat& foreground = current_frame->get_image("foreground");
    cv::Mat& overlay_image = current_frame->get_overlay_image();

    // Find the contours
    cv::Mat temp;
    foreground.copyTo(temp);

    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    // retrieve external contours
    cv::findContours(temp, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    // Remove the small ones
    contours.erase(std::remove_if(contours.begin(), contours.end(), ROI_Detector::is_too_small_), contours.end());

//    cv::drawContours(overlay_image, contours, -1, cv::Scalar(0, 255, 0), 20);

    // Build the list of rectangles
    std::vector<cv::Rect>& rects = current_frame->get_regions_of_interest();
    for (auto contour : contours)
      rects.push_back(cv::boundingRect(contour));

    // Sort the list
    std::sort(rects.begin(), rects.end(), ROI_Detector::compare_rect_size_);

    // Remove the nested rectangles
    // Start from the small end - only need to compare rectangles with >= ones.
    consolidate_rectangles(rects);

    for (auto rect : rects) {
      cv::rectangle(overlay_image, rect, cv::Scalar(0, 255, 0), 2);
    }
  } catch (std::exception& e) {
    return;
  }
}
