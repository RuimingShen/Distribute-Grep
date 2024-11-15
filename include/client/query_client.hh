#ifndef QUERY_CLIENT_HH
#define QUERY_CLIENT_HH

#include "../session/session.hh"

#include <boost/asio.hpp>

namespace client
{

class QueryClientSession : public session::Session
{
public:
  explicit QueryClientSession(boost::asio::io_context& io_context, boost::asio::ip::tcp::endpoint endpoint)
  : session::Session{io_context, endpoint} 
  {
    // to avoid delaying sending small packets
    boost::asio::ip::tcp::no_delay option(true);
    get_socket().set_option(option);
  }

  // override functions
  void handle_read(boost::system::error_code const& ec, size_t length) override;
  void handle_write(boost::system::error_code const& ec, size_t length) override;
};

class QueryClient 
{
public:
  explicit QueryClient(boost::asio::io_context& io_context, std::string grep_cmd)
  : _io_context{io_context}, _grep_cmd{std::move(grep_cmd)}
  {
    start_query();
  }

private:
  // Asynchronous functions that sends the query to all nodes asynchronously
  void start_query();

  boost::asio::io_context& _io_context;
  std::string _grep_cmd; // the command to be relayed to the servers
};
} // namespace client

#endif // QUERY_CLIENT_HH
