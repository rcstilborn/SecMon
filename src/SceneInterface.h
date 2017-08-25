/*
 * SceneInterface.h
 *
 *  Created on: Aug 13, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#ifndef SCENEINTERFACE_H_
#define SCENEINTERFACE_H_

#include <boost/core/noncopyable.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/thread/pthread/mutex.hpp>

#include <string>
#include <vector>
#include <memory>

class FrameSequence;
class GUI_Interface;

class SceneInterface : private boost::noncopyable {
 public:
  typedef std::shared_ptr<std::vector<unsigned char>> image_ptr;
  typedef boost::signals2::signal<void(image_ptr)> image_ready_signal;
  struct Stream {
    explicit Stream(const std::string& name)
        : name(name),
          image_ready() {
    }
    const std::string name;
    image_ready_signal image_ready;
  };

  SceneInterface(const std::string& display_name, const std::string& description, GUI_Interface& gui,
                 FrameSequence& frame_sequence);
  virtual ~SceneInterface();
  const std::string& get_description() const;
  const std::string& get_display_name() const;
  void add_stream(const std::string& name);
  void publish(const int frameid);
  //    boost::signals2::connection connect(const image_ready_signal::slot_type
  //    &subscriber);
  //    void trigger(image_ptr image);

 private:
  const std::string display_name_;
  const std::string description_;
  GUI_Interface& gui_;
  FrameSequence& frame_sequence_;
  boost::mutex streams_list_mtx_;
  std::vector<std::shared_ptr<Stream>> streams_;
};

#endif  // SCENEINTERFACE_H_
