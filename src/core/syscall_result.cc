#include "syscall_result.h"

#include <sstream>

SyscallResult SyscallResult::FromString(const std::string &out) {
  SyscallResult result;

  std::stringstream ss;
  std::string buffer;
  ss << out;

  while (ss.get() != '-') {}
  while (ss.get() != '\n') {}

  while (ss >> buffer, isdigit(buffer.front())) {
    SyscallEntry i;

    i.percent_time = std::stod(buffer);

    ss >> buffer;
    i.seconds = std::stod(buffer);

    ss >> buffer;
    i.usec_per_call = std::stoi(buffer);

    ss >> buffer;
    i.calls = std::stoi(buffer);

    ss >> buffer;
    if (isdigit(buffer.front())) {
      i.errors = std::stoi(buffer);
      ss >> buffer;
    } else {
      i.errors = 0;
    }
    i.syscall_name = buffer;

    result.entries_.emplace_back(i);
  }

  return result;
}
