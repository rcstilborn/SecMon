/*
 * ImageProcessor.h
 *
 *  Created on: Aug 14, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#ifndef IMAGEPROCESSING_IMAGEPROCESSOR_H_
#define IMAGEPROCESSING_IMAGEPROCESSOR_H_

#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>
#include <boost/core/noncopyable.hpp>
#include <boost/function/function_fwd.hpp>
#include <boost/function.hpp>
#include <string>

#include "../Frame.h"
#include "../SceneInterface.h"
#include "ROI_Detector.h"

class MovementDetector;
class FrameSequence;

class ImageProcessor {
 public:
  ImageProcessor(boost::asio::io_service& io_service, SceneInterface& sceneIf, FrameSequence& frameSequence,
                 boost::function<void(const int)> next);
  virtual ~ImageProcessor();
  void process_next_frame(const int frameId);

 private:
  ImageProcessor(const ImageProcessor&) = delete;
  ImageProcessor& operator=(const ImageProcessor&) = delete;

  /// Strand to ensure the connection's handlers are not called concurrently.
  boost::asio::io_service::strand strand_;

  SceneInterface& scene_interface_;
  FrameSequence& frame_sequence_;

  boost::function<void(const int)> next_;

//    SceneInterface::image_ready_signal& main_view_signal;
//    SceneInterface::image_ready_signal& overlay_view_signal;
//    SceneInterface::image_ready_signal& debug_view_signal0;
//    SceneInterface::image_ready_signal& debug_view_signal1;
//    SceneInterface::image_ready_signal& debug_view_signal2;

///diff, threshold, diff, blur method
  MovementDetector* movement_detector_ = NULL;

  /// OpenCV BackgroundSubtractor
//    BackgroundSubtractorCV bg1;

///IMBS Background Subtractor
//    BackgroundSubtractorIMBS imbs;

  ROI_Detector roi_;

  void process_frame(const int frameId);
//    void signalNewDebugImage(const int signal_id, const std::string& image_name, std::shared_ptr<Frame> frame);
};

#endif // IMAGEPROCESSING_IMAGEPROCESSOR_H_
