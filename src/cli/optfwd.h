#ifndef BISHE_OPTFWD_H
#define BISHE_OPTFWD_H

enum HttpMethod {
  kHMGet,
  kHMPost,
  kHMPut,
  kHMDelete,
};

enum Protocol {
  kPHttp,
  kPHttps,
};

enum HttpVersion {
  kHV1_0,
  kHV1_1,
};

struct HttpTrace {
  int server_pid_;
  bool trace_syscall_;
  bool trace_memory_;
  bool trace_cpu_;
};

#endif //BISHE_OPTFWD_H
