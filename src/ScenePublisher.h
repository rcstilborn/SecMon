/*
 * ScenePublisher.h
 *
 *  Created on: Aug 13, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#ifndef SCENEPUBLISHER_H_
#define SCENEPUBLISHER_H_

#include <boost/core/noncopyable.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/thread/pthread/mutex.hpp>

#include <string>
#include <vector>
#include <memory>
#include "Component.h"
#include "PerformanceMonitor.h"

class Frame;
class GUI_Interface;

class ScenePublisher : private boost::noncopyable, public Component{
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

  ScenePublisher(const std::string& display_name, const std::string& description, GUI_Interface& gui);
  virtual ~ScenePublisher();
  const std::string& get_description() const;
  const std::string& get_display_name() const;
  void add_stream(const std::string& name);
  void process_next_frame(std::shared_ptr<Frame>&);
  //    boost::signals2::connection connect(const image_ready_signal::slot_type
  //    &subscriber);
  //    void trigger(image_ptr image);

 private:
  const std::string display_name_;
  const std::string description_;
  GUI_Interface& gui_;
  boost::mutex streams_list_mtx_;
  std::vector<std::shared_ptr<Stream>> streams_;
  PerformanceMonitor performance_monitor_;
};

#endif  // SCENEPUBLISHER_H_
