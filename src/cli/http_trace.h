#ifndef BISHE_HTTP_TRACE_H
#define BISHE_HTTP_TRACE_H

struct HttpTrace {
  int server_pid_;
  bool trace_syscall_;
  bool trace_memory_;
  bool trace_cpu_;
};

#endif // BISHE_HTTP_TRACE_H
