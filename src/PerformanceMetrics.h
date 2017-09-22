/*
 * PerformanceMetrics.h
 *
 *  Created on: Aug 4, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#ifndef PERFORMANCEMETRICS_H_
#define PERFORMANCEMETRICS_H_

#include <stddef.h>
#include <boost/asio/steady_timer.hpp>
#include <boost/system/error_code.hpp>
#include <ctime>

class PerformanceMetrics {
 public:
  explicit PerformanceMetrics(boost::asio::io_service& io_service, const int interval = 5);
  virtual ~PerformanceMetrics();

 private:
  int interval_;
  boost::asio::steady_timer timer_;
  int number_of_processors_;
  clock_t last_cpu_usage_, last_system_cpu_usage_, last_user_cpu_usage_;

  size_t get_peak_rss();
  size_t get_current_rss();
  double get_current_cpu_usage();
  void get_next_stats(const boost::system::error_code& ec);
  char last_indicator_ = '/';
  char get_next_indicator();
};

#endif /* PERFORMANCEMETRICS_H_ */
