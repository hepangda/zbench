#include <string>
#include <fstream>
#include "system_info.h"

SystemInfo SystemInfo::Now() {
  SystemInfo info{};

  std::string buffer;
  {
    std::fstream file_stat{"/proc/loadavg", std::ios::in};

    file_stat >> buffer;
    info.process_avg_1min = std::stod(buffer);

    file_stat >> buffer;
    info.process_avg_5min = std::stod(buffer);

    file_stat >> buffer;
    info.process_avg_15min = std::stod(buffer);
  }

  {
    std::fstream file_stat{"/proc/stat", std::ios::in};
    file_stat >> buffer;

    file_stat >> buffer;
    info.user_time = std::stol(buffer);

    file_stat >> buffer;
    file_stat >> buffer;
    info.kernel_time = std::stol(buffer);

    file_stat >> buffer;
    info.idle_time = std::stol(buffer);

    file_stat >> buffer;
    info.io_wait_time = std::stol(buffer);

    file_stat >> buffer;
    info.hard_irq_time = std::stol(buffer);

    file_stat >> buffer;
    info.soft_irq_time = std::stol(buffer);
  }

  return info;
}