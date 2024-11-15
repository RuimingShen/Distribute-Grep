#include "../../include/client/query_client.hh"

#include <iomanip>
#include <iostream>
#include <sstream>

#include <boost/make_shared.hpp>

// Modify this for different team
constexpr int TEAM_NUM = 19;
constexpr int VM_NUM   = 10;

namespace ba = boost::asio;

namespace client
{
// Given TEAM_NUM and VM_NUM, generate the hostnames for all VMs
// The hostnames are generated during the constructor phase
void QueryClient::generate_hostnames()
{
  _hostnames.reserve(10);

  for (int i = 1; i <= VM_NUM; i++)
  {
    std::stringstream ss;
    
    ss << "fa24-cs425-"                                 // make sure that numbers are always 2 digit
       << std::setw(2) << std::setfill('0') << TEAM_NUM // team number
       << std::setw(2) << std::setfill('0') << i        // vm number
       << ".cs.illinois.edu";
    
    _hostnames.push_back(ss.str());
  } 
}

// Entry function to start interacting with all hosts in _hostnames
// Invoke start_query
void QueryClient::start_interact(std::string grep_command)
{
  try 
  {
    ba::io_context io_context;
    client::QueryClient client(io_context, std::move(grep_command));
    client.start_query();
    
    io_context.run();
  } 
  catch(boost::system::system_error& se) 
  {
    std::cerr << "Error: " << se.what() << std::endl;
  }
}

// Spawn a new QueryClientSession for every host and let it does the communication with the server
void QueryClient::start_query()
{
  _start_time = std::chrono::high_resolution_clock::now();
  for (const auto& hostname : _hostnames)
  {
    auto new_session = boost::make_shared<QueryClientSession>(_io_context, hostname, _grep_cmd);
    _expected_response++;

    // set the callback fns
    new_session->set_error_callback(
      [this](ErrorMsg msg) 
      {
        on_error(std::move(msg));
      }
    );

    new_session->set_success_callback(
      [this](Statistics stat) 
      {
        on_success(std::move(stat));
      }
    );

    // Start the connection
    new_session->start_connect();

    // store the session into a map
    _vms_to_session_map[hostname] = new_session; 
  }
}

// QueryClientSession invoke on_error if they encountered any errors throughout the process
// ErrorMsg contains the host and the error msg
void QueryClient::on_error(ErrorMsg err_msg)
{
  // decrease the number of expected response
  _expected_response--;
  _error_map[err_msg.host()] = std::move(err_msg.err_msg());

  if (_expected_response == 0)
  {
    _result_printed = true;
    print_result();
  } 
}

// QueryClientSession invoke on_success if they successfully received the reply from the host
// Statistics contains the host, number of line matched etc
void QueryClient::on_success(Statistics stat)
{
  _expected_response--;
  _statistics_map[stat.host()] = std::move(stat);

  if (_expected_response == 0)
  {
    _result_printed = true;
    print_result();
  }
}

// Invoke when all QueryClientSession has notify that they are done
// First print out all the errors followed by the line count for each host
void QueryClient::print_result()
{
  auto end_time = std::chrono::high_resolution_clock::now();

  // Print Errors encountered:
  std::cout << "Errors:\n";
  for (const auto& vm_err_pair : _error_map)
  {
    std::cout << "[" << vm_err_pair.first << "] " 
              << vm_err_pair.second << ".\n";
  }
  std::cout << "\n";

  // Print Statistics:
  size_t total_line_count = 0;
  std::cout << "Statistics:\n";
  for (const auto& vm_stats_pair : _statistics_map)
  {
    const auto& stat = vm_stats_pair.second;
    total_line_count += stat.line_count();

    std::cout << "[" << vm_stats_pair.first << "] " 
              << "Number of matched lines: " << stat.line_count() << "\n";
  }

  // Calculate total time taken + total line matched
  std::chrono::duration<double, std::milli> ms = end_time - _start_time;
  std::cout << "Total number of matched lines: " << total_line_count 
            << ", time taken: " << ms.count() << "ms.\n";
}
} // namespace client
