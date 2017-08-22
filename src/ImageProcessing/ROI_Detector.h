/*
 * ROI_Detector.h
 *
 *  Created on: Aug 16, 2015
 *      Author: richard
 */

#ifndef IMAGEPROCESSING_ROI_DETECTOR_H_
#define IMAGEPROCESSING_ROI_DETECTOR_H_

#include <boost/smart_ptr/shared_ptr.hpp>
#include <opencv2/core/types.hpp>
#include <cstdbool>
#include <vector>

#include "../Frame.h"

class ROI_Detector {
public:
    ROI_Detector();
    virtual ~ROI_Detector();

    void processFrame(boost::shared_ptr<Frame> frame0);

//private:
    static bool isTooSmall(std::vector<cv::Point> contour);
    static bool compareRectSize(const cv::Rect& a, const cv::Rect& b);
    static bool isInside(const cv::Rect& small, const cv::Rect& large);
    static int overlapPercentage(const cv::Rect& small, const cv::Rect& large);
    void removeNested(std::vector<cv::Rect>& rects);
    void consolidateRectangles(std::vector<cv::Rect>& rects);
};

#endif /* IMAGEPROCESSING_ROI_DETECTOR_H_ */
