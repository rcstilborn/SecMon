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

#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>
#include <boost/core/noncopyable.hpp>
#include <boost/function/function_fwd.hpp>
#include <boost/function.hpp>
#include <boost/system/error_code.hpp>
#include <cstdbool>
#include <string>

#include "Camera.h"

class FrameSequence;
class SceneInterface;

class GUI_Interface;

class ImageSource : private boost::noncopyable {
 public:
  ImageSource(const std::string& name, const std::string& url, boost::asio::io_service& io_service,
              FrameSequence& frameSequence, boost::function<void(const int)> next,
              const int fps = 1);
  virtual ~ImageSource();

  Camera& get_camera();
  const std::string& get_name() const;
  void shutdown();
  void toggle_pause();

  void set_frames_per_second(const int fps);

 private:
  // Make sure we can't copy them
//    Scene(const Scene&);
//    Scene& operator=(Scene);

  const std::string& name_;
  int interval_;
  Camera camera_;

  FrameSequence& frame_sequence_;

  boost::function<void(const int)> next_;

  boost::asio::steady_timer timer_;
  boost::asio::strand strand_;

  bool shutting_down_;
  void get_next_frame(const boost::system::error_code& ec);

  void restart_timer();
  void start_timer();

  bool is_paused_ = false;
};

#endif // IMAGESOURCE_IMAGESOURCE_H_
