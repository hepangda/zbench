#ifndef BISHE_ID_GENERATOR_H
#define BISHE_ID_GENERATOR_H

#include "time_utils.h"

class IdGenerator {
 public:
  static IdGenerator &Instance() {
    static IdGenerator instance;
    return instance;
  }

  long Id() {
    auto now = TimeUtils::Now();
    if (now == last_id_) {
      // if now == last_id, make now++, and record the now as new "last_id_"
      last_id_ = ++now;
    } else {}
    return now;
  }

 private:
  IdGenerator() = default;
  IdGenerator(const IdGenerator &) = default;

  long last_id_;
};

#endif // BISHE_ID_GENERATOR_H
