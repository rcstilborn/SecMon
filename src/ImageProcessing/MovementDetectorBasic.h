/*
 * ROIDetector.h
 *
 *  Created on: Aug 14, 2015
 *      Author: richard
 */

#ifndef IMAGEPROCESSING_MOVEMENT_DETECTOR_H_
#define IMAGEPROCESSING_MOVEMENT_DETECTOR_H_
#include <opencv2/opencv.hpp>
#include <boost/shared_ptr.hpp>

#include "MovementDetector.h"


class MovementDetectorBasic : public MovementDetector {
public:
	MovementDetectorBasic();
	virtual ~MovementDetectorBasic();

	void processFrame(boost::shared_ptr<Frame> frame0);

private:
	void searchForMovement(cv::Mat thresholdImage, cv::Mat &cameraFeed);
	cv::Mat previousImage;

};

#endif /* IMAGEPROCESSING_MOVEMENT_DETECTOR_H_ */
