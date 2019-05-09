#include "process_info.h"
#include <iostream>
#include <fstream>

#include <sys/user.h>

constexpr unsigned long ByteToKilobyte(unsigned long x) {
  return (x + 512) >> 10u;
}

constexpr unsigned long PageToKilobyte(unsigned long x) {
  return x << (PAGE_SHIFT - 10);
}

void GetSystemInfo() {
  std::string buffer;
  {
    std::fstream file_stat{"/proc/loadavg", std::ios::in};
    file_stat >> buffer;  // 1min 平均进程数
    file_stat >> buffer;  // 5min
    file_stat >> buffer;  // 15min

    file_stat >> buffer; //skip running/tasks
    file_stat >> buffer; //last_pid
  }

  {
    std::fstream file_stat{"/proc/stat", std::ios::in};
    file_stat >> buffer; // skip string{cpu}
    file_stat >> buffer; //user 用户态时间
    file_stat >> buffer; //nice nice为负进程占用的时间
    file_stat >> buffer; //system 内核态时间
    file_stat >> buffer; //idle 除过io等待时间以外其他等待时间
    file_stat >> buffer; //iowait io等待时间
    file_stat >> buffer; //irq 硬中断时间
    file_stat >> buffer; //softirq 软中断时间
    file_stat >> buffer; //stealstolen
    file_stat >> buffer; //guest
  }

  {
    std::fstream file_stat{"/proc/meminfo", std::ios::in};
  }
}

void GetProcInfoOfPid(pid_t pid) {
  std::fstream file_stat{"/proc/${pid}/stat", std::ios::in};

  std::string buffer;

  file_stat >> buffer;
  file_stat.ignore(')');

  file_stat >> buffer; // >>state 进程执行状态 %c

  file_stat >> buffer; // ppid
  file_stat >> buffer; // pgrp
  file_stat >> buffer; // session
  file_stat >> buffer; // tty
  file_stat >> buffer; // tty pgrp
  file_stat >> buffer; // flags
  file_stat >> buffer; // min flt
  file_stat >> buffer; // cmin flt
  file_stat >> buffer; // maj flt
  file_stat >> buffer; // cmaj flt

  file_stat >> buffer; // >>utime 任务在用户态运行的时间 jiffies %lu
  file_stat >> buffer; // >>stime 任务在内核态执行的时间 jiffies %lu

  file_stat >> buffer; // cutime
  file_stat >> buffer; // cstime

  file_stat >> buffer; // >>priority 动态优先级 %ld
  file_stat >> buffer; // >>nice 静态优先级 %ld

  file_stat >> buffer; // timeout
  file_stat >> buffer; // it_real_val
  file_stat >> buffer; // start_time

  file_stat >> buffer; // >>vsize 虚拟地址空间大小 %lu
  file_stat >> buffer; // >>rss 物理地址大小 %ld
}