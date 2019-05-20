#ifndef BISHE_SYSCALL_TRACER_H
#define BISHE_SYSCALL_TRACER_H

#include "syscall_result.h"

class SyscallTracer {
 public:
  SyscallResult Start(int pid, int timeout);
};

#endif // BISHE_SYSCALL_TRACER_H
