/*
 * Common.h
 *
 *  Created on: Nov 2, 2015
 *      Author: richard
 */

#ifndef SRC_COMMON_H_
#define SRC_COMMON_H_


#ifdef CPU_ONLY
#include <opencv2/core.hpp>
typedef cv::Mat Image;
#else
#include <opencv2/core/cuda.hpp>
typedef cv::cuda::GpuMat Image;
#endif // CPU_ONLY



#endif /* SRC_COMMON_H_ */
