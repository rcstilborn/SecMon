/*
 * SceneMonitor.h
 *
 *  Created on: Jul 27, 2015
 *      Author: richard
 */

#ifndef SCENEMONITOR_H_
#define SCENEMONITOR_H_

#include <boost/ptr_container/ptr_map.hpp>
#include <boost/thread/pthread/mutex.hpp>
#include <boost/core/noncopyable.hpp>
#include <string>

#include "Scene.h"

class Scene;

//struct scene_details {
//	std::string name;
//	unsigned short int height;
//	unsigned short int width;
//};

class SceneMonitor : private boost::noncopyable {
public:
	SceneMonitor(boost::asio::io_service& io_service, GUI_Interface& gui);
	virtual ~SceneMonitor();
	void startMonitoring(const std::string& name, const std::string& url);
	void stopMonitoring(const std::string&);
	void stopAllMonitoring();
	const std::string getSceneNames() const;
	//	std::vector<scene_details> getSceneDetails();
	void togglePause();
	void setFPS(const int fps);

private:
	boost::asio::io_service& io_service;
	GUI_Interface& gui;
	boost::ptr_map<std::string,Scene> scenes;
	boost::mutex scenes_mtx;

};

#endif /* SCENEMONITOR_H_ */
