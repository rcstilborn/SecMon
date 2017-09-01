/*
 * Pipeline_Test.cpp
 *
 *  Created on: Aug 27, 2017
 *      Author: Richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#include "Pipeline.h"
#include <gtest/gtest.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/bind/placeholders.hpp>
#include <memory>

struct foo {
};
void pipeline_test_function(std::shared_ptr<foo> i) {
if (i < 0)
  return;
}

TEST(PipelineConstructor, Success) {
  boost::asio::io_service io_service;
  pipeline::Pipeline<foo> p(io_service);
}

TEST(PipelineAddTimedElement, Success) {
  boost::asio::io_service io_service;
  pipeline::Pipeline<foo> p(io_service);
  try {
    p.addTimedElement(pipeline::TimerType::Interval, 10, boost::bind(pipeline_test_function, boost::placeholders::_1));
  } catch (const char* msg) {
    FAIL()<< msg;
  }
}

TEST(PipelineAddTimedElement, NotFirstElement) {
  boost::asio::io_service io_service;
  pipeline::Pipeline<foo> p(io_service);
  p.addTimedElement(pipeline::TimerType::Interval, 10, boost::bind(pipeline_test_function, boost::placeholders::_1));
  ASSERT_ANY_THROW(p.addTimedElement(pipeline::TimerType::Interval, 10,
                                     boost::bind(pipeline_test_function, boost::placeholders::_1)))<< "Did not throw!";
}

TEST(PipelineAddElement, Success) {
  boost::asio::io_service io_service;
  pipeline::Pipeline<foo> p(io_service);
  p.addTimedElement(pipeline::TimerType::Interval, 10, boost::bind(pipeline_test_function, boost::placeholders::_1));
  try {
    p.addElement(boost::bind(pipeline_test_function, boost::placeholders::_1));
  } catch (const char* msg) {
    FAIL()<< msg;
  }
}

TEST(PipelineAddElement, FirstElement) {
  boost::asio::io_service io_service;
  pipeline::Pipeline<foo> p(io_service);
  ASSERT_ANY_THROW(p.addElement(boost::bind(pipeline_test_function, boost::placeholders::_1)))<< "Did not throw!";
}

TEST(PipelineCompile, SuccessOneElement) {
  try {
    boost::asio::io_service io_service;
    pipeline::Pipeline<foo> p(io_service);
    p.addTimedElement(pipeline::TimerType::Interval, 10, boost::bind(pipeline_test_function, boost::placeholders::_1));
    p.compile();
  } catch (const char* msg) {
    FAIL()<< msg;
  }
}

TEST(PipelineCompile, SuccessTwoElements) {
  try {
    boost::asio::io_service io_service;
    pipeline::Pipeline<foo> p(io_service);
    p.addTimedElement(pipeline::TimerType::Interval, 10, boost::bind(pipeline_test_function, boost::placeholders::_1));
    p.addElement(boost::bind(pipeline_test_function, boost::placeholders::_1));
    p.compile();
  } catch (const char* msg) {
    FAIL()<< msg;
  }
}

TEST(PipelineCompile, SuccessThreeElements) {
  try {
    boost::asio::io_service io_service;
    pipeline::Pipeline<foo> p(io_service);
    p.addTimedElement(pipeline::TimerType::Interval, 10, boost::bind(pipeline_test_function, boost::placeholders::_1));
    p.addElement(boost::bind(pipeline_test_function, boost::placeholders::_1));
    p.addElement(boost::bind(pipeline_test_function, boost::placeholders::_1));
    p.compile();
  } catch (const char* msg) {
    FAIL()<< msg;
  }
}
