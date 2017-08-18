/*
 * ImageSource.cpp
 *
 *  Created on: Oct 23, 2015
 *      Author: richard
 */

#include "ImageSource.h"

#include <boost/asio/basic_deadline_timer.hpp>
#include <boost/asio/detail/wrapped_handler.hpp>
#include <boost/bind/arg.hpp>
#include <boost/bind/bind.hpp>
#include <boost/bind/placeholders.hpp>
#include <boost/date_time/microsec_time_clock.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/time.hpp>
#include <boost/date_time/time_duration.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <opencv2/core/mat.inl.hpp>
#include <glog/logging.h>
#include <exception>
#include <iostream>

#include "../Frame.h"
#include "../FrameSequence.h"

ImageSource::ImageSource(const std::string& name, const std::string& url, boost::asio::io_service& io_service, SceneInterface& sceneIf, FrameSequence& frameSequence, boost::function<void (const int)> next, const int fps)
: name(name),
  interval(1000/fps),
  camera(url),
  sceneIf(sceneIf),
  frameSequence(frameSequence),
  next(next),
  timer(io_service, boost::posix_time::milliseconds(interval)),
  strand(io_service),
  shuttingDown(false)
{
	DLOG(INFO) << "ImageSource(" << name << ") - constructed";
	frameSequence.setSize(camera.getWidth(), camera.getHeight());
	//	gui.registerNewScene(sceneIf);
	// Reset and start the timer
	startTimer();
}

void ImageSource::startTimer() {
	if(shuttingDown) return;
	// Reset the timer from now (restarting the camera takes time)
	this->timer.expires_from_now(boost::posix_time::milliseconds(this->interval));
	this->timer.async_wait(strand.wrap(boost::bind(&ImageSource::getNextFrame, this, _1)));
}

void ImageSource::restartTimer() {
	if(shuttingDown) return;
	// Reset the timer
	this->timer.expires_at(this->timer.expires_at()	+ boost::posix_time::milliseconds(this->interval));
	this->timer.async_wait(	strand.wrap(boost::bind(&ImageSource::getNextFrame, this, _1)));
}

void ImageSource::getNextFrame(const boost::system::error_code& ec) {
	//	std::cout << "ImageSource::getNextFrame() \"" << name << "\" - enter at " << boost::posix_time::microsec_clock::local_time() << std::endl;

	// Check for errors.
	if(ec) {
		//		std::cout << "ImageSource::getNextFrame() \"" << name << "\" - timer cancelled " << ec.message() << std::endl;
		return;
	}

	// Check for paused
    if(isPaused_) {
		restartTimer();
		return;
    }
	boost::shared_ptr<Frame> frame = frameSequence.getNewFrame();
	try {
		if(!this->camera.getNextFrame(frame->getOriginalImage(), frame->getOverlayImage())) {
			LOG(WARNING) << "ImageSource::getNextFrame() \"" << name << "\" - camera returned false so restarting at " << boost::posix_time::microsec_clock::local_time();
			this->camera.restart();

			// Try the camera again
			if(!this->camera.getNextFrame(frame->getOriginalImage(), frame->getOverlayImage())) {
				// If it fails again then abort - delete the last frame and don't restart the timer
				LOG(WARNING) << "ImageSource::getNextFrame() \"" << name << "\" - Camera failed to read after re-start.  Abort.";
				frameSequence.deleteFrame(frame->getFrameId());
				return;
			}
			// Reset the timer from now (restarting the camera takes time)
			startTimer();
		} else {
			// Reset the timer
			restartTimer();
		}
	} catch(std::exception& e) {
		LOG(WARNING) << "ImageSource::getNextFrame() \"" << name << "\" - caught exception from camera " << e.what();
		frameSequence.deleteFrame(frame->getFrameId());
		return;

	}

	// Check the frame isn't empty
	if(frame->getOriginalImage().empty()) {
		LOG(WARNING) << "Empty frame!!!";
		frameSequence.deleteFrame(frame->getFrameId());
		return;
	}

    next(frame->getFrameId());
	//	std::cout << "Scene::getNextFrame() - exit" << std::endl;
}

ImageSource::~ImageSource() {
	//	std::cout << "~ImageSource() - enter" << std::endl;
	/*int count = */timer.cancel();
	//	std::cout << "~ImageSource() - destructed " << count << std::endl;
}

Camera& ImageSource::getCamera() {
	return this->camera;
}

const std::string& ImageSource::getName() const {
	return this->name;
}

void ImageSource::shutdown() {
	//	std::cout << "Scene::shutdown() " << name << std::endl;
	shuttingDown = true;
	timer.cancel();
}

void ImageSource::togglePause() {
	isPaused_ = !isPaused_;
}

void ImageSource::setFPS(const int fps)  {
	interval = 1000/fps;
}
