#ifndef BISHE_CLI_PRINTER_H
#define BISHE_CLI_PRINTER_H

#include <list>
#include "../client/client_report.h"
#include "trace_result.h"
#include "syscall_result.h"
#include "../cli/option.h"

class CliPrinter {
 public:
  CliPrinter(std::list<ClientReport> &bench_result, std::list<TraceResult> &trace_result,
             SyscallResult &syscall_result, Option &option)
      : bench_result_(bench_result), trace_result_(trace_result), syscall_result_(syscall_result), option_(option) {}

  std::string Print();
 private:
  std::string PrintBench();
  std::string PrintTrace();
  std::string PrintSyscall();

  std::list<ClientReport> &bench_result_;
  std::list<TraceResult> &trace_result_;
  SyscallResult &syscall_result_;
  Option &option_;
};

#endif // BISHE_CLI_PRINTER_H
