/*
 * Scene.cpp
 *
 *  Created on: Jul 28, 2015
 *      Author: richard
 */

#include "Scene.h"

#include <boost/bind/arg.hpp>
#include <boost/bind/bind.hpp>
#include <boost/bind/placeholders.hpp>
#include <iostream>
#include <glog/logging.h>


Scene::Scene(const std::string& name, const std::string& url, boost::asio::io_service& io_service, GUI_Interface& gui, const int fps)
: name(name),
//  interval(1000/fps),
  frameSequence(),//camera.getHeight(), camera.getWidth()),
  sceneIf(name, "Description here", gui, frameSequence),
  ooiProcessor(io_service, sceneIf, frameSequence, boost::bind(&SceneInterface::publish, &sceneIf, _1)),
  imageProcessor(io_service, sceneIf, frameSequence, boost::bind(&OOI_Processor::processNextFrame, &ooiProcessor, _1)),
  imageSource(name, url, io_service, sceneIf, frameSequence, boost::bind(&ImageProcessor::processNextFrame, &imageProcessor, _1), fps)
{
	DLOG(INFO) << "Scene(" << name << ") - constructed";
}


Scene::~Scene() {
	//	std::cout << "~Scene() - enter" << std::endl;
	//	std::cout << "~Scene() - destructed " << count << std::endl;
}

FrameSequence& Scene::getFrameSequence() {
	return this->frameSequence;
}

const std::string& Scene::getName() const {
	return this->name;
}

void Scene::shutdown() {
	//	std::cout << "Scene::shutdown() " << name << std::endl;
	imageSource.shutdown();
}

void Scene::togglePause() {
	isPaused_ = !isPaused_;
	imageSource.togglePause();
	if(isPaused_)
		std::cout << "Pausing..." << std::endl;
	else
		std::cout << "Resuming..." << std::endl;
}

void Scene::setFPS(const int fps) {
	imageSource.setFPS(fps);
	DLOG(INFO) << "Setting FPS to " << fps;
}

