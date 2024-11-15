#include "../../include/client/query_client.hh"

#include <iostream>
#include <string>

int main() 
{
  std::cout << "Please enter the grep command:\n";
  
  std::string grep_command;
  std::getline(std::cin, grep_command);

  client::QueryClient::start_interact(std::move(grep_command));

  return 0;
}
