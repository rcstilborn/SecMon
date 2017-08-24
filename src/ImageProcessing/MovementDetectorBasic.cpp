/*
 * MovementDetectorBasic.cpp
 *
 *  Created on: Aug 14, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#include "MovementDetectorBasic.h"
#include <vector>
#include "../Frame.h"

//our sensitivity value to be used in the absdiff() function
static const int kSensitivityValue = 20;
//size of blur used to smooth the intensity image output from absdiff() function
static const int kBlurSize = 10;


MovementDetectorBasic::MovementDetectorBasic()
    : previous_image_() {
}

MovementDetectorBasic::~MovementDetectorBasic() {
}

void MovementDetectorBasic::process_frame(boost::shared_ptr<Frame> frame0) {
  //set up the matrices that we will need
  //the two frames we will be comparing
  cv::Mat& img0 = frame0->get_original_image();

  //grayscale image (needed for absdiff() function)
  cv::Mat grayImage0;    // = frame0->getNewImage("gray");
//    if (grayImage0.empty())
  cv::cvtColor(img0, grayImage0, cv::COLOR_BGR2GRAY);

  // Test for a previous frame.
  if (previous_image_.empty()) {
    previous_image_ = grayImage0;
    return;
  }

  cv::Mat grayImage1 = previous_image_;
  previous_image_ = grayImage0;

  //resulting difference image
  cv::Mat& differenceImage = frame0->get_new_image("difference");
  //thresholded difference image (for use in findContours() function)
  cv::Mat& foregroundImage = frame0->get_new_image("foreground");

  //perform frame differencing with the sequential images. This will output an "intensity image"
  //do not confuse this with a threshold image, we will need to perform thresholding afterwards.
  cv::absdiff(grayImage0, grayImage1, differenceImage);
  //threshold intensity image at a given sensitivity value
  cv::threshold(differenceImage, foregroundImage, kSensitivityValue, 255, cv::THRESH_BINARY);

  std::vector<std::vector<cv::Point> > v;
  std::vector<cv::Vec4i> hierarchy;
  cv::findContours(foregroundImage, v, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
  foregroundImage = cv::Scalar(0, 0, 0);
  for (size_t i = 0; i < v.size(); ++i) {
    // drop smaller blobs
    if (cv::contourArea(v[i]) < 5)
      continue;
    // draw filled blob
    cv::drawContours(foregroundImage, v, i, cv::Scalar(255, 0, 0), CV_FILLED, 8, hierarchy, 0, cv::Point());
  }

  cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(25, 25));
  cv::morphologyEx(foregroundImage, foregroundImage, cv::MORPH_CLOSE, element);

  //blur the image to get rid of the noise. This will output an intensity image
//    cv::blur(foregroundImage,foregroundImage,cv::Size(BLUR_SIZE,BLUR_SIZE));
  //threshold again to obtain binary image from blur output
//    cv::threshold(foregroundImage,foregroundImage,SENSITIVITY_VALUE,255,cv::THRESH_BINARY);
  //search for contours in our thresholded image
//    searchForMovement(foregroundImage,img0);
}

//void MovementDetectorBasic::searchForMovement(cv::Mat thresholdImage, cv::Mat &cameraFeed){
//    //notice how we use the '&' operator for objectDetected and cameraFeed. This is because we wish
//    //to take the values passed into the function and manipulate them, rather than just working with a copy.
//    //eg. we draw to the cameraFeed to be displayed in the main() function.
//    bool objectDetected = false;
//    cv::Mat temp;
//    thresholdImage.copyTo(temp);
//    //these two vectors needed for output of findContours
//    std::vector<std::vector<cv::Point> > contours;
//    std::vector<cv::Vec4i> hierarchy;
//    //find contours of filtered image using openCV findContours function
//    //findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );// retrieves all contours
//    cv::findContours(temp,contours,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE );// retrieves external contours
//
//    //if contours vector is not empty, we have found some objects
//    if(contours.size()>0)objectDetected=true;
//    else objectDetected = false;
//
//    //we'll have just one object to search for
//    //and keep track of its position.
//    int theObject[2] = {0,0};
//
//    //bounding rectangle of the object, we will use the center of this as its position.
//    cv::Rect objectBoundingRectangle = cv::Rect(0,0,0,0);
//
//    if(objectDetected){
//        //the largest contour is found at the end of the contours vector
//        //we will simply assume that the biggest contour is the object we are looking for.
//        std::vector<std::vector<cv::Point> > largestContourVec;
//        largestContourVec.push_back(contours.at(contours.size()-1));
//        //make a bounding rectangle around the largest contour then find its centroid
//        //this will be the object's final estimated position.
//        objectBoundingRectangle = boundingRect(largestContourVec.at(0));
//        int xpos = objectBoundingRectangle.x+objectBoundingRectangle.width/2;
//        int ypos = objectBoundingRectangle.y+objectBoundingRectangle.height/2;
//
//        //update the objects positions by changing the 'theObject' array values
//        theObject[0] = xpos , theObject[1] = ypos;
//    }
//    //make some temp x and y variables so we dont have to type out so much
//    int x = theObject[0];
//    int y = theObject[1];
//
//    //draw some crosshairs around the object
//    cv::circle(cameraFeed,cv::Point(x,y),20,cv::Scalar(0,255,0),2);
//    cv::line(cameraFeed,cv::Point(x,y),cv::Point(x,y-25),cv::Scalar(0,255,0),2);
//    cv::line(cameraFeed,cv::Point(x,y),cv::Point(x,y+25),cv::Scalar(0,255,0),2);
//    cv::line(cameraFeed,cv::Point(x,y),cv::Point(x-25,y),cv::Scalar(0,255,0),2);
//    cv::line(cameraFeed,cv::Point(x,y),cv::Point(x+25,y),cv::Scalar(0,255,0),2);
//
//    //write the position of the object to the screen
////    cv::putText(cameraFeed,"Tracking object at (" + x +"," + y + ")",cv::Point(x,y),1,1,cv::Scalar(255,0,0),2);
//}
