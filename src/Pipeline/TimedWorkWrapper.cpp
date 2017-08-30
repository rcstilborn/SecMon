/*
 * FirstElementTimed.cpp
 *
 *  Created on: Aug 27, 2017
 *      Author: Richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#include "TimedWorkWrapper.h"
#include <boost/function.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>
#include <boost/any.hpp>
#include <boost/bind.hpp>
#include <boost/bind/placeholders.hpp>
#include <stdexcept>
#include <memory>
#include <chrono>

namespace pipeline {

TimedWorkWrapper::TimedWorkWrapper(boost::asio::io_service& io_service,
                                   boost::function<void(std::shared_ptr<boost::any>)> work,
                                   const unsigned int milliseconds)
  : WorkWrapper::WorkWrapper(io_service, work),
    milliseconds_(milliseconds),
    timer_(io_service) {
}

TimedWorkWrapper::~TimedWorkWrapper() {
}

void TimedWorkWrapper::schedule_work(std::shared_ptr<boost::any> /*data*/) {
  start_timer();
}

void TimedWorkWrapper::do_work(const boost::system::error_code& ec) {
  // Check for some kind of error
  if (ec) throw ec.message();

  //    Time
  std::shared_ptr<boost::any> data;
  work_(data);
  //     Time
  if (!next_.empty()) next_(data);
}

void TimedWorkWrapper::pause() {
  if (paused_) {
    start_timer();
    paused_ = false;
  } else {
    timer_.cancel();
    paused_ = true;
  }
}

void TimedWorkWrapper::start_timer() {
  // Reset the timer from now (restarting the camera takes time)
  this->timer_.expires_from_now(std::chrono::milliseconds(milliseconds_));
  this->timer_.async_wait(strand_.wrap(boost::bind(&TimedWorkWrapper::do_work, this, boost::placeholders::_1)));
}

void TimedWorkWrapper::restart_timer() {
  // Reset the timer
  this->timer_.expires_at(this->timer_.expires_at() + std::chrono::milliseconds(milliseconds_));
  this->timer_.async_wait(strand_.wrap(boost::bind(&TimedWorkWrapper::do_work, this, boost::placeholders::_1)));
}
} // namespace pipeline
