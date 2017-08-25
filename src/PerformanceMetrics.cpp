/*
 * PerformanceMetrics.cpp
 *
 *  Created on: Aug 4, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#include "PerformanceMetrics.h"

#include <glog/logging.h>
#include <boost/asio/error.hpp>
#include <boost/bind/arg.hpp>
#include <boost/bind/bind.hpp>
#include <boost/bind/placeholders.hpp>

#include <string.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <stdio.h>
#include <iomanip>
#include <iostream>
#include <chrono>

//#ifndef CPU_ONLY
//#include
//#endif

PerformanceMetrics::PerformanceMetrics(boost::asio::io_service& io_service, const int interval)
    : interval_(interval),
      timer_(io_service, std::chrono::milliseconds(interval)),
      number_of_processors_(0),
      last_cpu_usage_(),
      last_system_cpu_usage_(),
      last_user_cpu_usage_() {

  this->timer_.expires_from_now(std::chrono::seconds(this->interval_));
  this->timer_.async_wait(boost::bind(&PerformanceMetrics::get_next_stats, this, boost::placeholders::_1));

  FILE* file;
  struct tms timeSample;
  char line[128];

  last_cpu_usage_ = times(&timeSample);
  last_system_cpu_usage_ = timeSample.tms_stime;
  last_user_cpu_usage_ = timeSample.tms_utime;

  struct sysinfo memInfo;
  sysinfo(&memInfo);

  uint64_t totalPhysMem = memInfo.totalram;
  //Multiply in next statement to avoid int overflow on right hand side...
  totalPhysMem *= memInfo.mem_unit;
  totalPhysMem /= (1024.0 * 1024.0);

  file = fopen("/proc/cpuinfo", "r");
  while (fgets(line, 128, file) != NULL) {
    if (strncmp(line, "processor", 9) == 0)
      number_of_processors_++;
  }
  fclose(file);
  std::cout << "Performance Metrics - constructed.  Interval=" << this->interval_ << "s, # processors="
            << this->number_of_processors_ << "Total memory available=" << totalPhysMem << std::endl;
  std::cout << std::chrono::high_resolution_clock::period::den << " ticks per second" << std::endl;
  DLOG(INFO) << "Performance Metrics - constructed.  Interval=" << this->interval_
             << "s, # processors=" << this->number_of_processors_
             << "Total memory available=" << totalPhysMem;
  std::cout.precision(4);
}

PerformanceMetrics::~PerformanceMetrics() {
}

void PerformanceMetrics::get_next_stats(const boost::system::error_code& ec) {
  // Check for errors.
  if (ec == boost::asio::error::operation_aborted) {
    std::cout << "PerformanceMetrics::getNextStats() - timer cancelled" << std::endl;
    return;
  }

  std::cout << "\rCurrent=" << std::setw(5) << static_cast<int>(this->get_current_rss() / 1024) / 1024.0 << "Mb, Peak="
            << std::setw(5) << static_cast<int>(this->get_peak_rss() / 1024) / 1024.0 << "Mb, CPU usage="
            << std::setw(4) << this->get_current_cpu_usage() << "%     " << get_next_indicator() << "     "
            << std::flush;
  // Reset the timer
  this->timer_.expires_at(this->timer_.expires_at() + std::chrono::seconds(this->interval_));
  this->timer_.async_wait(boost::bind(&PerformanceMetrics::get_next_stats, this, boost::placeholders::_1));
}

/**
 * Returns the peak (maximum so far) resident set size (physical
 * memory use) measured in bytes, or zero if the value cannot be
 * determined on this OS.
 */
size_t PerformanceMetrics::get_peak_rss() {
  struct rusage rusage;
  getrusage(RUSAGE_SELF, &rusage);
  return (size_t) (rusage.ru_maxrss * 1024L);
}

/**
 * Returns the current resident set size (physical memory use) measured
 * in bytes, or zero if the value cannot be determined on this OS.
 */
size_t PerformanceMetrics::get_current_rss() {
  /* Linux ---------------------------------------------------- */
  uint64_t rss = 0L;
  FILE* fp = NULL;
  if ((fp = fopen("/proc/self/statm", "r")) == NULL)
    return (size_t) 0L; /* Can't open? */
  if (fscanf(fp, "%*s%ld", &rss) != 1) {
    fclose(fp);
    return (size_t) 0L; /* Can't read? */
  }
  fclose(fp);
  return (size_t) rss * (size_t) sysconf( _SC_PAGESIZE);
}

double PerformanceMetrics::get_current_cpu_usage() {
  struct tms timeSample;
  clock_t now;
  double percent;

  now = times(&timeSample);
  if (now <= last_cpu_usage_ || timeSample.tms_stime < last_system_cpu_usage_
      || timeSample.tms_utime < last_user_cpu_usage_) {
    //Overflow detection. Just skip this value.
    percent = -1.0;
  } else {
    percent = (timeSample.tms_stime - last_system_cpu_usage_) + (timeSample.tms_utime - last_user_cpu_usage_);
    percent /= (now - last_cpu_usage_);
    percent /= number_of_processors_;
    percent *= 100;
  }
  last_cpu_usage_ = now;
  last_system_cpu_usage_ = timeSample.tms_stime;
  last_user_cpu_usage_ = timeSample.tms_utime;

  return percent;
}

char PerformanceMetrics::get_next_indicator() {
  if (last_indicator_ == '/')
    last_indicator_ = '\\';
  else if (last_indicator_ == '\\')
    last_indicator_ = '|';
  else if (last_indicator_ == '|')
    last_indicator_ = '/';
  return last_indicator_;
}
