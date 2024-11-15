#include "../../include/session/session.hh"

#include <boost/bind.hpp>

using namespace boost::asio;

namespace session
{

void Session::read_async()
{
  _msg_from_session.clear();
  async_read_until(_socket, 
                   dynamic_buffer(_msg_from_session), 
                   '\n',
                   boost::bind(&Session::handle_read,
                                shared_from_this(),
                                placeholders::error,
                                placeholders::bytes_transferred)
  
  );
}

void Session::write_async(std::string msg)
{
  _msg_to_session = std::move(msg);
  // boost::asio::write(_socket, buffer(_msg));
  async_write(_socket, 
              buffer(_msg_to_session), 
              boost::bind(&Session::handle_write,
                          shared_from_this(),
                          placeholders::error,
                          placeholders::bytes_transferred)
  
  );
}

} // namespace session
