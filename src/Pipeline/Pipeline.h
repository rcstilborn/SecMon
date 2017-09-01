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
#include <memory>
#include <vector>
#include <exception>
#include <iostream>
#include <utility>
#include "TimedWorkWrapper.h"
#include "WorkWrapper.h"

namespace pipeline {

enum TimerType { Interval, Delay };

template <class T>
class Pipeline {
 public:
  Pipeline() = delete;
  explicit Pipeline(boost::asio::io_service& io_service);
  virtual ~Pipeline();

  void addTimedElement(TimerType, const int, boost::function<void(std::shared_ptr<T>&)>);
  // TODO(Richard): Add blocking element type for webcam feeds
  // void addBlockingElement(boost::function<void(T)> work);
  void addElement(boost::function<void(std::shared_ptr<T>&)> work);
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
  std::vector<std::unique_ptr<WorkWrapper<T>>> elements_;

  // Make sure we can't copy them
  Pipeline(const Pipeline&) = delete;
  Pipeline& operator=(Pipeline) = delete;
};

template <class T>
Pipeline<T>::Pipeline(boost::asio::io_service& io_service)
  : io_service_(io_service),
    elements_() {
}

template <class T>
Pipeline<T>::~Pipeline() {
}

template <class T>
void Pipeline<T>::addTimedElement(TimerType /*timer_type*/,
                                  const int interval, boost::function<void(std::shared_ptr<T>&)> work) {
  if (compiled_) throw "Can't add elements once the pipeline has been compiled";
  if (!elements_.empty()) throw "Can only add TimedElement to an empty pipeline";
  elements_.emplace_back(new TimedWorkWrapper<T>(io_service_, work, interval));
}

template <class T>
void Pipeline<T>::addElement(boost::function<void(std::shared_ptr<T>&)> work) {
  if (compiled_) throw "Can't add elements once the pipeline has been compiled";
  if (elements_.empty()) throw "Can only add regular Elements after a TimedElement or BlockingElement has been added";
  elements_.emplace_back(new WorkWrapper<T>(io_service_, work));
}

template <class T>
void Pipeline<T>::compile() {
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

template <class T>
void Pipeline<T>::pause() {
  if (!compiled_) throw "Can't perform pause operation before pipeline is compiled";
  if (!started_) throw "Can't perform pause operation before pipeline is started";
  elements_.front()->pause();
  if (paused_)
    paused_ = false;
  else
    paused_ = true;
}

template <class T>
void Pipeline<T>::start() {
  if (!compiled_) throw "Can't perform start operation before pipeline is compiled";
  if (started_) throw "Pipeline has already been started!";
  auto dummy_data = std::shared_ptr<T>(nullptr);
  elements_.front()->schedule_work(dummy_data);
  started_ = true;
}

template <class T>
void Pipeline<T>::change_timer(const unsigned int milliseconds) {
  if (!compiled_) throw "Can't perform change_timer operation before pipeline is compiled";
  std::cout << "Would be changing the timer to: " << milliseconds << std::endl;
}

} // namespace pipeline


#endif // PIPELINE_PIPELINE_H_

