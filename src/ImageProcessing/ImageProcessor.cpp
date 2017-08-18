/*
 * ImageProcessor.cpp
 *
 *  Created on: Aug 14, 2015
 *      Author: richard
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


ImageProcessor::ImageProcessor(boost::asio::io_service& io_service, SceneInterface& sceneIf, FrameSequence& frameSequence, boost::function<void (const int)> next)
: strand(io_service),
  sceneIf(sceneIf),
  frameSequence(frameSequence),
  next(next)
//  main_view_signal(sceneIf.addStream("Main view")),
//  main_view_signal(sceneIf.addStream("Overlay")),
//  overlay_view_signal(sceneIf.addStream("Overlay")),
//  debug_view_signal0(sceneIf.addStream("Debug 0"))//,
//  debug_view_signal1(sceneIf.addStream("Debug 1")),
//  debug_view_signal2(sceneIf.addStream("Debug 2"))
//  imbs(10)
{
    movementDetector = new MovementDetectorBasic;
//    movementDetector = new MovementDetectorViBe;
#ifdef DEBUG
    sceneIf.addStream("foreground");
#endif // DEBUG
}

ImageProcessor::~ImageProcessor() {
  delete movementDetector;
}

void ImageProcessor::processNextFrame(const int frameId) {
	strand.post(boost::bind(&ImageProcessor::processFrame, this, frameId));
}

void ImageProcessor::processFrame(const int frameId) {
	//	std::cout << "ImageProcessor::processFrame id=" << frameId << std::endl;
	boost::shared_ptr<Frame> frame0;
	try {
		frame0 = frameSequence.getFrame(frameId);
	} catch (std::exception& e) {
		LOG(WARNING) << "ImageProcessor::processFrame - exception getting frame " << frameId << ". " << e.what();
		return;
	}

	frame0->setStartTime();

	movementDetector->processFrame(frame0);
//		bg1.subtractBackground(frame0);
	//	imbs.apply(frame0->getOriginalImage(),frame0->getNewImage("imbs"));

	if(frameId < 300)
        return;

	roi.processFrame(frame0);

//	if(main_view_signal.num_slots() > 0) {
//		main_view_signal(frame0->getOriginalImageAsJPG());
//		main_view_signal(frame0->getOriginalWithOverlayImageAsJPG());
//	}

//	if(overlay_view_signal.num_slots() > 0) {
//		overlay_view_signal(frame0->getOriginalWithOverlayImageAsJPG());
//	}
//	signalNewDebugImage(0, "mog2", frame0);
//	signalNewDebugImage(0, "difference", frame0);
//	signalNewDebugImage(0, "foreground", frame0);
//	signalNewDebugImage(1, "contours", frame0);
//	signalNewDebugImage(2, "overlay1", frame0);
	//	if(frameId % 4 == 0 )
	//		signalNewDebugImage(1, "original", frame0);
	//	else if(frameId % 4 == 1)
	//			signalNewDebugImage(0, "original", frame0);
	//	else if(frameId % 4 == 3)
	//			signalNewDebugImage(2, "original", frame0);
	//	signalNewDebugImage(2, "with_rect", frame0);
    next(frameId);
}

//void ImageProcessor::signalNewDebugImage(const int signal_id, const std::string& image_name, boost::shared_ptr<Frame> frame) {
//	try {
//		switch (signal_id) {
//		case 0:
//			if(debug_view_signal0.num_slots() > 0) {
//				debug_view_signal0(frame->getImageAsJPG(image_name));
//			}
//			break;
////		case 1:
////			if(debug_view_signal1.num_slots() > 0) {
////				debug_view_signal1(frame->getImageAsJPG(image_name));
////			}
////			break;
////		case 2:
////			if(debug_view_signal2.num_slots() > 0) {
////				debug_view_signal2(frame->getImageAsJPG(image_name));
////			}
////			break;
//		}
//	} catch (std::exception& e) {
//		std::cerr << "ImageProcessor::processFrame - caught something! " << e.what() << std::endl;
//	}
//}
