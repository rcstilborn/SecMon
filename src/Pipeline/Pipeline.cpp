/*
 * Pipeline.cpp
 *
 *  Created on: Aug 27, 2017
 *      Author: Richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#include "Pipeline.h"
#include <boost/asio/io_service.hpp>
#include <memory>
#include <vector>
#include <exception>
#include <iostream>
#include "WorkWrapper.h"
#include "TimedWorkWrapper.h"

namespace pipeline {

Pipeline::Pipeline(boost::asio::io_service& io_service)
  : io_service_(io_service),
    elements_() {
}

Pipeline::~Pipeline() {
}

void Pipeline::add(TimedElement te) {
  if (compiled_) throw "Can't add elements once the pipeline has been compiled";
  if (!elements_.empty()) throw "Can only add TimedElement to an empty pipeline";
  elements_.push_back(std::unique_ptr<WorkWrapper>(new TimedWorkWrapper(io_service_, te.work_, te.milliseconds_)));
}

void Pipeline::add(Element e) {
  if (compiled_) throw "Can't add elements once the pipeline has been compiled";
  if (elements_.empty()) throw "Can only add regular Elements after a TimedElement or BlockingElement has been added";
  elements_.push_back(std::unique_ptr<WorkWrapper>(new WorkWrapper(io_service_, e.work_)));
}

void Pipeline::compile() {
  if (elements_.size() > 1) {
    auto it_n_minus_1 = elements_.begin();
    auto it_n = elements_.begin();
    ++it_n;
//    std::cout << "Size now: " << elements_.size() << std::endl;
    while (it_n != elements_.end()) {
//      std::cout << "it_n_minus_1: " << it_n_minus_1 - elements_.begin()
//                << ". it_n: " << it_n - elements_.begin() << std::endl;
      (*it_n_minus_1)->set_next((*it_n)->get_schedule_work());
      ++it_n_minus_1;
      ++it_n;
    }
  }
  compiled_ = true;
}

void Pipeline::pause() {
  if (!compiled_) throw "Can't perform pause operation before pipeline is compiled";
  if (!started_) throw "Can't perform pause operation before pipeline is started";
  elements_.front()->pause();
  if (paused_)
    paused_ = false;
  else
    paused_ = true;
}

void Pipeline::start() {
  if (!compiled_) throw "Can't perform start operation before pipeline is compiled";
  if (started_) throw "Pipeline has already been started!";
  elements_.front()->schedule_work(nullptr);
  started_ = true;
}
} // namespace pipeline




