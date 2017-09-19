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
//#include "ImageProcessing/ImageProcessor.h"
#include "ImageProcessing/MovementDetectorBasic.h"
#include "ImageProcessing/ROI_Detector.h"
#include "ImageSource/ImageSource.h"
#include "ObjectProcessing/HaarClassifier.h"
#include "ObjectProcessing/OOI_Processor.h"
#include "Pipeline/Pipeline.h"
#include "Component.h"
#include "ScenePublisher.h"

Scene::Scene(const std::string& name, const std::string& url,
             boost::asio::io_service& io_service, GUI_Interface& gui,
             const double realtime_factor)
    : name_(name),
      components_(),
      pipeline_(io_service, 5000) {

  // Construct the image source and add it to the pipeline
  std::shared_ptr<ImageSource> is_ptr(new ImageSource(url));
  source_fps_ = is_ptr->get_frames_per_second();
  pipeline_.addTimedElement(pipeline::TimerType::Interval, 1000/(source_fps_*realtime_factor),
                            boost::bind(&ImageSource::process_next_frame, is_ptr, boost::placeholders::_1),
                            "  Camera");
  components_.push_back(is_ptr);

  // Construct the movement detector and add it to the pipeline
  std::shared_ptr<MovementDetectorBasic> mdb_ptr(new MovementDetectorBasic());
  pipeline_.addElement(boost::bind(&MovementDetectorBasic::process_next_frame, mdb_ptr, boost::placeholders::_1),
                       " Movemnt");
  components_.push_back(mdb_ptr);

  // Construct the roi processing and add it to the pipeline
  std::shared_ptr<ROI_Detector> rd_ptr(new ROI_Detector());
  pipeline_.addElement(boost::bind(&ROI_Detector::process_next_frame, rd_ptr, boost::placeholders::_1),
                       "    ROIs");
  components_.push_back(rd_ptr);

  // Construct the HaarClassifier and add it to the pipeline
//  std::shared_ptr<HaarClassifier> hc_ptr(new HaarClassifier());
//  pipeline_.addElement(boost::bind(&HaarClassifier::process_next_frame, hc_ptr, boost::placeholders::_1),
//                       "    Haar");
//  components_.push_back(hc_ptr);

  // Construct the publisher and add it to the pipeline
  std::shared_ptr<ScenePublisher> sp_ptr(new ScenePublisher(name, "Description here", gui));
  pipeline_.addElement(boost::bind(&ScenePublisher::process_next_frame, sp_ptr, boost::placeholders::_1), " Publish");
  components_.push_back(sp_ptr);

  pipeline_.compile();
  pipeline_.start();

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
