/*
 * PerformanceMetrics.cpp
 *
 *  Created on: Aug 4, 2015
 *      Author: richard
 */

#include "PerformanceMetrics.h"

#include <boost/asio/basic_deadline_timer.hpp>
#include <boost/asio/error.hpp>
#include <boost/bind/arg.hpp>
#include <boost/bind/bind.hpp>
#include <boost/bind/placeholders.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/date_time/time.hpp>
#include <boost/date_time/time_duration.hpp>
#include <stdio.h>
#include <iomanip>
#include <string.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <iostream>
#include <glog/logging.h>

using namespace boost::placeholders;

//#ifndef CPU_ONLY
//#include
//#endif


PerformanceMetrics::PerformanceMetrics(boost::asio::io_service& io_service, const int interval)
: interval(interval),
  timer(io_service, boost::posix_time::milliseconds(interval)),
  numProcessors(0) {

	this->timer.expires_from_now(boost::posix_time::seconds(this->interval));
	this->timer.async_wait(boost::bind(&PerformanceMetrics::getNextStats, this, _1));

	FILE* file;
	struct tms timeSample;
	char line[128];


	lastCPU = times(&timeSample);
	lastSysCPU = timeSample.tms_stime;
	lastUserCPU = timeSample.tms_utime;

    struct sysinfo memInfo;
    sysinfo (&memInfo);

    long long totalPhysMem = memInfo.totalram;
    //Multiply in next statement to avoid int overflow on right hand side...
    totalPhysMem *= memInfo.mem_unit;
    totalPhysMem /=(1024.0*1024.0);

	file = fopen("/proc/cpuinfo", "r");
	while(fgets(line, 128, file) != NULL){
		if (strncmp(line, "processor", 9) == 0) numProcessors++;
	}
	fclose(file);
	std::cout << "Performance Metrics - constructed.  Interval=" << this->interval
	                                       << "s, # processors=" << this->numProcessors
	                                       << "Total memory available=" << totalPhysMem << std::endl;
    DLOG(INFO) << "Performance Metrics - constructed.  Interval=" << this->interval
	                                       << "s, # processors=" << this->numProcessors
	                                       << "Total memory available=" << totalPhysMem;
	std::cout.precision(4);
}

PerformanceMetrics::~PerformanceMetrics() {
	// TODO Auto-generated destructor stub
}


void PerformanceMetrics::getNextStats(const boost::system::error_code& ec) {

	// Check for errors.
	if(ec == boost::asio::error::operation_aborted) {
		std::cout << "PerformanceMetrics::getNextStats() - timer cancelled" << std::endl;
		return;
	}

	std::cout << "\rCurrent=" << std::setw(5) << (int)(this->getCurrentRSS()/1024)/1024.0
			<< "Mb, Peak=" << std::setw(5) << (int)(this->getPeakRSS()/1024)/1024.0
			<< "Mb, CPU usage=" << std::setw(4) << this->getCurrentCPU() << "%     "
			<< getNextIndicator() << "     " << std::flush;
	// Reset the timer
	this->timer.expires_at(this->timer.expires_at() + boost::posix_time::seconds(this->interval));
	this->timer.async_wait(boost::bind(&PerformanceMetrics::getNextStats, this, _1));
}


/**
 * Returns the peak (maximum so far) resident set size (physical
 * memory use) measured in bytes, or zero if the value cannot be
 * determined on this OS.
 */
size_t PerformanceMetrics::getPeakRSS()
{
	struct rusage rusage;
	getrusage( RUSAGE_SELF, &rusage );
	return (size_t)(rusage.ru_maxrss * 1024L);
}



/**
 * Returns the current resident set size (physical memory use) measured
 * in bytes, or zero if the value cannot be determined on this OS.
 */
size_t PerformanceMetrics::getCurrentRSS()
{
	/* Linux ---------------------------------------------------- */
	long rss = 0L;
	FILE* fp = NULL;
	if ( (fp = fopen( "/proc/self/statm", "r" )) == NULL )
		return (size_t)0L;      /* Can't open? */
	if ( fscanf( fp, "%*s%ld", &rss ) != 1 )
	{
		fclose( fp );
		return (size_t)0L;      /* Can't read? */
	}
	fclose( fp );
	return (size_t)rss * (size_t)sysconf( _SC_PAGESIZE);
}



double PerformanceMetrics::getCurrentCPU(){
	struct tms timeSample;
	clock_t now;
	double percent;


	now = times(&timeSample);
	if (now <= lastCPU || timeSample.tms_stime < lastSysCPU ||
			timeSample.tms_utime < lastUserCPU){
		//Overflow detection. Just skip this value.
		percent = -1.0;
	}
	else{
		percent = (timeSample.tms_stime - lastSysCPU) +
				(timeSample.tms_utime - lastUserCPU);
		percent /= (now - lastCPU);
		percent /= numProcessors;
		percent *= 100;
	}
	lastCPU = now;
	lastSysCPU = timeSample.tms_stime;
	lastUserCPU = timeSample.tms_utime;


	return percent;
}

char PerformanceMetrics::getNextIndicator() {
    if      (last_indicator == '/') last_indicator = '\\';
    else if (last_indicator == '\\') last_indicator = '|';
    else if (last_indicator == '|') last_indicator = '/';
    return last_indicator;
}
