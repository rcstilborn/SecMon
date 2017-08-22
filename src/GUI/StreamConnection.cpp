/*
 * StreamConnection.cpp
 *
 *  Created on: Aug 10, 2015
 *      Author: richard
 */

#include "StreamConnection.h"

#include <boost/asio/basic_io_object.hpp>
#include <boost/asio/basic_socket.hpp>
#include <boost/asio/basic_stream_socket.hpp>
#include <boost/asio/detail/wrapped_handler.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/socket_base.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind/arg.hpp>
#include <boost/bind/bind.hpp>
#include <boost/bind/placeholders.hpp>
#include <boost/lexical_cast.hpp>
#include <glog/logging.h>
#include <algorithm>
#include <iostream>
#include <string>

#include "Header.h"
#include "Stream.h"

namespace http {

#define BOUNDARYSTRING "--BOUNDARYSTRING"
const std::string FrameHeader("\r\n--BOUNDARYSTRING\r\nContent-type: image/jpeg\r\nContent-Length: ");
const char crlf[] = { '\r', '\n' };

StreamConnection::StreamConnection(http::connection_ptr conn, Stream& stream)
: strand_(conn->socket().get_io_service()),
  socket_(std::move(conn->socket())),
  stream_(stream),
  stream_signal_connection_(),
  buffer_(),
  request_(conn->getRequest()),
  reply_(),
  outbox_(){
//    std::cout << "StreamConnection::StreamConnection - constructed - " << stream.getStreamId() << std::endl;
}

StreamConnection::~StreamConnection() {
//    boost::system::error_code ignored_ec;
//    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
//    stream_.deregisterConnection(shared_from_this());
//    scene_connection_.disconnect();

//    std::cout << "StreamConnection::~StreamConnection - destructed" << std::endl;
}

void StreamConnection::start() {
    sendInitialHeaders();
    stream_signal_connection_ = stream_.connectToSignal(strand_.wrap(boost::bind(&StreamConnection::sendFrame, shared_from_this(), _1)));
}

void StreamConnection::sendInitialHeaders() {

    // Fill out the reply to be sent to the client.
    //    std::cout << "StreamConnection::sendInitialHeaders - preparing headers" << std::endl;

    Reply reply;
    reply.status = Reply::ok;
    reply.headers.push_back(Header("Connection", "Keep-Alive"));
    reply.headers.push_back(Header("Max-Age", "0"));
    reply.headers.push_back(Header("Expires", "0"));
    reply.headers.push_back(Header("Cache-Control", "no-cache"));
    reply.headers.push_back(Header("Pragma", "no-cache"));
    reply.headers.push_back(Header("Content-Type", "multipart/x-mixed-replace; boundary=" BOUNDARYSTRING));
    message_ptr msg_ptr(new FrameToSend);
    outbox_.push_back(msg_ptr);
    boost::asio::async_write(socket_, reply.to_buffers(),
            strand_.wrap(
                    boost::bind(&StreamConnection::handle_write, shared_from_this(),
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred)));

//    std::cout << "StreamConnection::sendInitialHeaders - sent headers" << std::endl;
}

void StreamConnection::sendFrame(SceneInterface::image_ptr image){
//        std::cout << "StreamConnection::sendFrame - sending image" << std::endl;

    message_ptr msg_ptr(new FrameToSend);
    msg_ptr->buffers_.push_back(boost::asio::buffer(FrameHeader));
    msg_ptr->buffers_.push_back(boost::asio::buffer(boost::lexical_cast<std::string>(image->size())));
    msg_ptr->buffers_.push_back(boost::asio::buffer(crlf));
    msg_ptr->buffers_.push_back(boost::asio::buffer(crlf));
    msg_ptr->buffers_.push_back(boost::asio::buffer(*image));
//    msg_ptr->buffers_.push_back(boost::asio::buffer("\r\n--BOUNDARYSTRING\r\n"));
    msg_ptr->ptr_ = image;
    strand_.post(boost::bind(&StreamConnection::addToOutbox, shared_from_this(), msg_ptr));

//    std::cout << "StreamConnection::sendFrame - sent frame " << boost::lexical_cast<std::string>(image->size()) << stream_.getStreamId() << std::endl;

}

void StreamConnection::handle_read(const boost::system::error_code& e,
        std::size_t bytes_transferred)
{
    if (e) {
        LOG(WARNING) << "StreamConnection::handle_read - got " << e.message() << "and bytes transferred " << bytes_transferred;
    }
    if (e == boost::asio::error::eof  || e == boost::asio::error::connection_reset) {
        boost::system::error_code ignored_ec;
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
        stream_.deregisterConnection(shared_from_this());
    } else {
        socket_.async_read_some(boost::asio::buffer(buffer_),
                strand_.wrap(
                        boost::bind(&StreamConnection::handle_read, shared_from_this(),
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred)));

    }
}


void StreamConnection::addToOutbox(const message_ptr& msg_ptr) {
    outbox_.push_back( msg_ptr );
    if ( outbox_.size() > 1 ) {
//        std::cout << "Got a new message but one is already pending" << std::endl;
        // outstanding async_write
        return;
    }
    this->writeNextMessage();
}

void StreamConnection::writeNextMessage()
{
    //    const message& msg = *outbox_[0];
    if( !outbox_.empty()) {
        boost::asio::async_write(socket_, outbox_[0]->buffers_,
                strand_.wrap(
                        boost::bind(&StreamConnection::handle_write, shared_from_this(),
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred)));
//        std::cout << "Sending next message" << std::endl;
    }
}

void StreamConnection::handle_write(const boost::system::error_code& e,
        std::size_t bytes_transferred){
    if( !outbox_.empty())
        outbox_.pop_front();
    if (e)
    {
        LOG(WARNING) << "StreamConnection::handle_write - got error from write operation " << e.message() << " and bytes transferred " << bytes_transferred;
        // Initiate graceful connection closure.
        stream_signal_connection_.disconnect();
        boost::system::error_code ignored_ec;
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
        stream_.deregisterConnection(shared_from_this());
        return;
    }
//    std::cout << "StreamConnection::handle_write - wrote something! " << bytes_transferred << std::endl;
    if( !outbox_.empty())
        writeNextMessage();
//    else
//        socket_.async_read_some(boost::asio::buffer(buffer_),
//                strand_.wrap(
//                        boost::bind(&StreamConnection::handle_read, shared_from_this(),
//                                boost::asio::placeholders::error,
//                                boost::asio::placeholders::bytes_transferred)));

}

} // namespace http


