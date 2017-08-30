/*
 * FirstElementTimed.h
 *
 *  Created on: Aug 27, 2017
 *      Author: vagrant
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#ifndef PIPELINE_TIMEDWORKWRAPPER_H_
#define PIPELINE_TIMEDWORKWRAPPER_H_

#include "WorkWrapper.h"
#include <boost/function.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>
#include <boost/any.hpp>

#include <stdexcept>
#include <memory>

namespace pipeline {

class TimedWorkWrapper : public WorkWrapper {
 public:
  TimedWorkWrapper() = delete;
  explicit TimedWorkWrapper(boost::asio::io_service& io_service,
                            boost::function<void(std::shared_ptr<boost::any>)> work,
                            const unsigned int milliseconds);
  virtual ~TimedWorkWrapper();

  virtual void schedule_work(std::shared_ptr<boost::any> data);
  void pause();

 private:
  unsigned int milliseconds_;
  boost::asio::steady_timer timer_;
  void do_work(const boost::system::error_code& ec);
  void start_timer();
  void restart_timer();
  bool paused_ = false;
};
} // namespace pipeline


#endif // PIPELINE_TIMEDWORKWRAPPER_H_
