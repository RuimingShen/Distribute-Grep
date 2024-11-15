#ifndef GREP_SERVER_HH
#define GREP_SERVER_HH

#include "../../include/server/grep_server_session.hh"

#include <iostream>
#include <string>

#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string/case_conv.hpp>

namespace server 
{
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
  void handle_accept(GrepServerSessionPtr session, boost::system::error_code const& error);

  boost::asio::io_context& _io_context;
  boost::asio::ip::tcp::acceptor _acceptor;
  std::string _log_dir_path; // path to log directory, i.e. the target for the search
};

} // namespace grep_server

#endif // GREP_SERVER_HH
