#ifndef BISHE_SYSCALL_RESULT_H
#define BISHE_SYSCALL_RESULT_H

#include <vector>
#include "syscall_entry.h"

class SyscallResult {
 public:
  static SyscallResult FromString(const std::string &out);

  bool Empty() const {
    return entries_.empty();
  }

  const std::vector<SyscallEntry> &Entry() const {
    return entries_;
  }

 private:
  std::vector<SyscallEntry> entries_;
};

#endif // BISHE_SYSCALL_RESULT_H
