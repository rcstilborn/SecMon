/*
 * OOI.cpp
 *
 *  Created on: Oct 23, 2015
 *      Author: richard
 */

#include "OOI.h"

#include <opencv2/core/mat.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <algorithm>

OOI::OOI(const int id, const cv::Rect& pos, const int frameId)
: id(id),
  currentPos(pos),
  state(OOI_State::newOOI),
  type(OOI_Type::unknown),
  firstFrame(frameId),
  lastUpdateFrameId(firstFrame)
{
    //    std::cout << "OOI::OOI constructed in frame " << frameId << std::endl;
    // TODO Add call to classifier
    // TODO Add call to getGoodPointsToTrack
}

OOI::~OOI() {
    //    std::cout << "OOI::~OOI destructed" << std::endl;
}

const std::string& OOI::getClassification() const {
    return classification;
}

void OOI::setClassification(const std::string& classification) {
    this->classification = classification;
}

int OOI::getId() const {
    return id;
}

cv::Rect OOI::ResizeRect(const int delta, const int maxWidth, const int maxHeight) const {
    cv::Rect newRect = currentPos;
    if(newRect.x > (delta - 1)) {
        newRect.x -= delta;
        newRect.width += delta;
    }
    if(newRect.y > (delta - 1)) {
        newRect.y -= delta;
        newRect.height += delta;
    }
    newRect.width += std::min(delta, maxWidth - (newRect.x + newRect.width));
    newRect.height += std::min(delta, maxHeight - (newRect.y + newRect.height));
    return newRect;
}

const cv::Rect OOI::getRectForDisplay(const int maxWidth, const int maxHeight) const {
    return ResizeRect(2, maxWidth, maxHeight);
}

const cv::Rect OOI::getRectForClassification(const int maxWidth, const int maxHeight) const {
    return ResizeRect(10, maxWidth, maxHeight);
}

void OOI::updatePosition(const cv::Rect& newPos, const int frameId) {
    currentPos = newPos;
    lastUpdateFrameId = frameId;
    //    std::cout << "Updating position at frame " << frameId << std::endl;
}

const cv::Rect& OOI::getCurrentPos() const {
    return currentPos;
}

int OOI::getLastIdFrame() const {
    return lastIdFrame;
}
void OOI::setLastIdFrame(int frameId) {
    lastIdFrame = frameId;
}

int OOI::getLastUpdateFrameId() const {
    return lastUpdateFrameId;
}

void OOI::setLastUpdateFrameId(int lastUpdateFrameId) {
    this->lastUpdateFrameId = lastUpdateFrameId;
}

OOI_State OOI::getState() const {
    return state;
}

OOI_Type OOI::getType() const {
    return type;
}

void OOI::draw(cv::Mat& overlayImage) {
    cv::Rect ooiRect = getRectForDisplay(overlayImage.cols, overlayImage.rows);
    cv::rectangle(overlayImage, ooiRect, cv::Scalar(0, 0, 255), 2);

    std::string text = getClassification();
    double text_size = std::min(1.2,overlayImage.rows / 850.0);
    int base_line = 0;

    cv::Size size = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, text_size, 2, &base_line);

    if(ooiRect.y >= size.height + 2){
        // Put the label at the top
        cv::rectangle(overlayImage, {ooiRect.x, ooiRect.y - (size.height + 2), ooiRect.width, size.height + 2}, cv::Scalar(0, 0, 255), CV_FILLED);
        cv::putText(overlayImage, text, cvPoint(ooiRect.x + 1, ooiRect.y - 2), cv::FONT_HERSHEY_SIMPLEX, text_size, cv::Scalar(255,255,255),2);
    }
}

