/*
 * WebSocketServer.h
 *
 *  Created on: Aug 4, 2015
 *      Author: richard
 */

#ifndef WEBSOCKETSERVER_H_
#define WEBSOCKETSERVER_H_

#include <boost/function/function_fwd.hpp>
#include <boost/function.hpp>
#include <boost/thread/pthread/mutex.hpp>
#include <websocketpp/common/connection_hdl.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/message_buffer/alloc.hpp>
#include <websocketpp/message_buffer/message.hpp>
#include <websocketpp/roles/server_endpoint.hpp>
#include <cstdbool>
#include <map>
#include <memory>
#include <set>
#include <string>

class SceneMonitor;

class WebSocketServer {
public:
	explicit WebSocketServer(boost::asio::io_service& io_service);
	virtual ~WebSocketServer();

	void start();

	void createRoom(const std::string& roomName, boost::function<const std::string()> welcomeMessageProvider);
	void deleteRoom(const std::string& roomName);
	int howManyInRoom(const std::string& roomName) const;
	void sendToRoom(const std::string& roomName, const std::string& data);
	void sendToOne(websocketpp::connection_hdl hdl, const std::string& roomName, const std::string& data);
	void sendToRoom(const std::string& roomName, void const * data, const int size);
	void shutdown();

private:
	boost::asio::io_service& io_service;
	websocketpp::server<websocketpp::config::asio> server;

	bool shuttingDown;
	typedef std::set<websocketpp::connection_hdl,std::owner_less<websocketpp::connection_hdl>> occupant_list;
// This will all get refactored away when we move to libsourcey
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
	typedef struct {
		occupant_list occupants;
		boost::function<const std::string()> welcomeMessageProvider;
	} room;
#pragma GCC diagnostic pop
	typedef std::map<std::string,room> room_list;

    typedef websocketpp::message_buffer::message<websocketpp::message_buffer::alloc::con_msg_manager> message_type;
    typedef websocketpp::message_buffer::alloc::con_msg_manager<message_type> con_msg_man_type;
    occupant_list master_list;
	boost::mutex master_list_mtx;
	room_list rooms;
	boost::mutex rooms_mtx;
	void on_message(websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg);
	void on_open(websocketpp::connection_hdl hdl);
	void on_close(websocketpp::connection_hdl hdl);
	void joinRoom(websocketpp::connection_hdl hdl, const std::string& roomName);
	void leaveRoom(websocketpp::connection_hdl hdl, const std::string& roomName);
	room_list::iterator findRoom(const std::string& roomName);
};

#endif /* WEBSOCKETSERVER_H_ */
