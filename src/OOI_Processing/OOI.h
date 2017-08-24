/*
 * OOI.h
 *
 *  Created on: Oct 23, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#ifndef OOI_PROCESSING_OOI_H_
#define OOI_PROCESSING_OOI_H_

#include <boost/core/noncopyable.hpp>
#include <opencv2/core/types.hpp>
#include <string>

enum struct OOI_State
  : int {
    newOOI = 0,
};

enum struct OOI_Type
  : int {
    unknown = 0,
  pedestrian = 1,
  car = 2,
};

class OOI : private boost::noncopyable {
 public:
  OOI(const int id, const cv::Rect& pos, const int frameId);
  virtual ~OOI();

  const cv::Rect get_rect_for_display(const int maxWidth, const int maxHeight) const;
  const cv::Rect get_rect_for_classification(const int maxWidth, const int maxHeight) const;
  void update_position(const cv::Rect& newPos, const int frameId);
  const cv::Rect& get_current_pos() const;
  int get_last_id_frame() const;
  void set_last_id_frame(int frameId);
  int get_last_update_frame_id() const;
  void set_last_update_frame_id(int lastUpdateFrameId);
  OOI_State get_state() const;
  OOI_Type get_type() const;
  const std::string& get_classification() const;
  void set_classification(const std::string& classification);
  void draw(cv::Mat& overlayImage);
  int get_id() const;

 private:
  const int id_;
  cv::Rect current_pos_;
  OOI_State state_;
  OOI_Type type_;
  std::string classification_ = "none";
  const int first_frame_;
  int last_id_frame_ = -1;
  int last_update_frame_id_;

  cv::Rect ResizeRect(const int delta, const int maxWidth, const int maxHeight) const;
};

#endif // OOI_PROCESSING_OOI_H_
