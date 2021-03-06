/*
 * Scene.h
 *
 *  Created on: Jul 28, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#ifndef SCENE_H_
#define SCENE_H_

#include <boost/asio/io_service.hpp>
#include <boost/any.hpp>
#include <cstdbool>
#include <string>
#include <memory>
#include <vector>
#include "Pipeline/Pipeline.h"
#include "Component.h"

class GUI_Interface;

class Scene {
 public:
  Scene(const std::string& name, const std::string& url, boost::asio::io_service& io_service, GUI_Interface& gui,
        const double realtime_factor);
  virtual ~Scene();
  const std::string& get_name() const;
  void shutdown();
  void toggle_pause();
  void set_realtime_factor(const double realtime_factor);

 private:
  const std::string& name_;
  std::vector<std::shared_ptr<Component>> components_;
  pipeline::Pipeline<Frame> pipeline_;
  bool isPaused_ = false;
  double realtime_factor_ = 1.0;
  int source_fps_ = 0;
  // Make sure we can't copy them
  Scene(const Scene&) = delete;
  Scene& operator=(Scene) = delete;
};

#endif  // SCENE_H_
