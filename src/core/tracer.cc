#include <sstream>
#include "tracer.h"
#include "../utils/linux_process.h"

namespace detail {

TraceResult ExtractResult(const std::string &str, int pid) {
  std::stringstream ss;
  std::string spid, pcpu, pmem, size;
  ss << str;

  std::getline(ss, spid);
  while (ss >> spid >> pcpu >> pmem >> size) {
    int this_pid = std::stoi(spid);
    if (this_pid == pid) {
      TraceResult trace_result{};
      trace_result.cpu_percent = std::stod(pcpu);
      trace_result.mem_percent = std::stod(pmem);
      trace_result.mem_size = std::stoi(size);
      return trace_result;
    }
  }

  return TraceResult{-1};
}

}

void Tracer::DoTrace() {
  LinuxProcess ps_process;
  ps_process.SetAsync();
  ps_process.ExecCommand("ps", "-eo", "pid,pcpu,pmem,size");

  auto result = detail::ExtractResult(ps_process.GetAllStdout(), pid_);
  result.seq_number = id_;
  list_.emplace_back(result);
  id_++;
}