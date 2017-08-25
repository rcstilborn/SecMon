/*
 * OOIProcessor.h
 *
 *  Created on: Oct 23, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#ifndef OOI_PROCESSING_OOI_PROCESSOR_H_
#define OOI_PROCESSING_OOI_PROCESSOR_H_

#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>
#include <boost/core/noncopyable.hpp>
#include <boost/function/function_fwd.hpp>
#include <boost/function.hpp>
#include <vector>
#include <map>
#include <memory>

#include "OOI.h"

class SceneInterface;
class FrameSequence;
class Frame;

class OOI_Processor : private boost::noncopyable {
 public:
  OOI_Processor(boost::asio::io_service& io_service, SceneInterface& sceneIf, FrameSequence& frameSequence,
                boost::function<void(const int)> next);
  virtual ~OOI_Processor();
  void process_next_frame(const int frameId);
  void classify_this(std::shared_ptr<Frame> frame0, OOI* newOOI);

 private:
  /// Strand to ensure the connection's handlers are not called concurrently.
  boost::asio::io_service::strand strand_;
//    Classifier caffeClassifier;

  SceneInterface& scene_interface_;
  FrameSequence& frame_sequence_;
  boost::function<void(const int)> next_;
  unsigned int next_ooi_id_ = 0;
  std::map<unsigned int, std::shared_ptr<OOI>> ooi_list_;
  void process_frame(const int frameId);
  void create_new_ooi(std::vector<cv::Rect>::iterator rois_it, const int frameId,
                      const std::shared_ptr<Frame>& frame0, int& newOOIs, std::vector<cv::Rect>& rois);
};

#endif // OOI_PROCESSING_OOI_PROCESSOR_H_
