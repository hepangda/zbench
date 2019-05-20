#ifndef BISHE_SYSCALL_RESULT_H
#define BISHE_SYSCALL_RESULT_H

#include <vector>
#include "syscall_entry.h"

class SyscallResult {
 public:
  static SyscallResult FromString(const std::string &out);
  void Print() {
    for (auto i: entries_) {
      printf("%lf %lf %d %d %d %s\n", i.percent_time, i.seconds, i.usec_per_call, i.calls, i.errors, i.syscall_name.c_str());
    }
  }
 private:
  std::vector<SyscallEntry> entries_;
};

#endif // BISHE_SYSCALL_RESULT_H
