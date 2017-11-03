/*
 * MovementDetectorMOG2.h
 *
 *  Created on: Oct 18, 2017
 *      Author: richard
 */

#ifndef SRC_IMAGEPROCESSING_MOVEMENTDETECTORMOG2_H_
#define SRC_IMAGEPROCESSING_MOVEMENTDETECTORMOG2_H_

#include "MovementDetector.h"
#include <opencv2/video/background_segm.hpp>
#include <memory>

class MovementDetectorMOG2 : public MovementDetector {
 public:
  MovementDetectorMOG2();
  virtual ~MovementDetectorMOG2();
  virtual void process_next_frame(std::shared_ptr<Frame>& frame);

 private:
  cv::Ptr<cv::BackgroundSubtractor> pMOG2;

  void refineSegments(const cv::Mat& mask, cv::Mat& dst);
};

#endif /* SRC_IMAGEPROCESSING_MOVEMENTDETECTORMOG2_H_ */
