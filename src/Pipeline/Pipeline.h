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
#include <queue>
#include <chrono>
#include <exception>
#include <iostream>
#include <string>
#include <utility>
#include "TimedWorkWrapper.h"
#include "WorkWrapper.h"

namespace pipeline {

enum TimerType { Interval, Delay };
using TimeStatsQueue = std::queue<std::pair<std::chrono::time_point<std::chrono::high_resolution_clock>,
                                            std::chrono::time_point<std::chrono::high_resolution_clock>>>;

template <class T>
class Pipeline {
 public:
  Pipeline() = delete;
  explicit Pipeline(boost::asio::io_service& io_service, const unsigned int stats_interval = 0);
  virtual ~Pipeline();

  void addTimedElement(TimerType, const int, boost::function<void(std::shared_ptr<T>&)>);
  void addTimedElement(TimerType, const int, boost::function<void(std::shared_ptr<T>&)>, const std::string&);
  // TODO(Richard): Add blocking element type for webcam feeds
  // void addBlockingElement(boost::function<void(T)> work);
  void addElement(boost::function<void(std::shared_ptr<T>&)> work);
  void addElement(boost::function<void(std::shared_ptr<T>&)> work, const std::string&);
  void change_timer(const unsigned int milliseconds);
  void compile();
  void pause();
  void start();
  bool is_compiled() const { return compiled_; }
  bool is_paused() const { return paused_; }
  bool is_started() const { return started_; }
  void start_stats_timer();
  void collect_stats(const boost::system::error_code& ec);

