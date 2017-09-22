/*
 * Camera.h
 *
 *  Created on: Jul 27, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#ifndef IMAGESOURCE_CAMERA_H_
#define IMAGESOURCE_CAMERA_H_

#include <opencv2/videoio.hpp>
#include <string>
#include <memory>

class Scene;
class Frame;

class Camera {
 public:
  explicit Camera(const std::string& source);
  virtual ~Camera();
  bool get_next_frame(std::shared_ptr<Frame>& frame);
  const std::string& get_source_name() const;
  int get_frames_per_second() const;
  unsigned int get_width() const;
  unsigned int get_height() const;
  void restart();

 private:
  const std::string source_;
  cv::VideoCapture camera_;
  int fps_ = 0;
  unsigned int width_ = 0;
  unsigned int height_ = 0;
  bool real_camera_ = false;
};

#endif // IMAGESOURCE_CAMERA_H_
