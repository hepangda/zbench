#ifndef BISHE_ID_GENERATOR_H
#define BISHE_ID_GENERATOR_H

#include <chrono>

class IdGenerator {
 public:
  static IdGenerator &Instance() {
    static IdGenerator instance;
    return instance;
  }

  long Id() {
    auto now = Now();
    if (now == last_id_) {
      // if now == last_id, make now++, and record the now as new "last_id_"
      last_id_ = ++now;
    } else {}
    return now;
  }

 private:
  IdGenerator() = default;
  IdGenerator(const IdGenerator &) = default;

  static long Now() {
    return std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now()
    );
  }

  long last_id_;
};

#endif // BISHE_ID_GENERATOR_H
