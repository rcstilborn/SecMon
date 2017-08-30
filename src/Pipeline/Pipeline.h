/*
 * Pipeline.h
 *
 *  Created on: Aug 27, 2017
 *      Author: Richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 *
 *  Data processing pipeline for sequential data such as video streams.  Each element in the pipeline is called
 *  sequentially and in its own strand.  This guarantees that each piece of data is processed by each step AND
 *  that each step processes the data in order as well. It also guarantees that any element that blocks doesn't hold
 *  up other elements that could do work.
 *
 *  First element is the data pump - it must use timers or blocking calls to repeatedly pump data into the pipeline
 *  Other elements do the work
 *
 *  It is modeled after the keras neural network classes. Example usage:
 *
 *  Pipeline p();
 *
 *  p.add(FirstElement(foo, bar));
 *  p.add(MiddleElement());
 *  p.add(MiddleElement(baz));
 *
 *  p.compile(); // Joins the elements together
 *  p.run();     // Calls the execute function on the first element
 *
 *  p.pause();   // toggles on - stops pipeline at start - i.e. current data flows to end of pipeline
 *  p.pause();   // toggles off
 *
 *  p.stop();    // Stops for good - ?????
 */

#ifndef PIPELINE_PIPELINE_H_
#define PIPELINE_PIPELINE_H_

#include <boost/asio/io_service.hpp>
#include <boost/function.hpp>
#include <boost/any.hpp>
#include <memory>
#include <vector>
#include "WorkWrapper.h"

namespace pipeline {

/*
 * wrapper for a basic (not first) element
 * Must provide a function that takes a shared pointer and returns void
 */
struct Element{
  Element() = delete;
  explicit Element(boost::function<void(std::shared_ptr<boost::any>)> work) : work_(work) {}
  boost::function<void(std::shared_ptr<boost::any>)> work_;  // This is the work that gets done
};

/*
 * TimerType:
 *   Interval means the output happens every x milliseconds
 *   Delay means there is x milliseconds between each attempt to get the data
 */
enum TimerType { Interval, Delay };

struct TimedElement{
  TimedElement() = delete;
  TimedElement(TimerType timer_type, const int milliseconds, boost::function<void(std::shared_ptr<boost::any>)> work)
  : work_(work), timer_type_(timer_type), milliseconds_(milliseconds) {}
  boost::function<void(std::shared_ptr<boost::any>)> work_;  // This is the work that gets done
  TimerType timer_type_;
  unsigned int milliseconds_;
};

class Pipeline {
 public:
  Pipeline() = delete;
  explicit Pipeline(boost::asio::io_service& io_service);
  virtual ~Pipeline();

  void add(TimedElement te);
  // TODO(Richard): Add blocking element type for webcam feeds
  // void add(BlockingElement be);
  void add(Element e);
  void change_timer(const unsigned int milliseconds);
  void compile();
  void pause();
  void start();
  bool is_compiled() const { return compiled_; }
  bool is_paused() const { return paused_; }
  bool is_started() const { return started_; }

 private:
  boost::asio::io_service& io_service_;
  bool paused_ = false;
  bool started_ = false;
  bool compiled_ = false;
  std::vector<std::unique_ptr<WorkWrapper>> elements_;
};
} // namespace pipeline

#endif // PIPELINE_PIPELINE_H_

