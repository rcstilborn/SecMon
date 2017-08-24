/*
 * SceneMonitor.h
 *
 *  Created on: Jul 27, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#ifndef SCENEMONITOR_H_
#define SCENEMONITOR_H_

#include <boost/core/noncopyable.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/thread/pthread/mutex.hpp>
#include <string>

#include "Scene.h"

class Scene;

// struct scene_details {
//    std::string name;
//    unsigned short int height;
//    unsigned short int width;
//};

class SceneMonitor : private boost::noncopyable {
 public:
  SceneMonitor(boost::asio::io_service& io_service, GUI_Interface& gui);
  virtual ~SceneMonitor();
  void start_monitoring(const std::string& name, const std::string& url);
  void stop_monitoring(const std::string&);
  void stop_all_monitoring();
  const std::string get_scene_names() const;
  //    std::vector<scene_details> getSceneDetails();
  void toggle_pause();
  void set_frames_per_second(const int fps);

 private:
  boost::asio::io_service& io_service_;
  GUI_Interface& gui_;
  boost::ptr_map<std::string, Scene> scenes_;
  boost::mutex scenes_mtx_;
};

#endif  // SCENEMONITOR_H_
