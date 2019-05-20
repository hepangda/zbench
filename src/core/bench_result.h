#ifndef BISHE_BENCH_RESULT_H
#define BISHE_BENCH_RESULT_H

#include <cstdint>

struct BenchResult {
  std::int64_t total_read_bytes;
  std::int64_t total_write_bytes;
  std::int64_t errors_request;

  double bytes_per_second;
  double request_per_second;

  long min_connect_time;
  long middle_connect_time;
  long max_connect_time;

  long min_dealing_time;
  long middle_dealing_time;
  long max_dealing_time;

  long min_io_time;
  long middle_io_time;
  long max_io_time;

  long min_total_time;
  long middle_total_time;
  long max_total_time;

  long percent_50_time;
  long percent_66_time;
  long percent_75_time;
  long percent_80_time;
  long percent_90_time;
  long percent_95_time;
  long percent_98_time;
  long percent_99_time;
  long percent_100_time;

};

#endif // BISHE_BENCH_RESULT_H
