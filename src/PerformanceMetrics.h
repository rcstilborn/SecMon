/*
 * PerformanceMetrics.h
 *
 *  Created on: Aug 4, 2015
 *      Author: richard
 */

#ifndef PERFORMANCEMETRICS_H_
#define PERFORMANCEMETRICS_H_

#include <boost/asio/deadline_timer.hpp>
#include <boost/system/error_code.hpp>
#include <stddef.h>
#include <ctime>


class PerformanceMetrics {
public:
    PerformanceMetrics(boost::asio::io_service& io_service, const int interval=5);
    virtual ~PerformanceMetrics();

private:
    int interval;
    boost::asio::deadline_timer timer;
    int numProcessors;
    clock_t lastCPU, lastSysCPU, lastUserCPU;

    size_t getPeakRSS();
    size_t getCurrentRSS();
    double getCurrentCPU();
    void getNextStats(const boost::system::error_code& ec);
    char last_indicator='/';
    char getNextIndicator();
};

#endif /* PERFORMANCEMETRICS_H_ */
