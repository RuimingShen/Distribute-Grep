#ifndef SESSION_HH
#define SESSION_HH

#include <iostream>
#include <string>

#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

// This idea and code is reference from C++ Crash Course by Josh Lospinoso in Chapter 20
namespace session 
{
/* Session represents a connection established between a server and client.
 * Servers and clients can inherit from this generic session class to customize their own writing and reading behaviour
 */
class Session : public boost::enable_shared_from_this<Session>
{
public:
  // This is for server's socket
  explicit Session(boost::asio::io_context& io_context)
  : _socket{io_context} {}

  // This is for client's socket
  explicit Session(boost::asio::io_context& io_context, boost::asio::ip::tcp::endpoint endpoint)
  : _socket{io_context} 
  {
    _socket.connect(endpoint);
  }
  
  // The default behaviour is to asynchronously read from socket
  virtual void read_async();
  // read_async() will call handle_read. 
  // This is the specified read behaviour that the derived class needs to overwrite
  virtual void handle_read(boost::system::error_code const& ec, size_t length) = 0;

  // The default behaviour is to asynchronously write "msg" to socket
  virtual void write_async(std::string msg);
  // write_async() is an async function that calls handle_write
  // This is the specified write behaviour that the derived class needs to overwrite
  virtual void handle_write(boost::system::error_code const& ec, size_t length) = 0;

  // Getter functions
  boost::asio::ip::tcp::socket& get_socket() 
  {
    return _socket;
  }

protected:
  std::string _msg_to_session;
  std::string _msg_from_session;

private:
  boost::asio::ip::tcp::socket _socket;
};

using SessionPtr = boost::shared_ptr<Session>;
} // namespace grep_server

#endif // SESSION_HH
