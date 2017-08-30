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
#include <boost/any.hpp>
#include <boost/bind.hpp>
#include <boost/bind/placeholders.hpp>
#include <memory>

namespace pipeline {

class WorkWrapper {
 public:
  WorkWrapper() = delete;
  explicit WorkWrapper(boost::asio::io_service& io_service, boost::function<void(std::shared_ptr<boost::any>)> work);
  virtual ~WorkWrapper();
  virtual void schedule_work(std::shared_ptr<boost::any>);
  virtual void pause() {}

  void set_next(boost::function<void(std::shared_ptr<boost::any>)> next) {next_ = next;}
  boost::function<void(std::shared_ptr<boost::any>)> get_schedule_work() {
    return boost::bind(&WorkWrapper::schedule_work, this, boost::placeholders::_1); }

 protected:
  boost::asio::strand strand_;
  boost::function<void(std::shared_ptr<boost::any>)> work_;  // This is the work that gets done
  boost::function<void(std::shared_ptr<boost::any>)> next_;  // This points to schedule_work of next element
  virtual void do_work(std::shared_ptr<boost::any>);

 private:
};
} // namespace pipeline

#endif // PIPELINE_WORKWRAPPER_H_


//Element (fn(data))
//Start
//Pause
//Process next sets strand post for process
//Process
//    Time
//     Call fn passing data
//     Time
//     If Next then next & return true (means there is more)
//    Else return false so pipeline can delete data(?)


// FirstElementTimedInterval(ms, fn)) - sets time from last timer
// FirstElementTimedDelay(ms, fn) - sets time from end of execution
// FirstElementBlocking(fn) - calls execute repeatedly
