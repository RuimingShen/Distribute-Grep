#ifndef QUERY_CLIENT_SESSION_HH
#define QUERY_CLIENT_SESSION_HH

#include "../client/result.hh"

#include <vector>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace client
{
const std::string END_MARK = "\r\n\r\n";
const std::string OUTPUT_DIR = "./output";
constexpr int BUFFER_SIZE = 8192;

// The result of querying the ip addr:port of a given host
using ResolveResult = boost::asio::ip::tcp::resolver::results_type;
// The endpoint of the connection
using Endpoint = boost::asio::ip::tcp::endpoint;

// Callback function types
using ErrorCallback = std::function<void(ErrorMsg)>;
using SuccessCallback = std::function<void(Statistics)>;

class QueryClientSession : public boost::enable_shared_from_this<QueryClientSession>
{
public:
  // If no output file specified, default path is ./output/{host}.log
  explicit QueryClientSession(boost::asio::io_context& io_context, std::string host, std::string grep_cmd)
  : _socket{io_context}, _resolver{io_context}, _buffer(BUFFER_SIZE),
    _host{std::move(host)} , _grep_cmd{std::move(grep_cmd)}, 
    _output_dir{OUTPUT_DIR}
  {
    // Remove the output file if alread exist
    remove_file(get_output_filepath());
  }

  // Output exact matches to a specify file
  explicit QueryClientSession(boost::asio::io_context& io_context, std::string host, std::string grep_cmd, std::string output_dir)
  : _socket{io_context}, _resolver{io_context}, _buffer(BUFFER_SIZE),
    _host{std::move(host)} , _grep_cmd{std::move(grep_cmd)}, 
    _output_dir{std::move(output_dir)}
  {
    // Remove the output file if alread exist
    remove_file(get_output_filepath());
  }

  // setters
  void set_error_callback(ErrorCallback ecb) { _err_callback = ecb; }
  void set_success_callback(SuccessCallback scb) { _success_callback = scb; }

  /* entry function, does the following steps (all asynchronously):
   * 1. Resolve the host to get the ip address
   * 2. Connect to the host
   * 3. Forward the grep command
   * 4. Read the message from the server
   * 5. Notify QueryClient and write the result into a file
   */
  void start_connect();

private:
  // Step 2: After getting the ip address, tries to connect to it
  void handle_resolve(boost::system::error_code const& ec, const ResolveResult& results);
  // Step 3: After connecting to the server, send the grep command
  void handle_connect(boost::system::error_code const& ec, const Endpoint& results);
  // Step4. After sending the command, read from the connection
  void handle_write(boost::system::error_code const& ec, size_t length);
  // Step5. After sending the command, read from the connection
  void async_read_chunk();
  void handle_read_chunk(boost::system::error_code const& ec, size_t length);

private:
  // Utility function
  // Get the output file path
  const std::string get_output_filepath() { return _output_dir + "/" + _host + ".log"; }
  // Append lines to the output file
  void append_to_output_file(std::string lines);
  // Split a string by a delimiter
  static std::vector<std::string> split(std::string s, const std::string& delimiter); 
  // count the number of lines in a file
  static long countLines(const std::string& filename);
  // Remove file if exist
  static void remove_file(const std::string filepath);

private:
  // network related members
  boost::asio::ip::tcp::socket _socket;
  boost::asio::ip::tcp::resolver _resolver; // resolver to get the ip addr from the hostname
  std::vector<char> _buffer; // the buffer used to store incoming message
  
  // session related members
  std::string _host;
  std::string _grep_cmd;
  std::string _output_dir; // the dir to output the exact matches

  // callbacks to notify QueryClient
  ErrorCallback _err_callback;
  SuccessCallback _success_callback;
};

using QueryClientSessionPtr = boost::shared_ptr<QueryClientSession>;
} // namespace client

#endif // QUERY_CLIENT_SESSION_HH
