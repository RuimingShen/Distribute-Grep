#include "../../include/server/grep_server.hh"

#include <iostream>
#include <string>

#include <boost/asio.hpp>

namespace ba = boost::asio;

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cout << "Usage: query_client <log_dir>\n";
    return 0;
  }

  std::string log_dir = argv[1];

  try
  {
    ba::io_context io_context;
    server::GrepServer server(io_context, std::move(log_dir));

    io_context.run();
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}