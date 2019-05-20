#include <algorithm>
#include <cstring>
#include "cli_printer.h"

#define sprint(x, ...) sprintf(buffer, x, ##__VA_ARGS__); result += buffer; memset(buffer, 0, sizeof(buffer));

std::string CliPrinter::Print() {
  return PrintBench() + PrintTrace() + PrintSyscall();
}

std::string CliPrinter::PrintBench() {
  std::string result;
  char buffer[4096];

  std::int64_t total_read_bytes = 0;
  std::int64_t total_write_bytes = 0;
  std::int64_t errors_request = 0;
  std::vector<long> connect_time_q;
  std::vector<long> deal_time_q;
  std::vector<long> io_time_q;
  std::vector<long> total_time_q;

  for (const ClientReport &i : bench_result_) {
    total_read_bytes += i.GetReadBytes();
    total_write_bytes += i.GetWriteBytes();
    if (i.IsRight()) {
      connect_time_q.emplace_back(i.GetConnectTime());
      deal_time_q.emplace_back(i.GetDealingTime());
      io_time_q.emplace_back(i.GetIoTime());
      total_time_q.emplace_back(i.GetWholeTime());
    } else {
      errors_request++;
    }
  }

  double bytes_per_second = double(total_read_bytes + total_write_bytes) / option_.timeout();
  double request_per_second = double(bench_result_.size()) / option_.timeout();

  sprint("Total %zu request, %ld errors.\n", bench_result_.size(), errors_request);
  sprint("%.3lf Byte(s) per second, total %ld bytes.\n", bytes_per_second, total_read_bytes + total_write_bytes);
  sprint("%.3lf Request(s) per second, total %zu requests.\n", request_per_second, bench_result_.size());
  sprint("\n");

  std::sort(connect_time_q.begin(), connect_time_q.end());
  std::sort(deal_time_q.begin(), deal_time_q.end());
  std::sort(io_time_q.begin(), io_time_q.end());
  std::sort(total_time_q.begin(), total_time_q.end());

  int middle = io_time_q.size() / 2;
  sprint("%-17s %-7s %-7s %-7s\n", "", "Min", "Middle", "Max");
  sprint("%-17s %-7ld %-7ld %-7ld\n", "Connect Time(ms)", connect_time_q.front(),
      connect_time_q[middle], connect_time_q.back());
  sprint("%-17s %-7ld %-7ld %-7ld\n", "Dealing Time(ms)", deal_time_q.front(),
      deal_time_q[middle], deal_time_q.back());
  sprint("%-17s %-7ld %-7ld %-7ld\n", "I/O Time(ms)", io_time_q.front(), io_time_q[middle], io_time_q.back());
  sprint("%-17s %-7ld %-7ld %-7ld\n", "Total Time(ms)", total_time_q.front(),
      total_time_q[middle], total_time_q.back());
  sprint("\n");

  int percent_50 = (total_time_q.size() - 1) * 0.5;
  int percent_66 = (total_time_q.size() - 1) * 0.66;
  int percent_75 = (total_time_q.size() - 1) * 0.75;
  int percent_80 = (total_time_q.size() - 1) * 0.8;
  int percent_90 = (total_time_q.size() - 1) * 0.9;
  int percent_95 = (total_time_q.size() - 1) * 0.95;
  int percent_98 = (total_time_q.size() - 1) * 0.98;
  int percent_99 = (total_time_q.size() - 1) * 0.99;
  int percent_100 = (total_time_q.size() - 1) * 1;

  sprint("%-11s Time(ms)\n", "Percentage");
  sprint("%-11s %ld\n", "50%", total_time_q[percent_50]);
  sprint("%-11s %ld\n", "66%", total_time_q[percent_66]);
  sprint("%-11s %ld\n", "75%", total_time_q[percent_75]);
  sprint("%-11s %ld\n", "80%", total_time_q[percent_80]);
  sprint("%-11s %ld\n", "90%", total_time_q[percent_90]);
  sprint("%-11s %ld\n", "95%", total_time_q[percent_95]);
  sprint("%-11s %ld\n", "98%", total_time_q[percent_98]);
  sprint("%-11s %ld\n", "99%", total_time_q[percent_99]);
  sprint("%-11s %ld (slowest request)\n", "100%", total_time_q[percent_100]);
  sprint("\n");

  return result;
}

std::string CliPrinter::PrintTrace() {
  if (trace_result_.empty()) {
    return "";
  }
  std::string result;
  char buffer[4096];

  std::vector<TraceResult> traces;
  for (const auto &i : trace_result_) {
    if (i.seq_number != -1) {
      traces.push_back(i);
    }
  }

  std::sort(traces.begin(), traces.end(), [](auto i, auto j) { return i.seq_number < j.seq_number; });
  sprint("Server Program Status: \n");
  sprint("%-8s %-8s %-8s %-8s\n", "Second", "CPU(%)", "MEM(%)", "SIZE(KiB)");
  for (const auto &i : traces) {
    sprint("%-8d %-8.2lf %-8.2lf %-8d\n", i.seq_number, i.cpu_percent, i.mem_percent, i.mem_size);
  }
  sprint("\n");

  return result;
}

std::string CliPrinter::PrintSyscall() {
  if (syscall_result_.Empty()) {
    return "";
  }
  std::string result;
  char buffer[4096];

  sprint("Server Program Syscall: \n");
  sprint("%-15s %-10s %-14s %-7s %-15s %-8s\n",
      "Syscall Name", "Percent(%)", "Time Used(s)", "Calls", "USec Per Call", "Errors");
  for (const auto &i : syscall_result_.Entry()) {
    sprint("%-15s %-10.2lf %-14lf %-7d %-15d %-8d\n", i.syscall_name.c_str(), i.percent_time, i.seconds,
        i.calls, i.usec_per_call, i.errors);
  }
  sprint("\n");

  return result;
}