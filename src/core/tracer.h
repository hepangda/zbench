#ifndef BISHE_TRACER_H
#define BISHE_TRACER_H

#include <list>

#include "trace_result.h"

class Tracer {
 public:
  Tracer(int pid, int id, std::list<TraceResult> &list) : id_(id), pid_(pid), list_(list) {}

  void DoTrace();
 private:
  int id_;
  int pid_;
  std::list<TraceResult> &list_;
};

#endif // BISHE_TRACER_H
