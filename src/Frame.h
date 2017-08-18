/*
 * Frame.h
 *
 *  Created on: Jul 27, 2015
 *      Author: richard
 */

#ifndef FRAME_H_
#define FRAME_H_

#include <boost/date_time/posix_time/ptime.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#ifndef CPU_ONLY
#include <opencv2/core/cuda.hpp>
#endif // CPU_ONLY

#include <string>
#include <vector>

#include "SceneInterface.h"

const int max_images_per_frame = 10;

class Frame {
public:
	Frame(const int frame_id, const int width, const int height);
	virtual ~Frame();

	void setStartTime();
	int getFrameId() const;

	cv::Mat& getOriginalImage();
	cv::Mat& getOverlayImage();
	cv::Mat& getNewOrExistingImage(const std::string& name);
	cv::Mat& getNewImage(const std::string& name);
	cv::Mat& getNewBlankImage(const std::string& name);
	cv::Mat& getImage(const std::string& name);
	SceneInterface::image_ptr getImageAsJPG(const std::string& name);
	SceneInterface::image_ptr getImageAsJPG(const std::string& name1, const std::string& name2);
	SceneInterface::image_ptr getOriginalImageAsJPG();
	SceneInterface::image_ptr getOriginalWithOverlayImageAsJPG();
	const std::vector<cv::Rect>& getRoIs() const;
	std::vector<cv::Rect>& getRoIs();

private:
	const int frame_id_;
	// Make sure we can't copy them
	Frame(const Frame&);
	Frame& operator=(Frame);
	cv::Size frameSize;
	cv::Mat images[max_images_per_frame];
#ifndef CPU_ONLY
	cv::cuda::GpuMat gpu_images[max_images_per_frame];
#endif // CPU_ONLY

	std::string image_names[max_images_per_frame];
	boost::posix_time::ptime startTime;
	std::vector<cv::Rect> RoIs;
};

#endif /* FRAME_H_ */
