#ifndef QUERY_CLIENT_HH
#define QUERY_CLIENT_HH

#include "../client/query_client_session.hh"

#include <atomic>
#include <chrono>
#include <fstream>
#include <functional>
#include <map>
#include <iostream>
#include <vector>

#include <boost/asio.hpp>

namespace client
{
class QueryClient 
{
public:
  explicit QueryClient(boost::asio::io_context& io_context, std::string grep_cmd)
  : _io_context{io_context}, _grep_cmd{std::move(grep_cmd)}
  {
    generate_hostnames();
  }

  ~QueryClient()
  {
    auto end_time = std::chrono::high_resolution_clock::now();

    if (!_result_printed)
    {
      std::cout << "Errors:\n";
      std::cout << "\n";

      std::cout << "Statistics:\n";
      long total_count = 0;
      for (const auto& host : _hostnames)
      {
        std::string filename = "./output/" + host + ".log";
        
        std::ifstream file(filename);
        long lineCount = 0;

        if (file.is_open())
        {
          std::string line;
          while (std::getline(file, line)) 
          {
            ++lineCount; // Increment line count for each line read
          }

          // Close the file
          file.close();
        }

        total_count += lineCount;
        std::cout << "[" << host << "] " 
                  << "Number of matched lines: " << lineCount << "\n";
      }

      std::chrono::duration<double, std::milli> ms = end_time - _start_time;
      std::cout << "Total number of matched lines: " << total_count 
                << ", time taken: " << ms.count() << "ms.\n";
    }
  }

  // Entry function to start interacting with the host
  static void start_interact(std::string grep_command);

  // A callback fn invoked by each session if encountered any error
  void on_error(ErrorMsg err_msg);
  // A callback fn invoked by each session if successfully received msg from server
  void on_success(Statistics stat);

private:
  // Generate all server hostnames
  void generate_hostnames();
  // Asynchronous functions that sends the query to all nodes asynchronously
  void start_query();

  // Print out statistcs + errors (if any)
  void print_result();

  boost::asio::io_context& _io_context;
  std::string _grep_cmd; // the command to be relayed to the servers
  std::vector<std::string> _hostnames; // list of server hostname
  std::map<std::string, QueryClientSessionPtr> _vms_to_session_map; // map from host to the session established
  std::chrono::time_point<std::chrono::high_resolution_clock> _start_time; // the time when start_connect was invoked

  // For result printing
  std::atomic<int> _expected_response = 0; // this might be r/w concurrently, hence atomic to ensure consistency
  std::map<std::string, Statistics> _statistics_map;
  std::map<std::string, std::string> _error_map; // error msg by a session
  bool _result_printed = false;
};
} // namespace client

#endif // QUERY_CLIENT_HH