// ************ old sendFrame ****************
//        Reply reply;
//        reply.status = Reply::ok;
//
//    //    reply.content.clear();
//    //    reply.headers.clear();
//        reply.content.append(image->begin(), image->end());
//
//        reply.headers.push_back(Header("Content-type", "image/jpeg"));
//        reply.headers.push_back(Header("Content-Length", boost::lexical_cast<std::string>(reply.content.size())));
//        reply.content.append(BOUNDARYSTRING "\r\n");

//        std::vector<boost::asio::const_buffer> buffers = reply_.headers_to_buffers();
//        BOOST_FOREACH(const boost::asio::const_buffer& buff, reply_.content_to_buffers())
//        {
//            buffers.push_back(buff);
//        }
//        buffers.push_back(boost::asio::buffer(misc_strings::crlf));

//        message_ptr msg_ptr(new FrameToSend);
//        outbox_.push_back(msg_ptr);
//
//        boost::asio::async_write(socket_, reply.to_buffers(),
//                strand_.wrap(
//                        boost::bind(&StreamConnection::handle_write, shared_from_this(),
//                                boost::asio::placeholders::error,
//                                boost::asio::placeholders::bytes_transferred)));

//        std::cout << "reply_.content of size: " << msg_ptr->ptr_->size().content.size() << std::endl;
//        std::cout << "Content-Length: " << msg_ptr->buffers_[1] << std::endl;
//        std::cout << "image.size(): " << image->size() << std::endl;
// ************ old sendFrame ****************

//void StreamConnection::sendBoundary(const boost::system::error_code& e,
//        std::size_t bytes_transferred){
//    if (e)
//    {
//        std::cerr << "StreamConnection::sendBoundary - got error from write operation " << e.message() << std::endl;
//        // Initiate graceful connection closure.
//        boost::system::error_code ignored_ec;
//        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
//    }
//
//    //send the boundary synchronously
//    reply_.content = BOUNDARYSTRING;
//    strand_.post(boost::bind(&StreamConnection::addToOutbox, shared_from_this(), reply_.content_to_buffers()));
//    //    boost::asio::async_write(socket_, reply_.content_to_buffers(),
//    //            strand_.wrap(
//    //                    boost::bind(&StreamConnection::handle_write, shared_from_this(),
//    //                            boost::asio::placeholders::error,
//    //                            boost::asio::placeholders::bytes_transferred)));
//
//    std::cout << "StreamConnection::sendBoundary - sent the boundary! " << bytes_transferred << std::endl;
//
//    //    boost::asio::write(socket_, reply_.content_to_buffers());
//    //    socket_.async_read_some(boost::asio::buffer(buffer_),
//    //            strand_.wrap(
//    //                    boost::bind(&StreamConnection::handle_write, shared_from_this(),
//    //                            boost::asio::placeholders::error,
//    //                            boost::asio::placeholders::bytes_transferred)));
//
//}


