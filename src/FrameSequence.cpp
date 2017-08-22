/*
 * FrameSequence.cpp
 *
 *  Created on: Jul 27, 2015
 *      Author: richard
 */

#include "FrameSequence.h"

#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/pthread/mutex.hpp>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

#include "Frame.h"

FrameSequence::FrameSequence()
: current_frame_id_(0), frames(), frame_list_mtx() {
    //VLOG(1) << "FrameSequence() - constructed";
}

//FrameSequence::FrameSequence(const int width, const int height)
//: current_frame_id_(0),
//  width_(width),
//  height_(height) {
//    //    std::cout << "FrameSequence() - constructed" << std::endl;
//}

FrameSequence::~FrameSequence() {
    //    std::cout << "~FrameSequence() - deleting frames" << std::endl;
    boost::lock_guard<boost::mutex> guard(frame_list_mtx);
    while(!this->frames.empty()){
        this->frames.erase(this->frames.begin());
    }
    //VLOG(1) << "~FrameSequence() - destructed";
}

void FrameSequence::setSize(const int width, const int height) {
    width_ = width;
    height_ = height;
}

boost::shared_ptr<Frame> FrameSequence::getNewFrame(){
    //    std::cout << "FrameSequence.getNewFrame() - enter" << std::endl;
    const int frame_id = current_frame_id_++;
    boost::shared_ptr<Frame> newFrame(new Frame(frame_id, width_, height_));
    {
        boost::lock_guard<boost::mutex> guard(frame_list_mtx);
        this->frames.insert(std::pair<const int,boost::shared_ptr<Frame>>(frame_id, newFrame));
    }
    if (frames.size() > frame_maximum) {
        //        std::cerr << "FrameSequence::getNewFrame() - Warning! More than " << frame_maximum << " frames in queue! Deleting one now!" << std::endl;
        deleteEarliestFrame();
    }
    //    else if (frames.size() > frame_warning_level) {
    //        std::cerr << "FrameSequence::getNewFrame() - Warning! More than " << frame_warning_level << " frames in queue!" << std::endl;
    //    }
    //    std::cout << "FrameSequence.getNewFrame() - exit" << std::endl;
    return newFrame;
}

void FrameSequence::deleteFrame(const int frame_id) {
    boost::lock_guard<boost::mutex> guard(frame_list_mtx);
    auto it = frames.find(frame_id);
    if (it == frames.end())
        return;
    frames.erase(it);
    //    std::cout << "FrameSequence.deleteLastFrame() - exit" << std::endl;
}

boost::shared_ptr<Frame> FrameSequence::getFrame(const int frame_id){
    boost::lock_guard<boost::mutex> guard(frame_list_mtx);
    auto it = frames.find(frame_id);
    if (it == frames.end())
        throw std::invalid_argument("No such frame");
    return it->second;
    //    std::cout << "FrameSequence.deleteLastFrame() - exit" << std::endl;
}

std::vector<boost::shared_ptr<Frame>> FrameSequence::getFrameList(const int frame_id, const int listSize) {
    boost::lock_guard<boost::mutex> guard(frame_list_mtx);
    std::vector<boost::shared_ptr<Frame>> frameList;
    auto it = frames.find(frame_id);
    if (it == frames.end())
        throw std::invalid_argument("No such frame");
    frameList.push_back(it->second);
    for (int i = 0; i < listSize; i++) {
        if(--it == frames.begin())
            break;
        frameList.push_back(it->second);
    }
    return frameList;
}

void FrameSequence::deleteEarliestFrame() {
    boost::lock_guard<boost::mutex> guard(frame_list_mtx);
    //    std::cout << "FrameSequence.deleteEarliestFrame() - enter" << std::endl;
    if(!frames.empty())
        frames.erase(frames.begin());
    //    std::cout << "FrameSequence.deleteEarliestFrame() - exit" << std::endl;
}
