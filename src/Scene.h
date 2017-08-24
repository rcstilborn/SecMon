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

#include <boost/core/noncopyable.hpp>
#include <cstdbool>
#include <string>

#include "FrameSequence.h"
#include "ImageProcessing/ImageProcessor.h"
#include "ImageSource/ImageSource.h"
#include "OOI_Processing/OOI_Processor.h"
#include "SceneInterface.h"

class GUI_Interface;

class Scene : private boost::noncopyable {
 public:
  Scene(const std::string& name, const std::string& url, boost::asio::io_service& io_service, GUI_Interface& gui,
        const int fps = 1);
  virtual ~Scene();

  Camera& get_camera();
  FrameSequence& get_frame_sequence();
  const std::string& get_name() const;
  void shutdown();
  void toggle_pause();
  void set_frames_per_second(const int fps);

 private:
  const std::string& name_;
  //    int interval;
  FrameSequence frame_sequence_;
  SceneInterface scene_interface_;
  OOI_Processor ooi_processor_;
  ImageProcessor image_processor_;
  ImageSource image_source_;

  bool isPaused_ = false;
};

#endif  // SCENE_H_
