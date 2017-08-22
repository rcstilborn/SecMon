/*
 * WebSocketServer.cpp
 *
 *  Created on: Aug 4, 2015
 *      Author: richard
 */

#include "WebSocketServer.h"

#include <boost/asio/io_service.hpp>
#include <boost/bind/arg.hpp>
#include <boost/bind/bind.hpp>
#include <boost/bind/placeholders.hpp>
#include <boost/thread/lock_guard.hpp>
#include <rapidjson/document.h>
#include <stddef.h>
#include <websocketpp/close.hpp>
#include <websocketpp/endpoint.hpp>
#include <websocketpp/frame.hpp>
#include <websocketpp/impl/endpoint_impl.hpp>
#include <websocketpp/logger/levels.hpp>
#include <websocketpp/transport/asio/endpoint.hpp>
#include <glog/logging.h>
#include <iostream>
#include <stdexcept>
#include <utility>

using namespace boost::placeholders;

WebSocketServer::WebSocketServer(boost::asio::io_service& io_service)
: io_service(io_service),
  server(),
  shuttingDown(false),
  master_list(),
  master_list_mtx(),
  rooms(),
  rooms_mtx()
  {
    server.set_reuse_addr(true);
    server.set_message_handler(boost::bind(&WebSocketServer::on_message, this, _1, _2));
    server.set_close_handler(boost::bind(&WebSocketServer::on_close, this, _1));
    server.set_open_handler(boost::bind(&WebSocketServer::on_open, this, _1));
    server.clear_access_channels(websocketpp::log::alevel::frame_header | websocketpp::log::alevel::frame_payload);
    // this will turn off console output for frame header and payload

    //    server.clear_access_channels(websocketpp::log::alevel::all);
    // this will turn off everything in console output

    server.init_asio(&io_service);
    server.listen(9002);
}

WebSocketServer::~WebSocketServer() {
    DLOG(INFO) << "WebSocketServer closing connections";
//    server.stop_listening();
    for(auto hdl : master_list) {
        server.close(hdl,websocketpp::close::status::normal,"shutdown");
    }
}

void WebSocketServer::start() {
    server.start_accept();
    DLOG(INFO) << "WebSocketServer accepting connections";
}

void WebSocketServer::shutdown() {
    DLOG(INFO) << "WebSocketServer::shutdown closing connections";
    shuttingDown = true;
    for(auto hdl : master_list) {
        server.close(hdl,websocketpp::close::status::normal,"shutdown");
    }
    boost::lock_guard<boost::mutex> guard(master_list_mtx);
    master_list.clear();
    DLOG(INFO) << "WebSocketServer::shutdown connections closed";
}

void WebSocketServer::on_message(websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg) {
    DLOG(INFO) << "Got message: " << msg->get_payload();
    rapidjson::Document doc;
    doc.Parse(msg->get_payload().data());

    rapidjson::Value& r = doc["room"];
    std::string roomName = std::string(r.GetString());
    DLOG(INFO) << "room: " << roomName;

    rapidjson::Value& d = doc["data"];
    std::string data = std::string(d.GetString());
    DLOG(INFO) << "data: " << data;

    if(data == "join")
        joinRoom(hdl, roomName);
    else if(data == "leave")
        leaveRoom(hdl, roomName);

}

void WebSocketServer::on_open(websocketpp::connection_hdl hdl) {
    DLOG(INFO) << "New connection";
    if(shuttingDown) {
        DLOG(INFO) << "Rejecting new connection";

    }
    boost::lock_guard<boost::mutex> guard(master_list_mtx);
    master_list.insert(hdl);
}

void WebSocketServer::on_close(websocketpp::connection_hdl hdl) {
    DLOG(INFO) << "Connection closed";
    boost::lock_guard<boost::mutex> guard(master_list_mtx);
    master_list.erase(hdl);
}

void WebSocketServer::createRoom(const std::string& roomName, boost::function<const std::string()> welcomeMessageProvider){
    // Check if we already have this room
    room_list::iterator it = rooms.find(roomName);
    if (it != this->rooms.end())
        throw std::invalid_argument("A room with this name already exists: " + roomName);

    // Create the room
    boost::lock_guard<boost::mutex> guard(rooms_mtx);
    room new_room;
    new_room.welcomeMessageProvider = welcomeMessageProvider;
    this->rooms[roomName] = new_room;

}

//void WebSocketServer::deleteRoom(const std::string& roomName){
//    //    boost::lock_guard<boost::mutex> guard(mtx);
//
//}

int WebSocketServer::howManyInRoom(const std::string& roomName) const {
    room_list::const_iterator it = rooms.find(roomName);
    if (it == this->rooms.end())
        throw std::invalid_argument("No room with this name exists: " + roomName);
    return it->second.occupants.size();
}

void WebSocketServer::sendToRoom(const std::string& roomName, const std::string& data){
    room_list::iterator it = findRoom(roomName);
    for(auto hdl : it->second.occupants) {
        server.send(hdl, "{room: " + roomName + ", data: " + data + "}",websocketpp::frame::opcode::TEXT);
    }
}

void WebSocketServer::sendToRoom(const std::string& roomName, void const * data, const int size){
    room_list::iterator it = findRoom(roomName);
    for(auto hdl : it->second.occupants) {
        DLOG(INFO) << "Sending to room " << roomName << " binary data of size " << size;
        server.send(hdl, data, size, websocketpp::frame::opcode::BINARY);
    }
}

void WebSocketServer::sendToOne(websocketpp::connection_hdl hdl, const std::string& roomName, const std::string& data){
    //room_list::iterator it = findRoom(roomName);


    con_msg_man_type::ptr manager = websocketpp::lib::make_shared<con_msg_man_type>();
    size_t foo = 1024;
    message_type::ptr msg = manager->get_message(websocketpp::frame::opcode::TEXT,foo);

    msg.get()->set_payload("{room: ");
    msg.get()->append_payload(roomName);
    msg.get()->append_payload("}, {data:");
    msg.get()->append_payload(data);
    msg.get()->append_payload("}");
    server.send(hdl,msg);
    //    websocketpp::server<websocketpp::config::asio>::message_ptr msg = server.
}

void WebSocketServer::joinRoom(websocketpp::connection_hdl hdl, const std::string& roomName){
    room_list::iterator it = findRoom(roomName);
    boost::lock_guard<boost::mutex> guard(rooms_mtx);
    it->second.occupants.insert(hdl);
    sendToOne(hdl, roomName, it->second.welcomeMessageProvider());
}

void WebSocketServer::leaveRoom(websocketpp::connection_hdl hdl, const std::string& roomName){
    room_list::iterator it = findRoom(roomName);
    boost::lock_guard<boost::mutex> guard(rooms_mtx);
    it->second.occupants.erase(hdl);
}

WebSocketServer::room_list::iterator WebSocketServer::findRoom(const std::string& roomName) {
    room_list::iterator it = rooms.find(roomName);
    if (it == this->rooms.end())
        throw std::invalid_argument("No room with this name exists: " + roomName);
    return it;
}

