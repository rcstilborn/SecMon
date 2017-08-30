/*
 * WorkWrapper.cpp
 *
 *  Created on: Aug 27, 2017
 *      Author: Richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#include "WorkWrapper.h"

#include <boost/function.hpp>
#include <boost/bind/bind.hpp>
#include <boost/any.hpp>
#include <stdexcept>
#include <memory>

namespace pipeline {

WorkWrapper::WorkWrapper(boost::asio::io_service& io_service,
                         boost::function<void(std::shared_ptr<boost::any>)> work)
: strand_(io_service), work_(work), next_() {
}

WorkWrapper::~WorkWrapper() {
}

void WorkWrapper::schedule_work(std::shared_ptr<boost::any> data) {
  strand_.post(boost::bind(&WorkWrapper::do_work, this, data));
}

void WorkWrapper::do_work(std::shared_ptr<boost::any> data) {
  //    Time
  work_(data);
  //     Time
  if (!next_.empty()) next_(data);
}
} // namespace pipeline

