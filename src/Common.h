/*
 * Common.h
 *
 *  Created on: Nov 2, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#ifndef COMMON_H_
#define COMMON_H_

#ifdef CPU_ONLY
#include <opencv2/core.hpp>
typedef cv::Mat Image;
#else
#include <opencv2/core/cuda.hpp>
typedef cv::cuda::GpuMat Image;
#endif // CPU_ONLY

#endif // COMMON_H_
