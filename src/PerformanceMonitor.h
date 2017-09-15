/*
 * PerformanceMonitor.h
 *
 *  Created on: Aug 4, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#ifndef PERFORMANCEMONITOR_H_
#define PERFORMANCEMONITOR_H_

#include <stddef.h>
#include <ctime>
#include <memory>
#include <chrono>
#include <string>

class PerformanceMonitor {
 public:
  explicit PerformanceMonitor(const int interval = 1000);
  virtual ~PerformanceMonitor();
  const std::string get_performance_string();

 private:
  const int interval_ = 5000;
  int number_of_processors_;
  clock_t last_cpu_usage_, last_system_cpu_usage_, last_user_cpu_usage_;
  std::chrono::time_point<std::chrono::high_resolution_clock> last_perf_time_;
  unsigned int peak_rss_mb_ = 0;
  unsigned int current_rss_mb_ = 0;
  unsigned int cpu_usage_ = 0.0;
  unsigned int get_peak_rss_mb();
  unsigned int get_current_rss_mb();
  unsigned int get_current_cpu_usage();
};

#endif // PERFORMANCEMONITOR_H_
