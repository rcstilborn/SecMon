/*
 * ROI_DetectorTest.cpp
 *
 *  Created on: Oct 20, 2015
 *      Author: richard
 */

#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>

#include "../../src/ImageProcessing/ROI_Detector.h"


TEST(ROI_Detector_IsInsideTest, Inside) {
	ROI_Detector rd;
	cv::Rect small(10,10,10,10);
	cv::Rect large(8,8,20,20);
	EXPECT_TRUE(rd.isInside(small, large));
}

TEST(ROI_Detector_IsInsideTest, Outside) {
	ROI_Detector rd;
	cv::Rect small(10,10,10,10);
	cv::Rect large(8,8,20,20);
	EXPECT_FALSE(rd.isInside(large, small));
}

TEST(ROI_Detector_IsInsideTest, Same) {
	ROI_Detector rd;
	cv::Rect small(10,10,10,10);
	cv::Rect large(10,10,10,10);
	EXPECT_TRUE(rd.isInside(large, small));
}

TEST(ROI_Detector_ConsolidateRectangles, OneNested) {
	ROI_Detector rd;
	cv::Rect small(10,10,10,10);
	cv::Rect large(8,8,20,20);
	std::vector<cv::Rect> v = {small, large};
	rd.consolidateRectangles(v);
	EXPECT_EQ(large, *v.begin());
	EXPECT_EQ(1, v.size());
}

TEST(ROI_Detector_ConsolidateRectangles, OneNestedOneNot) {
	ROI_Detector rd;
	cv::Rect small(10,10,10,10);
	cv::Rect large(8,8,20,20);
	cv::Rect third(80,80,20,20);
	std::vector<cv::Rect> v = {small, large, third};
	rd.consolidateRectangles(v);
	//EXPECT_EQ(large, *v.begin());
	EXPECT_EQ(2, v.size());
}

TEST(ROI_Detector_ConsolidateRectangles, TwoNested) {
	ROI_Detector rd;
	cv::Rect small(10,10,10,10);
	cv::Rect small2(82,82,11,11);
	cv::Rect large(8,8,20,20);
	cv::Rect large2(80,80,21,21);
	std::vector<cv::Rect> v = {small, small2, large, large2};
	rd.consolidateRectangles(v);
	//EXPECT_EQ(large, *v.begin());
	EXPECT_EQ(2, v.size());
	EXPECT_EQ(large,*(std::find(v.begin(),v.end(),large)));
	EXPECT_EQ(large2,*(std::find(v.begin(),v.end(),large2)));
}

TEST(ROI_Detector_ConsolidateRectangles, TwoNestedOneTheSame) {
	ROI_Detector rd;
	cv::Rect small(10,10,10,10);
	cv::Rect small2(82,82,11,11);
	cv::Rect large(8,8,20,20);
	cv::Rect large2(80,80,21,21);
	cv::Rect large3(80,80,21,21);
	std::vector<cv::Rect> v = {small, small2, large, large2, large3};
	rd.consolidateRectangles(v);
	//EXPECT_EQ(large, *v.begin());
	EXPECT_EQ(2, v.size());
	EXPECT_EQ(large,*(std::find(v.begin(),v.end(),large)));
	EXPECT_EQ(large3,*(std::find(v.begin(),v.end(),large3)));
}

TEST(ROI_Detector_ConsolidateRectangles, None) {
	ROI_Detector rd;
	std::vector<cv::Rect> v;
	rd.consolidateRectangles(v);
	//EXPECT_EQ(large, *v.begin());
	EXPECT_EQ(0, v.size());
}

TEST(ROI_Detector_ConsolidateRectangles, One) {
	ROI_Detector rd;
	cv::Rect small(10,10,10,10);
	std::vector<cv::Rect> v = {small};
	rd.consolidateRectangles(v);
	//EXPECT_EQ(large, *v.begin());
	EXPECT_EQ(1, v.size());
}

TEST(ROI_Detector_ConsolidateRectangles, 50Overlap) {
	ROI_Detector rd;
	cv::Rect small(20,0, 10, 10);
	cv::Rect large(10,10,30,30);
	std::vector<cv::Rect> v = {small, large};
	rd.consolidateRectangles(v);
	EXPECT_EQ(large,*(std::find(v.begin(),v.end(),large)));
	EXPECT_EQ(small,*(std::find(v.begin(),v.end(),small)));
	EXPECT_EQ(2, v.size());
}

TEST(ROI_Detector_ConsolidateRectangles, 80Overlap) {
	ROI_Detector rd;
	cv::Rect small(20,8, 10, 10);
	cv::Rect large(10,10,30,30);
	std::vector<cv::Rect> v = {small, large};
	rd.consolidateRectangles(v);
	EXPECT_EQ(large,*(std::find(v.begin(),v.end(),large)));
	EXPECT_EQ(small,*(std::find(v.begin(),v.end(),small)));
	EXPECT_EQ(2, v.size());
}

TEST(ROI_Detector_ConsolidateRectangles, 90Overlap) {
	ROI_Detector rd;
	cv::Rect small(20,9, 10, 10);
	cv::Rect large(10,10,30,30);
	std::vector<cv::Rect> v = {small, large};
	rd.consolidateRectangles(v);
	EXPECT_EQ(10,v.begin()->x);
	EXPECT_EQ(9,v.begin()->y);
	EXPECT_EQ(30,v.begin()->width);
	EXPECT_EQ(31,v.begin()->height);
	EXPECT_EQ(1, v.size());
}

TEST(ROI_Detector_OverlapPercentage, Half) {
	ROI_Detector rd;
	cv::Rect small(10,10,10,10);
	cv::Rect large(10,15,10,10);
	EXPECT_EQ(50, rd.overlapPercentage(small, large));
}

TEST(ROI_Detector_OverlapPercentage, Quarter) {
	ROI_Detector rd;
	cv::Rect small(5,5,10,10);
	cv::Rect large(10,10,10,10);
	EXPECT_EQ(25, rd.overlapPercentage(small, large));
}

TEST(ROI_Detector_OverlapPercentage, None) {
	ROI_Detector rd;
	cv::Rect small(5,5,4,4);
	cv::Rect large(10,10,10,10);
	EXPECT_EQ(0, rd.overlapPercentage(small, large));
}
