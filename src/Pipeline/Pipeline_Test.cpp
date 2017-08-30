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
#include <memory>

void pipeline_test_function(const int i) {
  if (i < 0)
    return;
}

TEST(PipelineConstructor, Success) {
  boost::asio::io_service io_service;
  pipeline::Pipeline p(io_service);
}

TEST(PipelineAddTimedElement, Success) {
  boost::asio::io_service io_service;
  pipeline::Pipeline p(io_service);
  try {
    p.add(pipeline::TimedElement(pipeline::TimerType::Interval, 10, boost::bind(pipeline_test_function, 1)));
  } catch (const char* msg) {
    FAIL()<< msg;
  }
}

TEST(PipelineAddTimedElement, NotFirstElement) {
  boost::asio::io_service io_service;
  pipeline::Pipeline p(io_service);
  p.add(pipeline::TimedElement(pipeline::TimerType::Interval, 10, boost::bind(pipeline_test_function, 1)));
  ASSERT_ANY_THROW(p.add(pipeline::TimedElement
          (pipeline::TimerType::Interval, 10, boost::bind(pipeline_test_function, 1))))<< "Did not throw!";
}

TEST(PipelineAddElement, Success) {
  boost::asio::io_service io_service;
  pipeline::Pipeline p(io_service);
  p.add(pipeline::TimedElement(pipeline::TimerType::Interval, 10, boost::bind(pipeline_test_function, 1)));
  try {
    p.add(pipeline::Element(boost::bind(pipeline_test_function, 1)));
  } catch (const char* msg) {
    FAIL()<< msg;
  }
}

TEST(PipelineAddElement, FirstElement) {
  boost::asio::io_service io_service;
  pipeline::Pipeline p(io_service);
  ASSERT_ANY_THROW(p.add(pipeline::Element(boost::bind(pipeline_test_function, 1))))<< "Did not throw!";
}

TEST(PipelineCompile, SuccessOneElement) {
  try {
    boost::asio::io_service io_service;
    pipeline::Pipeline p(io_service);
    p.add(pipeline::TimedElement(pipeline::TimerType::Interval, 10, boost::bind(pipeline_test_function, 1)));
    p.compile();
  } catch (const char* msg) {
    FAIL()<< msg;
  }
}

TEST(PipelineCompile, SuccessTwoElements) {
  try {
    boost::asio::io_service io_service;
    pipeline::Pipeline p(io_service);
    p.add(pipeline::TimedElement(pipeline::TimerType::Interval, 10, boost::bind(pipeline_test_function, 1)));
    p.add(pipeline::Element(boost::bind(pipeline_test_function, 1)));
    p.compile();
  } catch (const char* msg) {
    FAIL()<< msg;
  }
}

TEST(PipelineCompile, SuccessThreeElements) {
  try {
    boost::asio::io_service io_service;
    pipeline::Pipeline p(io_service);
    p.add(pipeline::TimedElement(pipeline::TimerType::Interval, 10, boost::bind(pipeline_test_function, 1)));
    p.add(pipeline::Element(boost::bind(pipeline_test_function, 1)));
    p.add(pipeline::Element(boost::bind(pipeline_test_function, 1)));
    p.compile();
  } catch (const char* msg) {
    FAIL()<< msg;
  }
}
