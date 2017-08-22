/*
 * OOI.h
 *
 *  Created on: Oct 23, 2015
 *      Author: richard
 */

#ifndef SRC_OOI_PROCESSING_OOI_H_
#define SRC_OOI_PROCESSING_OOI_H_

#include <boost/core/noncopyable.hpp>
#include <opencv2/core/types.hpp>
#include <string>

enum struct OOI_State: int
{
    newOOI = 0,
};

enum struct OOI_Type: int
{
    unknown = 0,
    pedestrian = 1,
    car = 2,
};


class OOI : private boost::noncopyable {
public:
    OOI(const int id, const cv::Rect& pos, const int frameId);
    virtual ~OOI();

    const cv::Rect getRectForDisplay(const int maxWidth, const int maxHeight) const;
    const cv::Rect getRectForClassification(const int maxWidth, const int maxHeight) const;
    void updatePosition(const cv::Rect& newPos, const int frameId);
    const cv::Rect& getCurrentPos() const;
    int getLastIdFrame() const;
    void setLastIdFrame(int frameId);
    int getLastUpdateFrameId() const;
    void setLastUpdateFrameId(int lastUpdateFrameId);
    OOI_State getState() const;
    OOI_Type getType() const;
    const std::string& getClassification() const;
    void setClassification(const std::string& classification);
    void draw(cv::Mat& overlayImage);
    int getId() const;

private:
    const int id;
    cv::Rect currentPos;
    OOI_State state;
    OOI_Type type;
    std::string classification = "none";
    const int firstFrame;
    int lastIdFrame = -1;
    int lastUpdateFrameId;

    cv::Rect ResizeRect(const int delta, const int maxWidth, const int maxHeight) const;
};

#endif /* SRC_OOI_PROCESSING_OOI_H_ */
