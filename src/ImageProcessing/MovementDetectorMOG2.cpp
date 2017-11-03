/*
 * MovementDetectorMOG2.cpp
 *
 *  Created on: Oct 18, 2017
 *      Author: richard
 */

#include "MovementDetectorMOG2.h"
#include <opencv2/opencv.hpp>
#include <memory>
#include <vector>

#include "../Frame.h"


MovementDetectorMOG2::MovementDetectorMOG2()
 : pMOG2(cv::createBackgroundSubtractorMOG2(25,10)) {
  // TODO Auto-generated constructor stub

}

MovementDetectorMOG2::~MovementDetectorMOG2() {
  // TODO Auto-generated destructor stub
}

void MovementDetectorMOG2::process_next_frame(std::shared_ptr<Frame>& current_frame) {
  cv::Mat& image_current = current_frame->get_original_image();
  cv::Mat& difference_image = current_frame->get_new_image("difference");
  difference_image.reshape(CV_8UC1);
  cv::Mat& foreground_image = current_frame->get_new_blank_image("foreground");
//  foreground_image.zeros(image_current.size(), CV_8UC1);
//  cv::Mat temp;
//  temp.zeros(image_current.size(), CV_8UC1);

  pMOG2->apply(image_current, difference_image);
  refineSegments(difference_image, foreground_image);
}

void MovementDetectorMOG2::refineSegments(const cv::Mat& mask, cv::Mat& dst)
{
    int niters = 3;
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::Mat temp;
    cv::dilate(mask, temp, cv::Mat(), cv::Point(-1,-1), niters);
    cv::erode(temp, temp, cv::Mat(), cv::Point(-1,-1), niters*2);
    cv::dilate(temp, temp, cv::Mat(), cv::Point(-1,-1), niters);
    cv::findContours( temp, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE );
    if( contours.empty() ) {
      std::cout << "NO contours found!!" << std::endl;
      return;
    }

    // iterate through all the top-level contours,
    // draw each connected component with its own random color
    int idx = 0;//, largestComp = 0;
//    double maxArea = 0;
    for( ; idx >= 0; idx = hierarchy[idx][0] )
    {
      cv::drawContours( dst, contours, idx, cv::Scalar(255,0,0), cv::FILLED, cv::LINE_8, hierarchy );

//        const std::vector<cv::Point>& c = contours[idx];
//        double area = std::fabs(cv::contourArea(cv::Mat(c)));
//        if( area > maxArea )
//        {
//            maxArea = area;
//            largestComp = idx;
//        }
    }
//    cv::Scalar color( 255, 255, 255 );
//    cv::drawContours( dst, contours, largestComp, color, cv::FILLED, cv::LINE_8, hierarchy );
}

