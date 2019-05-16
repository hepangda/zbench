#ifndef BISHE_BENCHMARKER_H
#define BISHE_BENCHMARKER_H

#include "../utils/net.h"
#include "../utils/http_request.h"
#include "../client/client_scheduler.h"
#include "../cli/option.h"

class Benchmarker {
 public:
  Benchmarker(HttpRequest request, ni::tcp::endpoint endpoint, Option option)
      : io_ctx_(1), cs_(io_ctx_, std::move(endpoint), std::move(request)), option_(std::move(option)) {}

  void Start() {
    cs_.Spawn();
    io_ctx_.run_for(std::chrono::duration<int>{option_.timeout()});
    cs_.PrintResult(option_.timeout());
  }
 private:
  n::io_context io_ctx_;
  Option option_;
  ClientScheduler cs_;
};

#endif // BISHE_BENCHMARKER_H
