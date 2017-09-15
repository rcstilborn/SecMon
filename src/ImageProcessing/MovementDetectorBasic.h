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
#include <memory>

#include "MovementDetector.h"
class Frame;

class MovementDetectorBasic : public MovementDetector {
 public:
  MovementDetectorBasic();
  virtual ~MovementDetectorBasic();

  void process_next_frame(std::shared_ptr<Frame>& frame);

 private:
  void search_for_movement(cv::Mat thresholdImage, cv::Mat &cameraFeed);
  std::shared_ptr<Frame> previous_frame_;
  unsigned int frames_since_last_diff_ = 0;
};

#endif // IMAGEPROCESSING_MOVEMENTDETECTORBASIC_H_
