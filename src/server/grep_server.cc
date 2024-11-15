#include "../../include/server/grep_server.hh"

#include <functional>

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/process.hpp>

namespace ba = boost::asio;
namespace bp = boost::process;
using namespace session;
using namespace std;
namespace server
{

string GrepServerSession::execute_grep_cmd(string cmd) 
{
  bp::ipstream output_stream;
  bp::system(cmd.c_str(), bp::std_out > output_stream);

  string data;
  string line;

  while (getline(output_stream, line) && !line.empty())
  {
    data += line + "\n";
  }

  return data;
}

void GrepServerSession::handle_read(boost::system::error_code const& ec, size_t length [[maybe_unused]])
{
  if (ec || _msg_from_session == "\n") 
  {
    return;
  }

  if (!_msg_from_session.empty()) {
      _msg_from_session.pop_back();  // Removes the newline char
  }

  auto grep_cmd = _msg_from_session + " " + _log_dir_path;
  auto res = execute_grep_cmd(move(grep_cmd));

  write_async(res);
}

void GrepServerSession::handle_write(boost::system::error_code const& ec, size_t length [[maybe_unused]])
{
  if (ec) 
  {
    return;
  }

  _msg_to_session.clear();
  read_async();
}

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

void GrepServer::handle_accept(SessionPtr session, boost::system::error_code const& error)
{
  if (!error)
  {
    // to avoid delaying sending small packets
    boost::asio::ip::tcp::no_delay option(true);
    session->get_socket().set_option(option);

    session->read_async();
  }

  start_accept();
}
} // namespace grep_server


int main(int argc, char** argv)
{
  if (argc < 2)
  {
    cout << "Usage: query_client <log_dir>\n";
    return 0;
  }

  string log_dir = argv[1];

  try
  {
    ba::io_context io_context;
    server::GrepServer server(io_context, move(log_dir));

    io_context.run();
  }
  catch (exception& e)
  {
    cerr << e.what() << endl;
  }

  return 0;
}

