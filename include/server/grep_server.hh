#ifndef GREP_SERVER_HH
#define GREP_SERVER_HH

#include "session.hh"

#include <iostream>
#include <string>

#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string/case_conv.hpp>

// This idea and code is reference from C++ Crash Course by Josh Lospinoso in Chapter 20
namespace server 
{

class GrepServerSession : public session::Session
{
public:
  explicit GrepServerSession(boost::asio::io_context& io_context, std::string log_dir)
  : session::Session{io_context}, _log_dir_path{std::move(log_dir)} 
  {}

  std::string execute_grep_cmd(std::string cmd);

  // override functions
  void handle_read(boost::system::error_code const& ec, size_t length) override;
  void handle_write(boost::system::error_code const& ec, size_t length) override;

private:
  std::string _log_dir_path;
};

class GrepServer 
{
public:
  explicit GrepServer(boost::asio::io_context& io_context, std::string log_dir)
  : _io_context{io_context}, 
    _acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 50000)),
    _log_dir_path{std::move(log_dir)}
  {
    start_accept();
  }

private:
  // Asynchronous functions that keeps accepting new connection and creates new session
  void start_accept();
  void handle_accept(session::SessionPtr session, boost::system::error_code const& error);

  boost::asio::io_context& _io_context;
  boost::asio::ip::tcp::acceptor _acceptor;
  std::string _log_dir_path; // path to log directory, i.e. the target for the search
};

} // namespace grep_server

#endif // GREP_SERVER_HH
