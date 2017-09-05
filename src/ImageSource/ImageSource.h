/*
 * ImageSource.h
 *
 *  Created on: Oct 23, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#ifndef IMAGESOURCE_IMAGESOURCE_H_
#define IMAGESOURCE_IMAGESOURCE_H_

#include <memory>
#include <string>

#include "Camera.h"
#include "../Component.h"

class Frame;
class ScenePublisher;

class GUI_Interface;

class ImageSource : public Component {
 public:
  explicit ImageSource(const std::string& url);
  virtual ~ImageSource();
  void process_next_frame(std::shared_ptr<Frame>&);
  int get_frames_per_second() const { return camera_.get_frames_per_second(); }

 private:
  // Make sure we can't copy them
  ImageSource(const ImageSource&);
  ImageSource& operator=(ImageSource);

  Camera camera_;
  const unsigned int width_;
  const unsigned int height_;
};

#endif // IMAGESOURCE_IMAGESOURCE_H_
