/*
 * GUIInterface.cpp
 *
 *  Created on: Jul 29, 2015
 *      Author: richard
 */

#include "GUI_Interface.h"
#include <glog/logging.h>
#include <iostream>


GUI_Interface::GUI_Interface(boost::asio::io_service& io_service)
: io_service(io_service),
  webSocketServer_(io_service),
  streamDirectory_(io_service),
  httpServer_(io_service,/* address, port, doc_root,*/ streamDirectory_) {
//	std::cout << "GUI_Interface - constructed" << std::endl;

}

GUI_Interface::~GUI_Interface() {
}

void GUI_Interface::start() {
	httpServer_.start();
	webSocketServer_.start();
}
void GUI_Interface::sendCameraList(const std::string& list){
	webSocketServer_.sendToRoom("cameras", list);
}

void GUI_Interface::shutdown() {
	webSocketServer_.shutdown();
//	httpServer_.shutdown();
}

void GUI_Interface::registerNewStream(SceneInterface::Stream& stream) {
	streamDirectory_.addStream(stream);
}


// TODO Refactor away later
void GUI_Interface::createRoom(const std::string& roomName, boost::function<const std::string()> welcomeMessageProvider) {
	webSocketServer_.createRoom(roomName, welcomeMessageProvider);
}

void GUI_Interface::sendToRoom(const std::string& roomName, void const * data, const int size){
	webSocketServer_.sendToRoom(roomName, data, size);
}


//void GUI_Interface::received(boost::array<char, 1024ul> const& data){
//	std::cout << "GUI_Interface::received - enter" << std::endl;
//}

void GUI_Interface::newConnection(){
	DLOG(INFO) << "GUI_Interface::newConnection - enter";
	sendSceneList();
}

void GUI_Interface::sendSceneList() {
//	std::vector<std::string> names = this->sceneMonitor.getSceneNames();
//
//	rapidjson::StringBuffer sb;
//	rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
//
//    writer.StartObject();
//    writer.String("list");
//    writer.StartArray();
//	for(auto name: names){
////	    writer.String("name");
//	    writer.String(name.data());
//	}
//    writer.EndArray();
//    writer.EndObject();
//
//    socketServer.write("sl", sb.GetString(),sb.GetSize());
}
