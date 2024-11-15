#include "../../include/client/query_client.hh"

#include <boost/make_shared.hpp>

namespace ba = boost::asio;

namespace client
{

void QueryClientSession::handle_read(boost::system::error_code const& ec, size_t length [[maybe_unused]])
{
  if (ec || _msg_from_session == "\n") 
  {
    return;
  }

  std::cout << _msg_from_session << "\n";
}

void QueryClientSession::handle_write(boost::system::error_code const& ec, size_t length [[maybe_unused]])
{
  if (ec) 
  {
    return;
  }

  read_async();
}

void QueryClient::start_query()
{
  boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), 50000);
  auto new_session = boost::make_shared<QueryClientSession>(_io_context, endpoint);

  // send the grep command to the servers
  std::string grep_cmd_with_newline = _grep_cmd + "\n"; // add newline to indicate the end
  new_session->write_async(std::move(grep_cmd_with_newline));
}
} // namespace client


int main(int argc, char** argv) {
  std::cout << "Please enter the grep command:\n";
  
  std::string grep_command;
  std::getline(std::cin, grep_command);

  try 
  {
    ba::io_context io_context;
    client::QueryClient client(io_context, std::move(grep_command));

    io_context.run();
  } 
  catch(boost::system::system_error& se) 
  {
    std::cerr << "Error: " << se.what() << std::endl;
  }
}
