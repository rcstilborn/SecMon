/*
 * WorkWrapper_Test.cpp
 *
 *  Created on: Aug 27, 2017
 *      Author: Richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#include "WorkWrapper.h"
#include <gtest/gtest.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <memory>

void test_function(const int i) {
  if (i < 0)
    return;
}

TEST(WorkWrapperConstruct, Success) {
  boost::asio::io_service io_service;
  std::shared_ptr<pipeline::TimeStatsQueue> tsq(new pipeline::TimeStatsQueue());
  try {
    pipeline::WorkWrapper<const int> ww(io_service, boost::bind(test_function, 1), tsq);
  } catch (const char* msg) {
    FAIL()<< msg;
  }
}

