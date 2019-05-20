#ifndef BISHE_TIME_UTILS_H
#define BISHE_TIME_UTILS_H

#include <ctime>

class TimeUtils {
 public:
  static clock_t Now() {
    if (CLOCKS_PER_SEC == 1000) {
      return clock();
    } else {
      constexpr int level = CLOCKS_PER_SEC / 1000;
      return clock() / level;
    }
  }
};

#endif // BISHE_TIME_UTILS_H
