#ifndef BISHE_TIME_UTILS_H
#define BISHE_TIME_UTILS_H

#include <ctime>

class TimeUtils {
 public:
  static clock_t Now() {
    return clock();
  }
};

#endif // BISHE_TIME_UTILS_H
