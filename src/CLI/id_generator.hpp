/**
 * Created by Crow on 5/12/19.
 * Copyright (c) 2019 Crow All rights reserved.
 * @author Crow 
 */

#ifndef BISHE_CLI_ID_GENERATOR_HPP
#define BISHE_CLI_ID_GENERATOR_HPP

#include <chrono>

// Singleton Generator

class IdGenerator {
 public:
  static IdGenerator &Instance() {
    static IdGenerator instance;
    return instance;
  }

  long Id() {
    auto now = Now();
    if (now == last_id_) {               // if now == last_id, make now++, and record the now as new "last_id_"
      last_id_ = ++now;
    } else {}
    return now;
  }

 private:
  IdGenerator() = default;
  IdGenerator(const IdGenerator &) = default;
  long Now() {
    return std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now()
    );
  }

  long last_id_;
};

#endif //BISHE_CLI_ID_GENERATOR_HPP
