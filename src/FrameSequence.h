/*
 * FrameSequence.h
 *
 * This is the sequence of images from a camera or file.  It contains all the current frames
 * (which contain the images themselves as well as the processing done on them), mechanisms to
 * signal threads the images are ready to process etc.
 *
 *  Created on: Jul 27, 2015
 *      Author: richard
 */

#ifndef FRAMESEQUENCE_H_
#define FRAMESEQUENCE_H_

#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/thread/pthread/mutex.hpp>
#include <map>
#include <vector>


const int frame_warning_level = 15;
const int frame_maximum = 60;

class Frame;

class FrameSequence {
public:
    FrameSequence();
//    FrameSequence(const int height, const int width);
    virtual ~FrameSequence();
    void setSize(const int width, const int height);
    boost::shared_ptr<Frame> getNewFrame();
    boost::shared_ptr<Frame> getFrame(const int frame_id);
    void deleteFrame(const int frame_id);
    void deleteEarliestFrame();
    void processNextFrame(const int frameId);

    std::vector<boost::shared_ptr<Frame>> getFrameList(const int frame_id, const int listSize);

private:
    int current_frame_id_;

    std::map<int, boost::shared_ptr<Frame>> frames;
    boost::mutex frame_list_mtx;

    unsigned int width_ = 0;
    unsigned int height_ = 0;

    // Make sure we can't copy them
    FrameSequence(const FrameSequence&);
    FrameSequence& operator=(FrameSequence);
};

#endif /* FRAMESEQUENCE_H_ */
