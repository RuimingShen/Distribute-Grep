#include "../../include/server/grep_server_session.hh"

#include <cstdio>
#include <iostream>

#include <boost/bind/bind.hpp>
#include <boost/process.hpp>

namespace ba = boost::asio;
namespace bp = boost::process;

namespace server
{

/* After start_interact is being invoked, the following steps will happen in order:
 * 1. Reading in grep command from the client
 * 2. Execute the command and forward the result to client
 * 3. Read in next command (if available)
 */
void GrepServerSession::start_interact()
{
  ba::async_read_until(_socket, 
                       ba::dynamic_buffer(_buffer), 
                       END_MARK,
                       boost::bind(&GrepServerSession::handle_read,
                                   shared_from_this(),
                                   ba::placeholders::error,
                                   ba::placeholders::bytes_transferred)

  );
}

// Step 1.
// Read in the grep command from the client and execute it after appending the target file
void GrepServerSession::handle_read(boost::system::error_code const& ec, size_t length [[maybe_unused]])
{
  if (ec) 
  {
    return;
  }

  // Remove the end mark \r\n\r\n
  auto pos = _buffer.find(END_MARK);
  if (pos != std::string::npos) {
    _buffer = _buffer.substr(0, pos);
  }

  // Append the target files for the grep program to scan
  auto grep_cmd = _buffer + " " + _log_dir_path;

  execute_cmd_regex(std::move(grep_cmd));
}

// Step 2.
// Execute grep command and write the result using async_write
// The result is transmitted one line by another
// void GrepServerSession::execute_cmd(std::string cmd) 
// {
//   std::cout << "Received " << cmd << std::endl;
//   bp::ipstream output_stream;
//   bp::child grep_process(cmd.c_str(), bp::std_out > output_stream);
  
//   // Read the output and send to client
//   std::string line;

//   while (std::getline(output_stream, line))
//   {
//     async_write(std::move(line) + "\n");
//   }

//   output_stream.pipe().close();
//   grep_process.wait();

//   // to indicate the end of data
//   async_write(END_MARK);
// }

// Boost process cannot handle regex...
void GrepServerSession::execute_cmd_regex(std::string cmd) 
{
  std::cout << "Received " << cmd << std::endl;
  FILE *pipe = popen(cmd.c_str(), "r");

  char buffer[512];
  while (fgets(buffer, sizeof(buffer), pipe) != nullptr) 
  {
    std::string line = buffer;
    _buffer += std::move(line);    
  }

  _buffer += END_MARK;
  
  async_write();
  pclose(pipe);
}

// Write the given data to the socket
// Invoke handle_write when done writing this data
void GrepServerSession::async_write()
{
  // Write 8192 bytes at a time
  const size_t MAX_CHARS = 8192;

  // Determine the size of the substring to extract
  auto len = std::min(_buffer.size(), MAX_CHARS);

  std::string data = _buffer.substr(0, len);
  _buffer.erase(0, len);

  ba::async_write(_socket, 
                  ba::buffer(std::move(data)), 
                  boost::bind(&GrepServerSession::handle_write,
                              shared_from_this(),
                              ba::placeholders::error,
                              ba::placeholders::bytes_transferred)
  
  );
}


// Step 3.
// does nothing after writing to buffer
void GrepServerSession::handle_write(boost::system::error_code const& ec, size_t length [[maybe_unused]])
{
  if (!_buffer.empty())
  {
    async_write();
  }

  return;
}

} // namespace server