 private:
  boost::asio::io_service& io_service_;
  boost::asio::strand strand_;
  boost::asio::steady_timer timer_;
  bool paused_ = false;
  bool started_ = false;
  bool compiled_ = false;
  std::vector<std::unique_ptr<WorkWrapper<T>>> elements_;
  const unsigned int stats_interval_;
  std::vector<std::shared_ptr<TimeStatsQueue>> time_stats_;
  std::chrono::time_point<std::chrono::high_resolution_clock> last_stats_time_;
  std::string stats_output_header_;
  // Make sure we can't copy them
  Pipeline(const Pipeline&) = delete;
  Pipeline& operator=(Pipeline) = delete;
};

template <class T>
Pipeline<T>::Pipeline(boost::asio::io_service& io_service, const unsigned int stats_interval)
  : io_service_(io_service),
    strand_(io_service),
    timer_(io_service),
    elements_(),
    stats_interval_(stats_interval),
    time_stats_(),
    last_stats_time_(),
    stats_output_header_("Interval") {
}

template <class T>
Pipeline<T>::~Pipeline() {
}

template <class T>
void Pipeline<T>::addTimedElement(TimerType /*timer_type*/,
                                  const int interval, boost::function<void(std::shared_ptr<T>&)> work) {
  if (compiled_) throw "Can't add elements once the pipeline has been compiled";
  if (!elements_.empty()) throw "Can only add TimedElement to an empty pipeline";
  if (stats_interval_ > 0) {
    time_stats_.emplace_back(new TimeStatsQueue());
    elements_.emplace_back(new TimedWorkWrapper<T>(io_service_, work, interval, time_stats_.back()));
  } else {
    elements_.emplace_back(new TimedWorkWrapper<T>(io_service_, work, interval, nullptr));
  }
}

template <class T>
void Pipeline<T>::addTimedElement(TimerType timer_type,
                                  const int interval, boost::function<void(std::shared_ptr<T>&)> work,
                                  const std::string& stats_output_header) {
  if (compiled_) throw "Can't add elements once the pipeline has been compiled";
  if (!elements_.empty()) throw "Can only add TimedElement to an empty pipeline";
  stats_output_header_.append(stats_output_header);
  this->addTimedElement(timer_type, interval, work);
}

template <class T>
void Pipeline<T>::addElement(boost::function<void(std::shared_ptr<T>&)> work) {
  if (compiled_) throw "Can't add elements once the pipeline has been compiled";
  if (elements_.empty()) throw "Can only add regular Elements after a TimedElement or BlockingElement has been added";
  if (stats_interval_ > 0) {
    time_stats_.emplace_back(new TimeStatsQueue());
    elements_.emplace_back(new WorkWrapper<T>(io_service_, work, time_stats_.back()));
  } else {
    elements_.emplace_back(new WorkWrapper<T>(io_service_, work, nullptr));
  }
}

template <class T>
void Pipeline<T>::addElement(boost::function<void(std::shared_ptr<T>&)> work,
                             const std::string& stats_output_header) {
  if (compiled_) throw "Can't add elements once the pipeline has been compiled";
  if (elements_.empty()) throw "Can only add regular Elements after a TimedElement or BlockingElement has been added";
  stats_output_header_.append("      ->");
  stats_output_header_.append(stats_output_header);
  this->addElement(work);
}

template <class T>
void Pipeline<T>::compile() {
  if (elements_.size() > 1) {
    auto it_n_minus_1 = elements_.begin();
    auto it_n = elements_.begin();
    ++it_n;
    while (it_n != elements_.end()) {
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
  (static_cast<TimedWorkWrapper<T>*>(elements_.front().get()))->pause();
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
  start_stats_timer();
  started_ = true;
}

template <class T>
void Pipeline<T>::start_stats_timer() {
  timer_.expires_from_now(std::chrono::milliseconds(stats_interval_));
  timer_.async_wait(strand_.wrap(boost::bind(&Pipeline<T>::collect_stats, this, boost::placeholders::_1)));

  last_stats_time_ = std::chrono::high_resolution_clock::now();
}

template <class T>
void Pipeline<T>::collect_stats(const boost::system::error_code& ec) {
  // Check for some kind of error
  if (ec) {
    if (ec == boost::asio::error::operation_aborted)
      return;
    else
      throw ec.message();
  }

  auto start_time = std::chrono::high_resolution_clock::now();

  // Collect the stats
  std::chrono::time_point<std::chrono::high_resolution_clock> last_time = last_stats_time_;
  last_stats_time_ = time_stats_.front()->front().first;
  std::ostringstream output;
//  ouput << "Interval  Camera      -> Movment      ->    ROIs      ->    HAAR      -> PerfMon      -> Publish";
  output << stats_output_header_;
  for (unsigned int i = 0; i < time_stats_.back()->size(); i++) {
    last_stats_time_ = time_stats_.front()->front().first;
    output << '\n' << std::setw(8);
    for (auto stats_queue : time_stats_) {
      std::pair<std::chrono::time_point<std::chrono::high_resolution_clock>,
                      std::chrono::time_point<std::chrono::high_resolution_clock>> times = stats_queue->front();
      stats_queue->pop();
      output << std::setw(8) << std::chrono::duration_cast<
          std::chrono::microseconds>(times.first - last_time).count();
      output << std::setw(8) << std::chrono::duration_cast<
          std::chrono::microseconds>(times.second - times.first).count();
      last_time = times.second;
    }
  }

  int dur = std::chrono::duration_cast<std::chrono::microseconds>(
      std::chrono::high_resolution_clock::now() - start_time).count();
  output << "    (" << dur << ")\n";
  std::cout << output.str() << std::endl;
  // Reschedule the timer
  timer_.expires_at(timer_.expires_at() + std::chrono::milliseconds(stats_interval_));
  timer_.async_wait(strand_.wrap(boost::bind(&Pipeline<T>::collect_stats, this, boost::placeholders::_1)));
}

template <class T>
void Pipeline<T>::change_timer(const unsigned int milliseconds) {
  if (!compiled_) throw "Can't perform change_timer operation before pipeline is compiled";
  (static_cast<TimedWorkWrapper<T>*>(elements_.front().get()))->set_interval(milliseconds);
  std::cout << "changed the timer to: " << milliseconds << std::endl;
}

} // namespace pipeline


#endif // PIPELINE_PIPELINE_H_

