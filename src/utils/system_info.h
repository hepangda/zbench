#ifndef BISHE_SYSTEM_INFO_H
#define BISHE_SYSTEM_INFO_H

struct SystemInfo {
  double process_avg_1min;
  double process_avg_5min;
  double process_avg_15min;

  long user_time;
  long kernel_time;
  long idle_time;
  long io_wait_time;
  long hard_irq_time;
  long soft_irq_time;

  static SystemInfo Now();
};

#endif // BISHE_SYSTEM_INFO_H
