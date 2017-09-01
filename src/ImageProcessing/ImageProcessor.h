/*
 * ImageProcessor.h
 *
 *  Created on: Aug 14, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#ifndef IMAGEPROCESSING_IMAGEPROCESSOR_H_
#define IMAGEPROCESSING_IMAGEPROCESSOR_H_

#include <string>
#include <memory>
#include "../Component.h"
#include "ROI_Detector.h"

class MovementDetector;
class Frame;

class ImageProcessor : public Component {
 public:
  ImageProcessor();
  virtual ~ImageProcessor();
  void process_next_frame(std::shared_ptr<Frame>&);

 private:
  ImageProcessor(const ImageProcessor&) = delete;
  ImageProcessor& operator=(const ImageProcessor&) = delete;


///diff, threshold, diff, blur method
  MovementDetector* movement_detector_ = NULL;

  /// OpenCV BackgroundSubtractor
//    BackgroundSubtractorCV bg1;

///IMBS Background Subtractor
//    BackgroundSubtractorIMBS imbs;

  ROI_Detector roi_;
};

#endif // IMAGEPROCESSING_IMAGEPROCESSOR_H_
