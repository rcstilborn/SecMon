/*
 * FrameTest.cpp
 *
 *  Created on: Aug 22, 2017
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>

#include "../src/Frame.h"


TEST(Frame_Constructor, one) {
    Frame f(1, 1920, 1080);
    EXPECT_EQ(f.get_frame_id(), 1) << "Expected frame id to be 1, got: " << f.get_frame_id();
}
