#ifndef RESULT_HH
#define RESULT_HH

#include <string>

namespace client
{

class ErrorMsg
{
public:
  ErrorMsg(std::string host, std::string msg)
  : _host{std::move(host)}, _error_msg{std::move(msg)}
  {}

  // Getters
  const std::string& host() const { return _host; }
  const std::string& err_msg() const { return _error_msg; }

private:
  std::string _host;
  std::string _error_msg;
};

class Statistics
{
public:
  Statistics() {}

  Statistics(std::string host, std::string cmd, size_t lc)
  : _host{std::move(host)}, _grep_cmd{std::move(cmd)}, _line_count{lc}
  {}

  // Getters
  const std::string& host() const { return _host; }
  const std::string& cmd() const { return _grep_cmd; }
  size_t line_count() const { return _line_count; }

private:
  std::string _host;
  std::string _grep_cmd;
  size_t _line_count;
};

} // namespace client

#endif // RESULT_HH