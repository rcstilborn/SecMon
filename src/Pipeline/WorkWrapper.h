/*
 * WorkWrapper.h
 *
 *  Created on: Aug 27, 2017
 *      Author: Richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 *
 *  This is a wrapper for the actual work functions of the pipeline.  It provides the boost asio mechanism and
 *  the glue that holds the pipeline together.
 *
 *  The element takes a function pointer that gets executed at the right time.
 */

#ifndef PIPELINE_WORKWRAPPER_H_
#define PIPELINE_WORKWRAPPER_H_

#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/bind/placeholders.hpp>
#include <memory>
#include <queue>
#include <utility>
#include <chrono>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <ctime>

namespace pipeline {

using TimeStatsQueue = std::queue<std::pair<std::chrono::time_point<std::chrono::high_resolution_clock>,
                                            std::chrono::time_point<std::chrono::high_resolution_clock>>>;

template <class T>
class WorkWrapper {
 public:
  WorkWrapper() = delete;
  explicit WorkWrapper(boost::asio::io_service& io_service,
                       boost::function<void(std::shared_ptr<T>&)> work,
                       std::shared_ptr<TimeStatsQueue> time_stats_queue);
  virtual ~WorkWrapper();
  virtual void schedule_work(std::shared_ptr<T>&);

  void set_next(boost::function<void(std::shared_ptr<T>&)> next) {next_ = next;}
  boost::function<void(std::shared_ptr<T>&)> get_schedule_work() {
    return boost::bind(&WorkWrapper<T>::schedule_work, this, boost::placeholders::_1); }

 protected:
  boost::asio::strand strand_;
  boost::function<void(std::shared_ptr<T>&)> work_;  // This is the work that gets done
  boost::function<void(std::shared_ptr<T>&)> next_;  // This points to schedule_work of next element
  virtual void do_work(std::shared_ptr<T>&);         // This calls work_ then next_
  std::shared_ptr<TimeStatsQueue> time_stats_queue_;
};

template <class T>
WorkWrapper<T>::WorkWrapper(boost::asio::io_service& io_service,
                            boost::function<void(std::shared_ptr<T>&)> work,
                            std::shared_ptr<TimeStatsQueue> time_stats_queue)
: strand_(io_service), work_(work), next_(), time_stats_queue_(time_stats_queue) {
}

template <class T>
WorkWrapper<T>::~WorkWrapper() {
}

template <class T>
void WorkWrapper<T>::schedule_work(std::shared_ptr<T>& data) {
  // Record Time
  strand_.post(boost::bind(&WorkWrapper<T>::do_work, this, data));
}

template <class T>
void WorkWrapper<T>::do_work(std::shared_ptr<T>& data) {
  // Record time
  auto start_time = std::chrono::high_resolution_clock::now();

  // Do the work
  work_(data);

  // Report Times
  auto end_time = std::chrono::high_resolution_clock::now();
  if (time_stats_queue_ != nullptr)
    time_stats_queue_->emplace(start_time, end_time);

  // Schedule the next element in the pipeline
  if (!next_.empty()) {
    next_(data);
  }
}
} // namespace pipeline

#endif // PIPELINE_WORKWRAPPER_H_
