/*
 * TimedWorkWrapper.h
 *
 *  Created on: Aug 27, 2017
 *      Author: vagrant
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#ifndef PIPELINE_TIMEDWORKWRAPPER_H_
#define PIPELINE_TIMEDWORKWRAPPER_H_

#include <boost/function.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>
#include <boost/bind.hpp>
#include <boost/bind/placeholders.hpp>
#include <stdexcept>
#include <memory>
#include <chrono>
#include <iostream>
#include <vector>
#include <queue>
#include <utility>
#include "WorkWrapper.h"

namespace pipeline {

using TimeStatsQueue = std::queue<std::pair<std::chrono::time_point<std::chrono::high_resolution_clock>,
                                            std::chrono::time_point<std::chrono::high_resolution_clock>>>;

template <class T>
class TimedWorkWrapper : public WorkWrapper<T> {
 public:
  TimedWorkWrapper() = delete;
  explicit TimedWorkWrapper(boost::asio::io_service& io_service,
                            boost::function<void(std::shared_ptr<T>&)> work,
                            const unsigned int milliseconds,
                            std::shared_ptr<TimeStatsQueue> time_stats_queue);
  ~TimedWorkWrapper();

  void schedule_work(std::shared_ptr<T>& data);
  void pause();
  void set_interval(const int milliseconds) { milliseconds_ = milliseconds; }

 private:
  unsigned int milliseconds_;
  boost::asio::steady_timer timer_;
  void do_work(const boost::system::error_code& ec);
  void start_timer();
  void restart_timer();
  bool paused_ = false;
};

template <class T>
TimedWorkWrapper<T>::TimedWorkWrapper(boost::asio::io_service& io_service,
                                      boost::function<void(std::shared_ptr<T>&)> work,
                                      const unsigned int milliseconds,
                                      std::shared_ptr<TimeStatsQueue> time_stats_queue)
  : WorkWrapper<T>::WorkWrapper(io_service, work, time_stats_queue),
    milliseconds_(milliseconds),
    timer_(io_service) {
}

template <class T>
TimedWorkWrapper<T>::~TimedWorkWrapper() {
}

template <class T>
void TimedWorkWrapper<T>::schedule_work(std::shared_ptr<T>& /*data*/) {
  start_timer();
}

template <class T>
void TimedWorkWrapper<T>::do_work(const boost::system::error_code& ec) {
  // Check for some kind of error
  if (ec) {
    if (ec == boost::asio::error::operation_aborted)
      return;
    else
      throw ec.message();
  }

  // Create some dummy data
  std::shared_ptr<T> data;
  WorkWrapper<T>::do_work(data);

  // Set the timer again
  restart_timer();
}

template <class T>
void TimedWorkWrapper<T>::pause() {
  if (paused_) {
    start_timer();
    paused_ = false;
  } else {
    timer_.cancel();
    paused_ = true;
  }
}

template <class T>
void TimedWorkWrapper<T>::start_timer() {
  // Reset the timer from now (restarting the camera takes time)
  timer_.expires_from_now(std::chrono::milliseconds(milliseconds_));
  timer_.async_wait(WorkWrapper<T>::strand_.wrap(
      boost::bind(&TimedWorkWrapper<T>::do_work, this, boost::placeholders::_1)));
}

template <class T>
void TimedWorkWrapper<T>::restart_timer() {
  // Reset the timer
  timer_.expires_at(timer_.expires_at() + std::chrono::milliseconds(milliseconds_));
  timer_.async_wait(WorkWrapper<T>::strand_.wrap(
      boost::bind(&TimedWorkWrapper<T>::do_work, this, boost::placeholders::_1)));
}
} // namespace pipeline


#endif // PIPELINE_TIMEDWORKWRAPPER_H_
