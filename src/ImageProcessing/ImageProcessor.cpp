/*
 * ImageProcessor.cpp
 *
 *  Created on: Aug 14, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#include "ImageProcessor.h"

#include <glog/logging.h>
#include <exception>
#include <iostream>
#include <memory>

#include "MovementDetectorBasic.h"

ImageProcessor::ImageProcessor()
    : roi_() {
  movement_detector_ = new MovementDetectorBasic;
//    movementDetector = new MovementDetectorViBe;
}

ImageProcessor::~ImageProcessor() {
  delete movement_detector_;
}

void ImageProcessor::process_next_frame(std::shared_ptr<Frame>& frame) {

  frame->set_start_time();

  movement_detector_->process_frame(frame);
//        bg1.subtractBackground(frame0);
  //    imbs.apply(frame0->getOriginalImage(),frame0->getNewImage("imbs"));

  roi_.processFrame(frame);
}
