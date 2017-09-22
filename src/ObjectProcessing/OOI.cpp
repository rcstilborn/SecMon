/*
 * OOI.cpp
 *
 *  Created on: Oct 23, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#include "../ObjectProcessing/OOI.h"

#include <opencv2/core/mat.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <algorithm>
#include <string>

OOI::OOI(const int id, const cv::Rect& pos, const int frameId)
    : id_(id),
      current_pos_(pos),
      state_(OOI_State::newOOI),
      type_(OOI_Type::unknown),
      first_frame_(frameId),
      last_update_frame_id_(first_frame_) {
}

OOI::~OOI() {
}

const std::string& OOI::get_classification() const {
  return classification_;
}

void OOI::set_classification(const std::string& classification) {
  this->classification_ = classification;
}

int OOI::get_id() const {
  return id_;
}

cv::Rect OOI::ResizeRect(const int delta, const int maxWidth, const int maxHeight) const {
  cv::Rect newRect = current_pos_;
  if (newRect.x > (delta - 1)) {
    newRect.x -= delta;
    newRect.width += delta;
  }
  if (newRect.y > (delta - 1)) {
    newRect.y -= delta;
    newRect.height += delta;
  }
  newRect.width += std::min(delta, maxWidth - (newRect.x + newRect.width));
  newRect.height += std::min(delta, maxHeight - (newRect.y + newRect.height));
  return newRect;
}

const cv::Rect OOI::get_rect_for_display(const int maxWidth, const int maxHeight) const {
  return ResizeRect(2, maxWidth, maxHeight);
}

const cv::Rect OOI::get_rect_for_classification(const int maxWidth, const int maxHeight) const {
  return ResizeRect(10, maxWidth, maxHeight);
}

void OOI::update_position(const cv::Rect& newPos, const int frameId) {
  current_pos_ = newPos;
  last_update_frame_id_ = frameId;
}

const cv::Rect& OOI::get_current_pos() const {
  return current_pos_;
}

int OOI::get_last_id_frame() const {
  return last_id_frame_;
}
void OOI::set_last_id_frame(int frameId) {
  last_id_frame_ = frameId;
}

int OOI::get_last_update_frame_id() const {
  return last_update_frame_id_;
}

void OOI::set_last_update_frame_id(int lastUpdateFrameId) {
  this->last_update_frame_id_ = lastUpdateFrameId;
}

OOI_State OOI::get_state() const {
  return state_;
}

OOI_Type OOI::get_type() const {
  return type_;
}

void OOI::draw(cv::Mat& overlayImage) {
  cv::Rect ooiRect = get_rect_for_display(overlayImage.cols, overlayImage.rows);
  cv::rectangle(overlayImage, ooiRect, cv::Scalar(0, 0, 255), 2);

  std::string text = get_classification();
  double text_size = std::min(1.2, overlayImage.rows / 850.0);
  int base_line = 0;

  cv::Size size = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, text_size, 2, &base_line);

  if (ooiRect.y >= size.height + 2) {
    // Put the label at the top
    cv::rectangle(overlayImage, { ooiRect.x, ooiRect.y - (size.height + 2), ooiRect.width, size.height + 2 },
                  cv::Scalar(0, 0, 255), CV_FILLED);
    cv::putText(overlayImage, text, cvPoint(ooiRect.x + 1, ooiRect.y - 2), cv::FONT_HERSHEY_SIMPLEX, text_size,
                cv::Scalar(255, 255, 255), 2);
  }
}

