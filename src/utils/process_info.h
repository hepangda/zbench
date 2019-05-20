#ifndef BISHE_PROCESS_INFO_H
#define BISHE_PROCESS_INFO_H

struct ProcessInfo {
  char state;
  long user_time;
  long kernel_time;
  long vsize;
  long rss;

  static ProcessInfo Get(int pid);
};

#endif // BISHE_PROCESS_INFO_H
