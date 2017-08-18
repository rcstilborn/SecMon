/*
 * Camera.cpp
 *
 *  Created on: Jul 27, 2015
 *      Author: richard
 */

#include "Camera.h"

#include <boost/date_time/microsec_time_clock.hpp>
#include <boost/date_time/posix_time/posix_time_config.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/date_time/time.hpp>
#include <boost/date_time/time_clock.hpp>
#include <boost/date_time/time_duration.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/mat.inl.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio/videoio_c.h>
#include <glog/logging.h>
#include <iostream>
#include <stdexcept>


Camera::Camera(const std::string& source)
: source_(source) {

	// Connect to the camera
	DLOG(INFO) << "Opening camera at " << source_;

	if(source.substr(0,4) == "http")
		real_camera_ = true;
	else
		real_camera_ = false;

	boost::posix_time::ptime start = boost::posix_time::microsec_clock::local_time();

	this->camera_.open(source_);
	boost::posix_time::time_duration dur = boost::posix_time::microsec_clock::local_time() - start;
	if (!this->camera_.isOpened()) {
		LOG(ERROR) << "Could not open camera at: " << source_;
		throw std::runtime_error("Could not open camera at: " + source_);
	}

	cv::Mat img;
	if(!this->camera_.read(img) || img.empty()) {
		LOG(ERROR) << "Could not get first image from camera at: " << source_;
		throw std::runtime_error("Could not get first image from camera at: " + source_);
	}

	fps_ = this->camera_.get(CV_CAP_PROP_FPS);
	width_ = img.cols;//(unsigned int) this->camera_.get(CV_CAP_PROP_FRAME_WIDTH);
	height_ = img.rows;//(unsigned int) this->camera_.get(CV_CAP_PROP_FRAME_HEIGHT);

	// Make text size proportional to the image height
	text_size_ = img.rows / 650.0;
	left_margin_ = text_size_ * 25;
	top_margin_ = text_size_ * 35;

	DLOG(INFO) << "Connected to camera in " << dur.total_milliseconds()/1000.0 << "s. With size: [" << this->height_ << "x" << this->width_ << "]  FPS: " << this->fps_;
	//    std::cout << "Text size " << text_size_ << " left margin " << left_margin_ << " top margin " << top_margin_ << std::endl;
}

Camera::~Camera() {
	//	std::cout << "~Camera() - destructed" << std::endl;
}

bool Camera::getNextFrame(cv::Mat& img, cv::Mat& overlay) {
	//	std::cout << "Camera::getNextFrame() - enter" << std::endl;
	if (camera_.read(img)) {
		if (real_camera_) {
			const std::string now = boost::posix_time::to_simple_string(boost::posix_time::second_clock::local_time());
			cv::putText(overlay, now, cvPoint(40, 25), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(255,255,255),2);
		} else {
			std::string frame_number = std::to_string((unsigned int)this->camera_.get(CV_CAP_PROP_POS_FRAMES));
			cv::putText(overlay, frame_number, cvPoint(left_margin_, top_margin_), cv::FONT_HERSHEY_SIMPLEX, text_size_, cv::Scalar(255,255,255),2);
		}
		return true;
	}
	return false;
	//	std::cout << "Camera::getNextFrame() - exit" << std::endl;
}

int Camera::getFPS() const {
	return this->fps_;
}

const std::string& Camera::getSourceName() const {
	return this->source_;
}

unsigned int Camera::getWidth() const{
	return this->width_;
}

unsigned int Camera::getHeight() const{
	return this->height_;
}

void Camera::restart() {
	//	std::cout << "Camera::restart() - enter" << std::endl;
	this->camera_.release();
	boost::posix_time::ptime start = boost::posix_time::microsec_clock::local_time();
	this->camera_.open(this->source_);
	boost::posix_time::time_duration dur = boost::posix_time::microsec_clock::local_time() - start;
	if (!this->camera_.isOpened()) {
		LOG(ERROR) << "Could not re-open camera at: " << source_;
		throw std::runtime_error("Could not re-open camera at: " + source_);
	}
	if (real_camera_)
		DLOG(INFO) << "Re-connected to camera at " << source_ << " in " << dur.total_milliseconds()/1000.0 << "s";

}
