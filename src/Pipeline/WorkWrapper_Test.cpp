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

struct foo {
  explicit foo(int ii)
  : i(ii) {
  }
  int i;
};

void test_function(std::shared_ptr<foo>& data) {
  if (data->i < 0)
    return;
}

//TEST(WorkWrapperConstruct, Success) {
//  boost::asio::io_service io_service;
//  std::shared_ptr<pipeline::TimeStatsQueue> tsq(new pipeline::TimeStatsQueue());
//  try {
//    pipeline::WorkWrapper<foo> ww(io_service, boost::bind(test_function, 1), tsq);
//  } catch (const char* msg) {
//    FAIL()<< msg;
//  }
//}
//
//TEST(WorkWrapperConstruct, SuccessNoStats) {
//  boost::asio::io_service io_service;
////  std::shared_ptr<pipeline::TimeStatsQueue> tsq();
//  try {
//    pipeline::WorkWrapper<foo> ww(io_service, boost::bind(test_function, 1), nullptr);
//  } catch (const char* msg) {
//    FAIL()<< msg;
//  }
//}
//
//TEST(WorkWrapperDoWork, NoNextNoStats) {
//  boost::asio::io_service io_service;
//  try {
//    auto data = std::make_shared<foo>(1);
//    pipeline::WorkWrapper<foo> ww(io_service, boost::bind(test_function, 1), nullptr);
//    ww.do_work(data);
//  } catch (const char* msg) {
//    FAIL()<< msg;
//  }
//}
