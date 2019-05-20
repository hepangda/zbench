#ifndef BISHE_TRACE_RESULT_H
#define BISHE_TRACE_RESULT_H

struct TraceResult {
  int seq_number;
  double cpu_percent;
  double mem_percent;
  int mem_size;
};

#endif // BISHE_TRACE_RESULT_H

