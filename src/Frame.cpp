/*
 * Frame.cpp
 *
 *  Created on: Jul 27, 2015
 *      Author: richard
 */

#include "Frame.h"

#include <boost/date_time/microsec_time_clock.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <opencv2/core/cvdef.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/mat.inl.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

Frame::Frame(const int frame_id, const int width, const int height)
: frame_id_(frame_id),
  frameSize(width, height),
  startTime(),
  RoIs(){
	//VLOG(2) << "Frame() - constructed id=" <<  frame_id_;
	image_names[0] = "original";
	image_names[1] = "overlay";
	images[1] = cv::Mat::zeros(frameSize, CV_8UC3);
}


Frame::~Frame() {
	//VLOG(2) << "~Frame() - destructed id=" << frame_id_;
}

cv::Mat& Frame::getOriginalImage() {
	return images[0];
}

cv::Mat& Frame::getOverlayImage() {
//	std::cout << "overlay size now " << images[1].size() << std::endl;
	return images[1];
}

cv::Mat& Frame::getNewOrExistingImage(const std::string& name) {
	for (int i = 0; i < max_images_per_frame; i++)
		if(image_names[i] == name)
			return images[i];
	return getNewImage(name);
}


cv::Mat& Frame::getNewImage(const std::string& name) {
	for (int i = 0; i < max_images_per_frame; i++)
		if(image_names[i] == name) {
			throw std::invalid_argument("Image already registered with name " + name);
		} else if (image_names[i].empty()) {
			image_names[i] = name;
			return images[i];
		}
	throw std::invalid_argument("No available images");
}

cv::Mat& Frame::getNewBlankImage(const std::string& name) {
	for (int i = 0; i < max_images_per_frame; i++)
		if(image_names[i] == name) {
			throw std::invalid_argument("Image already registered with name " + name);
		} else if (image_names[i].empty()) {
			image_names[i] = name;
			images[i].zeros(frameSize, CV_8UC3);
			return images[i];
		}
	throw std::invalid_argument("No available images");
}

cv::Mat& Frame::getImage(const std::string& name) {
	for (int i = 0; i < max_images_per_frame; i++)
		if(image_names[i] == name)
			return images[i];
	throw std::invalid_argument("No image registered with name " + name);
}

SceneInterface::image_ptr Frame::getImageAsJPG(const std::string& name) {
	// TODO make this hack more elegant
	if(name=="main")
		return getOriginalWithOverlayImageAsJPG();
	cv::Mat& image = getImage(name);
	if (!image.empty()) {
		SceneInterface::image_ptr buffer(new std::vector<unsigned char>());
		cv::imencode(".jpg",image,*buffer);
		return buffer;
	}
	throw std::invalid_argument("No data in image with name " + name);
}

SceneInterface::image_ptr Frame::getOriginalImageAsJPG() {
	if (!images[0].empty()) {
		SceneInterface::image_ptr buffer(new std::vector<unsigned char>());
		cv::imencode(".jpg",images[0],*buffer);
		return buffer;
	}
	throw std::invalid_argument("No data in original image");
}

SceneInterface::image_ptr Frame::getImageAsJPG(const std::string& name1, const std::string& name2) {
	cv::Mat& image1 = getImage(name1);
	if (image1.empty())
		throw std::invalid_argument("Frame::getImageAsJPG - No data in image with name " + name1);

	cv::Mat& image2 = getImage(name2);
	if (!image2.empty()) {
		image2.copyTo(image1);
	}
	SceneInterface::image_ptr buffer(new std::vector<unsigned char>());
	cv::imencode(".jpg",image1,*buffer);
	return buffer;
}

SceneInterface::image_ptr Frame::getOriginalWithOverlayImageAsJPG() {
	if (images[0].empty())
		throw std::invalid_argument("No data in original image");

	images[1].copyTo(images[0],images[1]);
	SceneInterface::image_ptr buffer(new std::vector<unsigned char>());
	cv::imencode(".jpg",images[0],*buffer);
	return buffer;
}

void Frame::setStartTime() {
	this->startTime = boost::posix_time::microsec_clock::local_time();
}

int Frame::getFrameId() const {
	return frame_id_;
}

const std::vector<cv::Rect>& Frame::getRoIs() const {
	return RoIs;
}

std::vector<cv::Rect>& Frame::getRoIs() {
	return RoIs;
}
