/*
 * MovementDetector.h
 *
 *  Created on: Oct 7, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */


#ifndef IMAGEPROCESSING_MOVEMENTDETECTOR_H_
#define IMAGEPROCESSING_MOVEMENTDETECTOR_H_

#include <boost/smart_ptr/shared_ptr.hpp>

#include "../Frame.h"

class Frame;

class MovementDetector {
 public:
  virtual void process_frame(boost::shared_ptr<Frame> frame0) = 0;
  virtual ~MovementDetector();
};

#endif // IMAGEPROCESSING_MOVEMENTDETECTOR_H_
