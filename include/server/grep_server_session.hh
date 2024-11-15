#ifndef GERP_SERVER_SESSION_HH
#define GERP_SERVER_SESSION_HH

#include <mutex>
#include <string>
#include <queue>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/make_shared.hpp>

namespace server
{

const std::string END_MARK = "\r\n\r\n";

class GrepServerSession : public boost::enable_shared_from_this<GrepServerSession>
{
public:
  explicit GrepServerSession(boost::asio::io_context& io_context, std::string log_dir)
  : _socket{io_context}, _log_dir_path{std::move(log_dir)}
  {}

  // Start by reading the cmd from the client, does the following:
  // 1. Read in the grep command
  // 2. Execute the grep command
  // 3. Forward the result to client one line by one line
  void start_interact();

  // Getters
  boost::asio::ip::tcp::socket& get_socket() { return _socket; }

private:
  void handle_read(boost::system::error_code const& ec, size_t length);
  // Step 2. Execute command and forward it to client
  void execute_cmd(std::string cmd);
  void execute_cmd_regex(std::string cmd);
  // Write data to socket
  void async_write();
  void handle_write(boost::system::error_code const& ec, size_t length);

private:
  boost::asio::ip::tcp::socket _socket;
  std::string _log_dir_path;
  std::string _buffer; // used to store the read in data
  std::mutex _mutex;
};

using GrepServerSessionPtr = boost::shared_ptr<GrepServerSession>;
} // namespace server

#endif // GERP_SERVER_SESSION_HH
