/*
 * OOI_Test.cpp
 *
 *  Created on: Oct 27, 2015
 *      Author: richard
 */

#include "../../src/ObjectProcessing/OOI.h"

#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>


TEST(OOI_getRectForDisplay, Normal) {
    cv::Rect orig(10,10,10,10);
    OOI ooi(0,orig,0);
    cv::Rect newOne(8,8,14,14);
    EXPECT_EQ(newOne, ooi.get_rect_for_display(40,40));
}

TEST(OOI_getRectForDisplay, Left) {
    cv::Rect orig(0,10,10,10);
    OOI ooi(0,orig,0);
    cv::Rect newOne(0,8,12,14);
    EXPECT_EQ(newOne, ooi.get_rect_for_display(40,40));
}

TEST(OOI_getRectForDisplay, TopLeft) {
    cv::Rect orig(0,0,10,10);
    OOI ooi(0,orig,0);
    cv::Rect newOne(0,0,12,12);
    EXPECT_EQ(newOne, ooi.get_rect_for_display(40,40));
}

TEST(OOI_getRectForDisplay, BottomRight) {
    cv::Rect orig(10,10,10,10);
    OOI ooi(0,orig,0);
    cv::Rect newOne(8,8,13,13);
    EXPECT_EQ(newOne, ooi.get_rect_for_display(21,21));
}

TEST(OOI_getRectForClassification, Normal) {
    cv::Rect orig(10,10,10,10);
    OOI ooi(0,orig,0);
    cv::Rect newOne(0,0,30,30);
    EXPECT_EQ(newOne, ooi.get_rect_for_classification(40,40));
}
