/*
 * StreamConnection.h
 *
 *  Created on: Aug 10, 2015
 *      Author: richard
 */

#ifndef GUI_STREAMCONNECTION_H_
#define GUI_STREAMCONNECTION_H_

#include <boost/array.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/core/noncopyable.hpp>
#include <boost/signals2/connection.hpp>
#include <boost/smart_ptr/enable_shared_from_this.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <cstddef>
#include <deque>
#include <vector>

#include "../SceneInterface.h"
#include "Connection.h"
#include "Reply.h"
#include "Request.h"

class Stream;

namespace http {


class StreamConnection
          : public boost::enable_shared_from_this<StreamConnection>,
            private boost::noncopyable
{
public:

    StreamConnection(http::connection_ptr conn, Stream& stream);
    virtual ~StreamConnection();

    void start();

    void sendInitialHeaders();
    void sendFrame(SceneInterface::image_ptr image);

private:
      /// Handle completion of a read operation.
      void handle_read(const boost::system::error_code& e,
          std::size_t bytes_transferred);

      /// Handle completion of a write operation.
      void handle_write(const boost::system::error_code& e,
                std::size_t bytes_transferred);

      struct FrameToSend {
          FrameToSend(): buffers_(), ptr_() {
//              std::cout << "FrameToSend::FrameToSend() - constructed" << std::endl;
          }
//          ~FrameToSend() {
//              std::cout << "FrameToSend::~FrameToSend() - destructed" << std::endl;
//          }
          std::vector<boost::asio::const_buffer> buffers_;
          boost::shared_ptr<std::vector<unsigned char>> ptr_;
      };

      typedef boost::shared_ptr<FrameToSend> message_ptr;

      void addToOutbox(const message_ptr& msg_ptr);
      void writeNextMessage();

      /// Strand to ensure the connection's handlers are not called concurrently.
      boost::asio::io_service::strand strand_;

      /// Socket for the connection.
      boost::asio::ip::tcp::socket socket_;

      // The Stream we are dealig with
      Stream& stream_;

      // Connection to the scene signal
      boost::signals2::connection  stream_signal_connection_;

      /// The handler used to process the incoming request.
//      RequestHandler& request_handler_;

      /// Buffer for incoming data.
      boost::array<char, 8192> buffer_;

      /// The incoming request.
      Request request_;

      /// The parser for the incoming request.
//      RequestParser request_parser_;

      /// The reply to be sent back to the client.
      Reply reply_;

      // Outgoing queue
      typedef std::deque<message_ptr> Outbox;
      Outbox outbox_;

};

typedef boost::shared_ptr<StreamConnection> stream_connection_ptr;

} // namespace http

#endif /* GUI_STREAMCONNECTION_H_ */
