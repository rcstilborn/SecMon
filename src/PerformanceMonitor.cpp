/*
 * PerformanceMonitor.cpp
 *
 *  Created on: Aug 4, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#include "PerformanceMonitor.h"

#include <glog/logging.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <stdio.h>
#include <iomanip>
#include <iostream>
#include <string>
#include <chrono>
#include <memory>

//#ifndef CPU_ONLY
//#include
//#endif

PerformanceMonitor::PerformanceMonitor(const int interval)
    : interval_(interval),
      number_of_processors_(0),
      last_cpu_usage_(),
      last_system_cpu_usage_(),
      last_user_cpu_usage_(),
      last_perf_time_(std::chrono::high_resolution_clock::now()) {

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
  DLOG(INFO) << "Performance Metrics - constructed.  Interval=" << this->interval_
             << "s, # processors=" << this->number_of_processors_
             << "Total memory available=" << totalPhysMem;

  std::chrono::duration<double, std::nano> ns1 = typename std::chrono::high_resolution_clock::duration(1);
  std::cout << "Resolution of high_resolution_clock: " << ns1.count() << " ns\n";

  std::chrono::duration<double, std::nano> ns2 = typename std::chrono::system_clock::duration(1);
  std::cout << "Resolution of system_clock: " << ns2.count() << " ns\n";

  DLOG(INFO)<< "Resolution of high_resolution_clock: " << ns1.count() << " ns\n";
  DLOG(INFO)<< "Resolution of system_clock: " << ns2.count() << " ns\n";

  std::cout.precision(4);

  peak_rss_mb_ = get_peak_rss_mb();
  current_rss_mb_ = get_current_rss_mb();
  cpu_usage_ = get_current_cpu_usage();
}

PerformanceMonitor::~PerformanceMonitor() {
}

const std::string PerformanceMonitor::get_performance_string() {
  if (std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - last_perf_time_).count() > interval_) {
    last_perf_time_ = std::chrono::high_resolution_clock::now();
    peak_rss_mb_ = get_peak_rss_mb();
    current_rss_mb_ = get_current_rss_mb();
    cpu_usage_ = get_current_cpu_usage();
  }
  std::string stats_text("CPU: ");
  stats_text.append(std::to_string(cpu_usage_)).append("%  Curr: ");
  stats_text.append(std::to_string(current_rss_mb_)).append("Mb  Peak: ");
  stats_text.append(std::to_string(peak_rss_mb_)).append("Mb");
  return stats_text;
}

/**
 * Returns the peak (maximum so far) resident set size (physical
 * memory use) measured in Mbytes, or zero if the value cannot be
 * determined on this OS.
 */
unsigned int PerformanceMonitor::get_peak_rss_mb() {
  struct rusage rusage;
  getrusage(RUSAGE_SELF, &rusage);
  return static_cast<unsigned int>(rusage.ru_maxrss / 1024.0);
}

/**
 * Returns the current resident set size (physical memory use) measured
 * in Mbytes, or zero if the value cannot be determined on this OS.
 */
unsigned int PerformanceMonitor::get_current_rss_mb() {
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
  return static_cast<unsigned int>(rss * (size_t) sysconf( _SC_PAGESIZE) / 1048576);
}

unsigned int PerformanceMonitor::get_current_cpu_usage() {
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

  return static_cast<int>(percent);
}
