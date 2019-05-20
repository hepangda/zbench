#include "syscall_tracer.h"
#include "../utils/linux_process.h"

SyscallResult SyscallTracer::Start(int pid, int timeout) {
  char pid_string[12];
  sprintf(pid_string, "%d", pid);

  LinuxProcess strace_process;
  strace_process.SetAsync();
  strace_process.ExecCommand("strace", "-f", "-c", "-p", pid_string);
  sleep(timeout);
  strace_process.Kill();

  return SyscallResult::FromString(strace_process.GetAllStderr());
}