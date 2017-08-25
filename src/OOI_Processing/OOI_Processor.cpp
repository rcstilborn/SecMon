/*
 * OOIProcessor.cpp
 *
 *  Created on: Oct 23, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#include "OOI_Processor.h"

#include <boost/bind/bind.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <glog/logging.h>
#include <exception>
#include <iostream>
#include <vector>
#include <memory>

#include "../Frame.h"
#include "../FrameSequence.h"

//static int VERY_CLOSE = 90;

OOI_Processor::OOI_Processor(boost::asio::io_service& io_service, SceneInterface& sceneIf, FrameSequence& frameSequence,
                             boost::function<void(const int)> next)
    : strand_(io_service),
//  caffeClassifier("/usr/local/caffe-master/models/bvlc_reference_caffenet/deploy.prototxt",
//          "/usr/local/caffe-master/models/bvlc_reference_caffenet/bvlc_reference_caffenet.caffemodel",
//          "/usr/local/caffe-master/data/ilsvrc12/imagenet_mean.binaryproto",
//          "/usr/local/caffe-master/data/ilsvrc12/synset_words.txt"),
      scene_interface_(sceneIf),
      frame_sequence_(frameSequence),
      next_(next),
      ooi_list_() {
}

OOI_Processor::~OOI_Processor() {
}

void OOI_Processor::process_next_frame(const int frameId) {
  strand_.post(boost::bind(&OOI_Processor::process_frame, this, frameId));
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void OOI_Processor::create_new_ooi(std::vector<cv::Rect>::iterator rois_it, const int frameId,
                                   const std::shared_ptr<Frame>& frame0, int& newOOIs, std::vector<cv::Rect>& rois) {
  std::shared_ptr<OOI> newOOI(new OOI(next_ooi_id_++, *rois_it, frameId));
//    classifyThis(frame0, newOOI);
  ooi_list_.insert({ newOOI->get_id(), newOOI });
  rois.erase(rois_it);  // automatically moves the it forward one
  newOOIs++;
}
#pragma GCC diagnostic error "-Wunused-parameter"

void OOI_Processor::process_frame(const int frameId) {
  std::shared_ptr<Frame> frame0;
  try {
    frame0 = frame_sequence_.get_frame(frameId);
  } catch (std::exception& e) {
    LOG(WARNING)<< "OOI_Processor::processFrame - exception getting frame " << frameId << ". ";
    return;
  }

//    // Copy the ROIs so we can delete the ones we have processed
//    std::vector<cv::Rect> rois = frame0->getRoIs();
//    //    std::cout << "Got " << rois.size() << " RoIs" << std::endl;
//
//    std::vector<cv::Rect>::iterator rois_it;
//    int newOOIs = 0, foundOOIs = 0, deletedOOIs = 0;
//    for (rois_it = rois.begin(); rois_it != rois.end();) {
//        std::vector<OOI*> ooisInThisRect;
//        for (auto ooip : ooiList)
//            if((*rois_it & ooip.second->getCurrentPos()).area() > 0)
//                ooisInThisRect.push_back(&ooip.second.get());
//        //        std::cout << "Got " << ooisInThisRect.size() << " OOIs for this ROI" << std::endl;
//
//        switch(ooisInThisRect.size()) {
//        // No matching OOI found so create a new one
//        case 0: {
//            createNewOOI(rois_it, frameId, frame0, newOOIs, rois);
//            continue;
//            break;
//        }
//        // One OOI found
//        case 1: {
//            foundOOIs++;
//            OOI& ooi = *ooisInThisRect.front();
//            int ov = (*rois_it & ooi.getCurrentPos()).area();
//            int overlapOR = (ov/(double) rois_it->area()) * 100;
//            int overlapRO = (ov/(double) ooi.getCurrentPos().area()) * 100;
//
//            if(overlapRO >= VERY_CLOSE) {  // Almost all of OOI is covered by ROI
//
//                if(overlapOR >= VERY_CLOSE) {  // Almost all of the ROI is covered too
//                    ooi.updatePosition(*rois_it, frameId);
//                    rois.erase(rois_it);  // automatically moves the it forward one
//                    continue;
//                } else {
//                    // ROI is bigger than OOI - could be a shadow or new OOI on edge of scene
//                    // TODO Complete this
//                    createNewOOI(rois_it, frameId, frame0, newOOIs, rois);
////                    ooi.setLastUpdateFrameId(frameId);
////                    rois.erase(rois_it);  // automatically moves the it forward one
//                    continue;
//                }
//            }
//            break;
//        }
//        // Multiple OOIs found - probably two ROIs got merged together or one is in front of another.
//        default: {
//            int roiCoverage = 0;
//            for (auto ooip : ooisInThisRect) {
//                foundOOIs++;
//                OOI& ooi = *ooip;
//                int ov = (*rois_it & ooi.getCurrentPos()).area();
//                int overlapOR = (ov/(double) rois_it->area()) * 100;
//                int overlapRO = (ov/(double) ooi.getCurrentPos().area()) * 100;
//
//                if(overlapRO >= VERY_CLOSE) {  // Almost all of OOI is covered by ROI
//
//                    if(overlapOR >= VERY_CLOSE) {  // Almost all of the ROI is covered too
//                        ooi.updatePosition(*rois_it, frameId);
//                        roiCoverage += overlapOR;
//                        rois.erase(rois_it);  // automatically moves the it forward one
//                        continue;
//                    }
//                    // TODO look at edges and update OOI accordingly
//                    ooi.setLastUpdateFrameId(frameId);
//                    roiCoverage += overlapOR;
//                }
//            }
//            if(roiCoverage >= VERY_CLOSE) {
//                rois.erase(rois_it);  // automatically moves the it forward one
//                continue;
//            }
//        }
//        }
//        ++rois_it;
//    }
//
//    //    std::cout << "Got " << ooiList.size() << " OOIs" << std::endl;
//
//    cv::Mat& overlayImage = frame0->getOverlayImage();
//    std::map<unsigned int, std::shared_ptr<OOI>>::iterator ooi_it;
//    for (ooi_it = ooiList.begin(); ooi_it != ooiList.end();) {
//        OOI& ooi = *ooi_it->second.get();
//        //    for(auto ooi : ooiList) {
//        if(ooi.getLastUpdateFrameId() < frameId -5) {
//            deletedOOIs++;
//            delete *ooi_it;
//            ooiList.erase(ooi_it);
//            continue;
//        }
//        ooi.draw(overlayImage);
//        ++ooi_it;
//    }
//    std::cout << "FrameId " << frameId << ", newOOIs " << newOOIs << ". foundOOIS " << foundOOIs <<
//            ", deletedOOIS " << deletedOOIs << ", ROIs " << frame0->getRoIs().size() <<
//            ", OOIs " << ooiList.size() << std::endl;

  next_(frameId);
  //   std::cout << "OOI_Processor::processFrame(" << frameId << ")" << std::endl;
}

//void OOI_Processor::classifyThis(std::shared_ptr<Frame> frame0, OOI* newOOI) {
//    // Get the image inside the ROI
//    cv::Mat image = frame0->getOriginalImage()(newOOI->getRectForClassification
//        (frame0->getOriginalImage().cols, frame0->getOriginalImage().rows));////
//
//    if(image.empty()){
//        std::cerr << "OOI image is blank" << std::endl;
//        return;
//    }

// Classify the image
//    std::vector<Prediction> predictions = caffeClassifier.Classify(image,1);
//    int i = 0;
////    while (isblank(predictions.front().first[i])) i++;
//    while (!isblank(predictions.front().first[i])) i++;
//    while (isblank(predictions.front().first[i])) i++;
//    int precision = predictions.front().second * 100;
//    std::string foo = std::to_string(precision) + ":" + predictions.front().first.substr(i,10);
//    newOOI->setClassification(foo);
//    newOOI->setLastIdFrame(frame0->getFrameId());

/* Print the top N predictions. */
//    for (size_t i = 0; i < predictions.size(); ++i) {
//        Prediction p = predictions[i];
//        std::cout << std::fixed << std::setprecision(4) << p.second << " - \""
//                << p.first << "\"" << std::endl;
//    }
//}
