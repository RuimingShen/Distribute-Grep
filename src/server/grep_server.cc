#include "../../include/server/grep_server.hh"

#include <functional>

#include <boost/bind/bind.hpp>
#include <boost/make_shared.hpp>

namespace ba = boost::asio;

namespace server
{

// start_accept() will spawn a GrepServerSession object to handle this connection and return immediately
void GrepServer::start_accept()
{
  auto new_session = boost::make_shared<GrepServerSession>(_io_context, _log_dir_path);

  _acceptor.async_accept(
      new_session->get_socket(),
      boost::bind(
        &GrepServer::handle_accept,
        this,
        new_session,
        ba::placeholders::error
      )
  );
}

// Initiate the session to interact with the client and calls start_accept to accept new incoming connections
void GrepServer::handle_accept(GrepServerSessionPtr session, boost::system::error_code const& error)
{
  if (!error)
  {
    // to avoid delaying sending small packets
    // boost::asio::ip::tcp::no_delay option(true);
    // session->get_socket().set_option(option);

    session->start_interact();
  }

  start_accept();
}
} // namespace grep_server
