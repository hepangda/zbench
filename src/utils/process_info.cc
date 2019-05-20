#include "process_info.h"
#include <iostream>
#include <fstream>

ProcessInfo ProcessInfo::Get(int pid) {
  ProcessInfo info{};

  char stat_file[50];
  sprintf(stat_file, "/proc/%d/stat", pid);
  std::fstream file_stat{stat_file, std::ios::in};

  std::string buffer;

  file_stat >> buffer;
  file_stat.ignore(')');

  file_stat >> buffer;
  info.state = buffer[0];

  file_stat >> buffer;
  file_stat >> buffer;
  file_stat >> buffer;
  file_stat >> buffer;
  file_stat >> buffer;
  file_stat >> buffer;
  file_stat >> buffer;
  file_stat >> buffer;
  file_stat >> buffer;
  file_stat >> buffer;

  file_stat >> buffer; // jiffies
  info.user_time = std::stol(buffer);
  file_stat >> buffer;
  info.kernel_time = std::stol(buffer);

  file_stat >> buffer;
  file_stat >> buffer;
  file_stat >> buffer;
  file_stat >> buffer;
  file_stat >> buffer;
  file_stat >> buffer;
  file_stat >> buffer;

  file_stat >> buffer;
  info.vsize = std::stol(buffer);
  file_stat >> buffer;
  info.rss = std::stol(buffer);

  return info;
}