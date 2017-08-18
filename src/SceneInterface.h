/*
 * SceneInterface.h
 *
 *  Created on: Aug 13, 2015
 *      Author: richard
 */

#ifndef SCENEINTERFACE_H_
#define SCENEINTERFACE_H_

#include <boost/core/noncopyable.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/thread/pthread/mutex.hpp>
#include <string>
#include <vector>

class FrameSequence;
class GUI_Interface;

class SceneInterface : private boost::noncopyable {
public:
	typedef boost::shared_ptr<std::vector<unsigned char>> image_ptr;
	typedef boost::signals2::signal<void (image_ptr)>  image_ready_signal;
	struct Stream {
		Stream(const std::string& name) : name(name) {}
		const std::string name;
		image_ready_signal image_ready;
	};

	SceneInterface(const std::string& displayName, const std::string& description, GUI_Interface& gui, FrameSequence& frameSequence);
	virtual ~SceneInterface();
	const std::string& getDescription() const;
	const std::string& getDisplayName() const;
	void addStream(const std::string& name);
	void publish(const int frameid);
//	boost::signals2::connection connect(const image_ready_signal::slot_type &subscriber);
//	void trigger(image_ptr image);

private:
	const std::string displayName;
	const std::string description;
	GUI_Interface& gui;
	FrameSequence& frameSequence;
	boost::mutex streams_list_mtx_;
	std::vector<boost::shared_ptr<Stream>> streams;

};

#endif /* SCENEINTERFACE_H_ */

