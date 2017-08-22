/*
 * SceneMonitor.cpp
 *
 *  Created on: Jul 27, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
*/

#include "SceneMonitor.h"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>

#include <boost/asio/io_service.hpp>
#include <boost/bind/bind.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/ptr_container/detail/associative_ptr_container.hpp>
#include <boost/ptr_container/detail/reversible_ptr_container.hpp>
#include <boost/ptr_container/ptr_map_adapter.hpp>
#include <boost/thread/lock_guard.hpp>

#include "GUI/GUI_Interface.h"

SceneMonitor::SceneMonitor(boost::asio::io_service& io_service, GUI_Interface& gui)
  : io_service_(io_service), gui_(gui), scenes_(), scenes_mtx_() {
    //    std::cout << "SceneMonitor() - constructed" << std::endl;
    gui_.createRoom("cameras", boost::bind<const std::string>(&SceneMonitor::get_scene_names, this));
}

SceneMonitor::~SceneMonitor() {
    boost::lock_guard<boost::mutex> guard(scenes_mtx_);
    for (boost::ptr_map<std::string, Scene>::iterator it = scenes_.begin(); it != scenes_.end(); ++it)
        scenes_.erase(it);
//    std::cout << "~SceneMonitor() - destructed" << std::endl;
}

void SceneMonitor::start_monitoring(const std::string& name, const std::string& url) {
//    std::cout << "SceneMonitor.startMonitoring() - enter" << std::endl;

    // Check if we already have this name
    boost::ptr_map<std::string, Scene>::iterator it = scenes_.find(name);
    if (it != this->scenes_.end())
        throw std::invalid_argument("A scene with this name already exists: " + name);

    // Create the scene
    {
        Scene* scene = new Scene(name, url, io_service_, gui_, 10);
        boost::lock_guard<boost::mutex> guard(scenes_mtx_);
        // TODO(richard): Fix this silliness!
        std::string foo(name);
        this->scenes_.insert(foo, scene);
    }
//    gui.addStream(1);
//    std::cout << "SceneMonitor.startMonitoring() - exit - " << getSceneNames() << std::endl;
}

void SceneMonitor::stop_monitoring(const std::string& name) {
    std::cout << "SceneMonitor.stopMonitoring() - enter" << std::endl;

    boost::ptr_map<std::string, Scene>::iterator it = scenes_.find(name);
    if (it != this->scenes_.end()) {
        boost::lock_guard<boost::mutex> guard(scenes_mtx_);
        it->second->shutdown();
    }

//    std::cout << "SceneMonitor.stopMonitoring() - exit" << std::endl;
}

void SceneMonitor::stop_all_monitoring() {
    std::cout << "SceneMonitor.stopAllMonitoring() - enter" << std::endl;

    for (auto scene : this->scenes_)
        scene.second->shutdown();

    std::this_thread::sleep_for(std::chrono::seconds(2));
    //std::cout << "SceneMonitor.stopAllMonitoring() - exit" << std::endl;
}

const std::string SceneMonitor::get_scene_names() const {
    std::stringstream list;
    const char* separator = "";

    list << '[';
    for (auto name : this->scenes_) {
        list << separator << name.first;
        separator = ",";
    }
    list << ']';

    return list.str();
}

//std::vector<scene_details> SceneMonitor::getSceneDetails(){
//    std::vector<scene_details> details;
//    for(auto entry: this->scenes){
//        scene_details detail;
//        detail.name = entry.second->getName();
//        detail.height = entry.second->getCamera().getHeight();
//        detail.width = entry.second->getCamera().getWidth();
//        details.push_back(detail);
//    }
//    return details;
//}

void SceneMonitor::toggle_pause() {
//    std::cout << "SceneMonitor.togglePause() - enter" << std::endl;

    for (auto scene : this->scenes_)
        scene.second->togglePause();

//    std::cout << "SceneMonitor.togglePause() - exit" << std::endl;
}

void SceneMonitor::set_frames_per_second(const int fps) {
    for (auto scene : this->scenes_)
        scene.second->setFPS(fps);
}
