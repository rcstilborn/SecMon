/*
 * OOI_Test.cpp
 *
 *  Created on: Oct 27, 2015
 *      Author: richard
 */

#include "../../src/OOI_Processing/OOI.h"

#include "/usr/local/googletest/include/gtest/gtest.h"


TEST(getRectForDisplay, Normal) {
	cv::Rect orig(10,10,10,10);
	OOI ooi(0,orig,0);
	cv::Rect newOne(8,8,14,14);
	EXPECT_EQ(newOne, ooi.getRectForDisplay(40,40));
}

TEST(getRectForDisplay, Left) {
	cv::Rect orig(0,10,10,10);
	OOI ooi(0,orig,0);
	cv::Rect newOne(0,8,12,14);
	EXPECT_EQ(newOne, ooi.getRectForDisplay(40,40));
}

TEST(getRectForDisplay, TopLeft) {
	cv::Rect orig(0,0,10,10);
	OOI ooi(0,orig,0);
	cv::Rect newOne(0,0,12,12);
	EXPECT_EQ(newOne, ooi.getRectForDisplay(40,40));
}

TEST(getRectForDisplay, BottomRight) {
	cv::Rect orig(10,10,10,10);
	OOI ooi(0,orig,0);
	cv::Rect newOne(8,8,13,13);
	EXPECT_EQ(newOne, ooi.getRectForDisplay(21,21));
}

TEST(getRectForClassification, Normal) {
	cv::Rect orig(10,10,10,10);
	OOI ooi(0,orig,0);
	cv::Rect newOne(0,0,30,30);
	EXPECT_EQ(newOne, ooi.getRectForClassification(40,40));
}
