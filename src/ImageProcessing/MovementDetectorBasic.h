/*
 * MovementDetectorBasic.h
 *
 *  Created on: Aug 14, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#ifndef IMAGEPROCESSING_MOVEMENTDETECTORBASIC_H_
#define IMAGEPROCESSING_MOVEMENTDETECTORBASIC_H_
#include <opencv2/opencv.hpp>
#include <boost/shared_ptr.hpp>

#include "MovementDetector.h"

class MovementDetectorBasic : public MovementDetector {
 public:
  MovementDetectorBasic();
  virtual ~MovementDetectorBasic();

  void process_frame(boost::shared_ptr<Frame> frame0);

 private:
  void search_for_movement(cv::Mat thresholdImage, cv::Mat &cameraFeed);
  cv::Mat previous_image_;
};

#endif // IMAGEPROCESSING_MOVEMENTDETECTORBASIC_H_
