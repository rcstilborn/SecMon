/*
 * Component.h
 *
 *  Created on: Aug 30, 2017
 *      Author: Richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#ifndef COMPONENT_H_
#define COMPONENT_H_

#include <memory>

class Frame;

class Component {
 public:
  virtual ~Component() {}
  virtual void process_next_frame(std::shared_ptr<Frame>&) = 0;
};

#endif // COMPONENT_H_
