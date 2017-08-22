/*
 * SceneInterface.cpp
 *
 *  Created on: Aug 13, 2015
 *      Author: richard
 */

#include "SceneInterface.h"

#include <boost/thread/lock_guard.hpp>

#include "Frame.h"
#include "FrameSequence.h"
#include "GUI/GUI_Interface.h"

SceneInterface::SceneInterface(const std::string& displayName, const std::string& description, GUI_Interface& gui, FrameSequence& frameSequence)
:displayName(displayName),
 description(description),
 gui(gui),
 frameSequence(frameSequence),
 streams_list_mtx_(),
 streams() {
	addStream("main");
}

SceneInterface::~SceneInterface() {
	// TODO Auto-generated destructor stub
}

const std::string& SceneInterface::getDescription() const {
	return description;
}

const std::string& SceneInterface::getDisplayName() const {
	return displayName;
}

void SceneInterface::addStream(const std::string& name) {
	boost::lock_guard<boost::mutex> guard(streams_list_mtx_);
	boost::shared_ptr<SceneInterface::Stream> stream_ptr(new SceneInterface::Stream(name));
	streams.push_back(stream_ptr);
	//TODO Should pass the shared_ptr
	gui.registerNewStream(*stream_ptr);
	return;
}
//boost::signals2::connection SceneInterface::connect(const image_ready_signal::slot_type &subscriber)
//{
//    return image_ready.connect(subscriber);
//}
//
//void SceneInterface::trigger(image_ptr image) {
//	image_ready(image);
//}

void SceneInterface::publish(const int frameid) {
	for( auto stream : streams)
		if(stream->image_ready.num_slots() > 0)
			try {
				stream->image_ready(frameSequence.getFrame(frameid)->getImageAsJPG(stream->name));
			} catch( std::exception& e ) {
			}
}
