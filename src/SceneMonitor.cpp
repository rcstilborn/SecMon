/*
 * SceneMonitor.cpp
 *
 *  Created on: Jul 27, 2015
 *      Author: richard
 */

#include "SceneMonitor.h"

#include <boost/asio/io_service.hpp>
#include <boost/bind/bind.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/ptr_container/detail/associative_ptr_container.hpp>
#include <boost/ptr_container/detail/reversible_ptr_container.hpp>
#include <boost/ptr_container/ptr_map_adapter.hpp>
#include <boost/thread/lock_guard.hpp>
#include <chrono>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <utility>

#include "GUI/GUI_Interface.h"

SceneMonitor::SceneMonitor(boost::asio::io_service& io_service, GUI_Interface& gui) : io_service(io_service), gui(gui), scenes(), scenes_mtx() {
	//	std::cout << "SceneMonitor() - constructed" << std::endl;
	gui.createRoom("cameras", boost::bind<const std::string>(&SceneMonitor::getSceneNames, this));
}

SceneMonitor::~SceneMonitor() {
	boost::lock_guard<boost::mutex> guard(scenes_mtx);
	for (boost::ptr_map<std::string,Scene>::iterator it = scenes.begin(); it != scenes.end(); ++it)
		scenes.erase(it);
//	std::cout << "~SceneMonitor() - destructed" << std::endl;
}

void SceneMonitor::startMonitoring(const std::string& name, const std::string& url){
//	std::cout << "SceneMonitor.startMonitoring() - enter" << std::endl;

	// Check if we already have this name
	boost::ptr_map<std::string, Scene>::iterator it = scenes.find(name);
	if (it != this->scenes.end())
		throw std::invalid_argument("A scene with this name already exists: " + name);

	// Create the scene
	{
		Scene* scene = new Scene(name, url, io_service, gui, 10);
		boost::lock_guard<boost::mutex> guard(scenes_mtx);
		// TODO Fix this silliness!
		std::string foo(name);
		this->scenes.insert(foo, scene);
	}
//	gui.addStream(1);
//	std::cout << "SceneMonitor.startMonitoring() - exit - " << getSceneNames() << std::endl;
}

void SceneMonitor::stopMonitoring(const std::string& name) {
	std::cout << "SceneMonitor.stopMonitoring() - enter" << std::endl;

	boost::ptr_map<std::string, Scene>::iterator it = scenes.find(name);
	if (it != this->scenes.end()){
		boost::lock_guard<boost::mutex> guard(scenes_mtx);
		it->second->shutdown();
	}

//	std::cout << "SceneMonitor.stopMonitoring() - exit" << std::endl;
}

void SceneMonitor::stopAllMonitoring() {
	std::cout << "SceneMonitor.stopAllMonitoring() - enter" << std::endl;

	for (auto scene : this->scenes)
		scene.second->shutdown();

	std::this_thread::sleep_for(std::chrono::seconds(2));
	//std::cout << "SceneMonitor.stopAllMonitoring() - exit" << std::endl;
}

const std::string SceneMonitor::getSceneNames() const {
	std::stringstream list;
	const char* separator = "";

	list << '[';
	for(auto name: this->scenes) {
		list << separator << name.first;
		separator = ",";
	}
	list << ']';

	return list.str();
}

//std::vector<scene_details> SceneMonitor::getSceneDetails(){
//	std::vector<scene_details> details;
//	for(auto entry: this->scenes){
//		scene_details detail;
//		detail.name = entry.second->getName();
//		detail.height = entry.second->getCamera().getHeight();
//		detail.width = entry.second->getCamera().getWidth();
//		details.push_back(detail);
//	}
//	return details;
//}

void SceneMonitor::togglePause() {
//	std::cout << "SceneMonitor.togglePause() - enter" << std::endl;

	for (auto scene : this->scenes)
		scene.second->togglePause();

//	std::cout << "SceneMonitor.togglePause() - exit" << std::endl;
}

void SceneMonitor::setFPS(const int fps) {
	for (auto scene : this->scenes)
		scene.second->setFPS(fps);
}
