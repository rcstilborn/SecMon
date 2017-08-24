/*
 * ImageProcessor.cpp
 *
 *  Created on: Aug 14, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#include "ImageProcessor.h"

#include <boost/bind/bind.hpp>
#include <boost/signals2/signal.hpp>
#include <glog/logging.h>
#include <exception>
#include <iostream>

#include "../FrameSequence.h"
#include "MovementDetectorBasic.h"
//#include "MovementDetectorViBe.h"

ImageProcessor::ImageProcessor(boost::asio::io_service& io_service, SceneInterface& sceneIf,
                               FrameSequence& frameSequence, boost::function<void(const int)> next)
    : strand_(io_service),
      scene_interface_(sceneIf),
      frame_sequence_(frameSequence),
      next_(next),
      roi_() {
//  main_view_signal(sceneIf.addStream("Main view")),
//  main_view_signal(sceneIf.addStream("Overlay")),
//  overlay_view_signal(sceneIf.addStream("Overlay")),
//  debug_view_signal0(sceneIf.addStream("Debug 0"))//,
//  debug_view_signal1(sceneIf.addStream("Debug 1")),
//  debug_view_signal2(sceneIf.addStream("Debug 2"))
//  imbs(10)
  movement_detector_ = new MovementDetectorBasic;
//    movementDetector = new MovementDetectorViBe;
#ifdef DEBUG
  scene_interface_.add_stream("foreground");
#endif // DEBUG
}

ImageProcessor::~ImageProcessor() {
  delete movement_detector_;
}

void ImageProcessor::process_next_frame(const int frameId) {
  strand_.post(boost::bind(&ImageProcessor::process_frame, this, frameId));
}

void ImageProcessor::process_frame(const int frameId) {
  //    std::cout << "ImageProcessor::processFrame id=" << frameId << std::endl;
  boost::shared_ptr<Frame> frame0;
  try {
    frame0 = frame_sequence_.get_frame(frameId);
  } catch (std::exception& e) {
    LOG(WARNING)<< "ImageProcessor::processFrame - exception getting frame " << frameId << ". " << e.what();
    return;
  }

  frame0->set_start_time();

  movement_detector_->process_frame(frame0);
//        bg1.subtractBackground(frame0);
  //    imbs.apply(frame0->getOriginalImage(),frame0->getNewImage("imbs"));

  if (frameId < 10)
    return;

  roi_.processFrame(frame0);

//    if(main_view_signal.num_slots() > 0) {
//        main_view_signal(frame0->getOriginalImageAsJPG());
//        main_view_signal(frame0->getOriginalWithOverlayImageAsJPG());
//    }

//    if(overlay_view_signal.num_slots() > 0) {
//        overlay_view_signal(frame0->getOriginalWithOverlayImageAsJPG());
//    }
//    signalNewDebugImage(0, "mog2", frame0);
//    signalNewDebugImage(0, "difference", frame0);
//    signalNewDebugImage(0, "foreground", frame0);
//    signalNewDebugImage(1, "contours", frame0);
//    signalNewDebugImage(2, "overlay1", frame0);
  //    if(frameId % 4 == 0 )
  //        signalNewDebugImage(1, "original", frame0);
  //    else if(frameId % 4 == 1)
  //            signalNewDebugImage(0, "original", frame0);
  //    else if(frameId % 4 == 3)
  //            signalNewDebugImage(2, "original", frame0);
  //    signalNewDebugImage(2, "with_rect", frame0);
  next_(frameId);
}

//void ImageProcessor::signalNewDebugImage(const int signal_id, const std::string& image_name,
//                                         boost::shared_ptr<Frame> frame) {
//    try {
//        switch (signal_id) {
//        case 0:
//            if(debug_view_signal0.num_slots() > 0) {
//                debug_view_signal0(frame->getImageAsJPG(image_name));
//            }
//            break;
////        case 1:
////            if(debug_view_signal1.num_slots() > 0) {
////                debug_view_signal1(frame->getImageAsJPG(image_name));
////            }
////            break;
////        case 2:
////            if(debug_view_signal2.num_slots() > 0) {
////                debug_view_signal2(frame->getImageAsJPG(image_name));
////            }
////            break;
//        }
//    } catch (std::exception& e) {
//        std::cerr << "ImageProcessor::processFrame - caught something! " << e.what() << std::endl;
//    }
//}
