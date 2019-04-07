#ifndef BISHE_BENCHMARKER_H
#define BISHE_BENCHMARKER_H

#include "../utils/net.h"
#include "../utils/http_request.h"
#include "../client/client_scheduler.h"

class Benchmarker {
 public:
  Benchmarker(HttpRequest request, ni::tcp::endpoint endpoint)
      : io_ctx_(1), cs_(io_ctx_, std::move(endpoint), std::move(request)) {}

  void Start() {
    cs_.Spawn();
    io_ctx_.run();
  }
 private:
  n::io_context io_ctx_;
  ClientScheduler cs_;
};

#endif // BISHE_BENCHMARKER_H
