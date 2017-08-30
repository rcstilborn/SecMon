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

#include <iostream>
#include <string>

Scene::Scene(const std::string& name, const std::string& url, boost::asio::io_service& io_service, GUI_Interface& gui,
             const int fps)
    : name_(name),
      //  interval(1000/fps),
      frame_sequence_(),  // camera.getHeight(), camera.getWidth()),
      scene_interface_(name, "Description here", gui, frame_sequence_),
      ooi_processor_(io_service, scene_interface_, frame_sequence_,
                     boost::bind(&SceneInterface::publish, &scene_interface_, _1)),
      image_processor_(io_service, scene_interface_, frame_sequence_,
                       boost::bind(&OOI_Processor::process_next_frame, &ooi_processor_, _1)),
      image_source_(name, url, io_service, frame_sequence_,
                    boost::bind(&ImageProcessor::process_next_frame, &image_processor_, _1), fps) {
  DLOG(INFO)<< "Scene(" << name << ") - constructed";
}

Scene::~Scene() {
  //    std::cout << "~Scene() - enter" << std::endl;
  //    std::cout << "~Scene() - destructed " << count << std::endl;
}

FrameSequence& Scene::get_frame_sequence() {
  return this->frame_sequence_;
}

const std::string& Scene::get_name() const {
  return this->name_;
}

void Scene::shutdown() {
  //    std::cout << "Scene::shutdown() " << name << std::endl;
  image_source_.shutdown();
}

void Scene::toggle_pause() {
  isPaused_ = !isPaused_;
  image_source_.toggle_pause();
  if (isPaused_)
    std::cout << "Pausing..." << std::endl;
  else
    std::cout << "Resuming..." << std::endl;
}

void Scene::set_frames_per_second(const int fps) {
  image_source_.set_frames_per_second(fps);
  DLOG(INFO)<< "Setting FPS to " << fps;
}
