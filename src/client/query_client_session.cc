#include "../../include/client/query_client_session.hh"

#include <iostream>
#include <filesystem>
#include <fstream>

#include <boost/bind/bind.hpp>

namespace ba = boost::asio;

const char PORT[] = "50000";

namespace client
{
// Step 1.
// The entry function, resolve the ip addr of the given host.
// Invoke handle_resolve when done
void QueryClientSession::start_connect()
{    
  ba::ip::tcp::resolver::query query(_host.c_str(), PORT);

  _resolver.async_resolve(query,
                          boost::bind(&QueryClientSession::handle_resolve,
                                      this,
                                      ba::placeholders::error,
                                      ba::placeholders::results
                                     )
                         );
}

// Step 2.
// After ip_addr:port is resolved, connect to the ip_addr:port
// Invoke handle_connect when done connecting
void QueryClientSession::handle_resolve(boost::system::error_code const& ec, const ResolveResult& results)
{
  if (ec) 
  {
    _err_callback(ErrorMsg(_host, ec.message()));
    return;
  }

  ba::async_connect(_socket,
                    results,
                    boost::bind(&QueryClientSession::handle_connect,
                                this,
                                ba::placeholders::error,
                                ba::placeholders::endpoint
                    )
  );
}

// Step 3.
// After connection to the host is established, send the grep command to the host
// Invoke handle_write after command is sent
void QueryClientSession::handle_connect(boost::system::error_code const& ec, const Endpoint& results)
{
  if (ec) 
  {
    _err_callback(ErrorMsg(_host, ec.message()));
    return;
  }

  // to avoid delaying sending small packets
  // ba::ip::tcp::no_delay option(true);
  // get_socket().set_option(option);

  std::string grep_cmd_with_endmark = _grep_cmd + END_MARK; // add end mark to indicate the end

  ba::async_write(_socket, 
                  ba::buffer(std::move(grep_cmd_with_endmark)), 
                  boost::bind(&QueryClientSession::handle_write,
                              shared_from_this(),
                              ba::placeholders::error,
                              ba::placeholders::bytes_transferred)
  
  );
}

// Step 4.
// After sending the command to the host, read the result from host.
// Invoke async_read_chunk to read the data from server
void QueryClientSession::handle_write(boost::system::error_code const& ec, size_t length [[maybe_unused]])
{
  if (ec) 
  {
    _err_callback(ErrorMsg(_host, ec.message()));
    return;
  }

  async_read_chunk();
}

// Step 5.
// Read the result from host chunk by chunk
// Splitting into chunks of data can overcome the usage of very large buffers
// Invoke handle_read_chunk to process the chunk of data 
void QueryClientSession::async_read_chunk()
{
  _socket.async_read_some(ba::buffer(_buffer, BUFFER_SIZE),
                          boost::bind(&QueryClientSession::handle_read_chunk,
                                      this,
                                      ba::placeholders::error,
                                      ba::placeholders::bytes_transferred
                         )
  );
}
// Handle a particular chunk of data
/* There are 2 cases: 
 *  1. There is no end mark in this chunk:
 *     Append the data to _output_file and invoke async_read_chunk to read in more data
 *  2. There is an end mark in this chunk:
 *     Append that data (without end mark) to _output_file and notify client
 */
void QueryClientSession::handle_read_chunk(boost::system::error_code const& ec, size_t length)
{
  if (!ec)
  {
    std::string chunk(_buffer.data(), length);

    auto end_mark_pos = chunk.find(END_MARK);

    if (end_mark_pos == std::string::npos)
    {
      // Case 1: No end mark found
      append_to_output_file(std::move(chunk));
      // read more data in
      async_read_chunk();
    }
    else
    {
      // Remove the end mark
      chunk = chunk.substr(0, end_mark_pos);
      append_to_output_file(std::move(chunk));

      auto lc = countLines(get_output_filepath());
      // notify client
      _success_callback(Statistics(_host, _grep_cmd, lc));
    }
  }
}

/*============================================Utility functions=============================================================*/
// Write the matched lines into {host}.log file
void QueryClientSession::append_to_output_file(std::string lines)
{
  if (!std::filesystem::exists(_output_dir)) 
  {
    // create output dir if does not exist
    std::filesystem::create_directories(_output_dir);
  }

  const auto file = get_output_filepath();

  std::ofstream file_stream(file, std::ios::app);
  
  file_stream << std::move(lines);
  file_stream.close();
}

// Split a string using a delimiter
// Use to count the number of lines received
std::vector<std::string> QueryClientSession::split(std::string s, const std::string& delimiter) 
{
  std::vector<std::string> tokens;
  size_t pos = 0;
  std::string token;
  
  while ((pos = s.find(delimiter)) != std::string::npos) {
      token = s.substr(0, pos);

      if (!token.empty() && token != "\r")
      {
        // only push in non-empty string
        tokens.push_back(token);
      }
      
      s.erase(0, pos + delimiter.length());
  }

  if (!s.empty() && s != END_MARK)
  {
    tokens.push_back(s);
  }

  return tokens;
}

// Count the number of lines of a file
long QueryClientSession::countLines(const std::string& filename) 
{
  std::ifstream file(filename);
  long lineCount = 0;

  std::string line;
  while (std::getline(file, line)) 
  {
      ++lineCount; // Increment line count for each line read
  }

  // Close the file
  file.close();

  return lineCount;
}

// Remove file if exist
void QueryClientSession::remove_file(const std::string filepath)
{
  if (std::filesystem::exists(filepath)) 
  {
    std::filesystem::remove(filepath);
  }
}
} // namespace client
