#ifndef BISHE_SYSCALL_ENTRY_H
#define BISHE_SYSCALL_ENTRY_H

#include <string>

struct SyscallEntry {
  double percent_time;
  double seconds;
  int usec_per_call;
  int calls;
  int errors;
  std::string syscall_name;
};

#endif // BISHE_SYSCALL_ENTRY_H
