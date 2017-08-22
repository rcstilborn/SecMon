/*
 * GUIInterface.h
 *
 *  Created on: Jul 29, 2015
 *      Author: richard
 */

#ifndef GUIINTERFACE_H_
#define GUIINTERFACE_H_

#include <boost/array.hpp>
#include <boost/function/function_fwd.hpp>
#include <boost/function.hpp>
#include <string>

#include "../SceneInterface.h"
#include "HTTP_Server.h"
#include "StreamDirectory.h"
#include "WebSocketServer.h"

class SceneMonitor;
class SceneInterface;

class GUI_Interface {

public:
    explicit GUI_Interface(boost::asio::io_service& io_service);
    virtual ~GUI_Interface();

    void start();
    void registerNewStream(SceneInterface::Stream& stream);
    void shutdown();

    // TODO Refactor away later
    void sendToRoom(const std::string& roomName, void const * data, const int size);
    void createRoom(const std::string& roomName, boost::function<const std::string()> welcomeMessageProvider);
    void sendCameraList(const std::string& list);

private:
    boost::asio::io_service& io_service;
    WebSocketServer webSocketServer_;
    StreamDirectory streamDirectory_;
    http::HTTP_Server httpServer_;

    // TODO Refactor away later
    void received(const boost::array<char, 1024>&);
    void newConnection();
    void sendSceneList();
};

#endif /* GUIINTERFACE_H_ */
