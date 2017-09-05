/*
 * Scene.cpp
 *
 *  Created on: Jul 28, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#include "Scene.h"

#include <glog/logging.h>
#include <boost/bind/arg.hpp>
#include <boost/bind/bind.hpp>
#include <boost/bind/placeholders.hpp>
#include <boost/any.hpp>
#include <iostream>
#include <string>
#include <memory>
#include <utility>
#include "ImageProcessing/ImageProcessor.h"
#include "ImageSource/ImageSource.h"
#include "ObjectProcessing/OOI_Processor.h"
#include "Pipeline/Pipeline.h"
#include "Component.h"
#include "ScenePublisher.h"

Scene::Scene(const std::string& name, const std::string& url,
             boost::asio::io_service& io_service, GUI_Interface& gui,
             const double realtime_factor)
    : name_(name),
      components_(),
      pipeline_(io_service) {

  // Construct the image source and add it to the pipeline
  std::shared_ptr<ImageSource> is_ptr(new ImageSource(url));
  source_fps_ = is_ptr->get_frames_per_second();
  pipeline_.addTimedElement(pipeline::TimerType::Interval, 1000/(source_fps_*realtime_factor),
                            boost::bind(&ImageSource::process_next_frame, is_ptr, boost::placeholders::_1));
  components_.push_back(is_ptr);

  // Construct the image processor and add it to the pipeline
  std::shared_ptr<ImageProcessor> ip_ptr(new ImageProcessor());
  pipeline_.addElement(boost::bind(&ImageProcessor::process_next_frame, ip_ptr, boost::placeholders::_1));
  components_.push_back(ip_ptr);

  // Construct the publisher and add it to the pipeline
  std::shared_ptr<ScenePublisher> sp_ptr(new ScenePublisher(name, "Description here", gui));
  pipeline_.addElement(boost::bind(&ScenePublisher::process_next_frame, sp_ptr, boost::placeholders::_1));
  components_.push_back(sp_ptr);

  pipeline_.compile();

  pipeline_.start();
//  components_.push_back(std::unique_ptr<WorkWrapper>(
//      new OOI_Processor ooi_processor(scene_interface_, frame_sequence_));
//  components_.push_back(std::unique_ptr<WorkWrapper>(
//      new ImageProcessor image_processor_(scene_interface_, frame_sequence_));
    DLOG(INFO)<< "Scene(" << name << ", " << url << ", " << source_fps_ << "fps, * "
              << realtime_factor << " factor) - constructed";
  }

  Scene::~Scene() {
    components_.clear();
  }

const std::string& Scene::get_name() const {
  return this->name_;
}

void Scene::shutdown() {
  pipeline_.pause();
}

void Scene::toggle_pause() {
  if (!isPaused_)
    DLOG(INFO) << "Pausing...";
  else
    DLOG(INFO) << "Resuming...";
  pipeline_.pause();
  isPaused_ = !isPaused_;
}

void Scene::set_realtime_factor(const double realtime_factor) {
  pipeline_.change_timer(1000/(source_fps_*realtime_factor));
  DLOG(INFO)<< "Setting realtime_factor to " << realtime_factor;
}
