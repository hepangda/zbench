#ifndef BISHE_BENCHMARKER_H
#define BISHE_BENCHMARKER_H

#include "../utils/net.h"
#include "../utils/http_request.h"
#include "../client/client_scheduler.h"
#include "../cli/option.h"

class Benchmarker {
 public:
  Benchmarker(const HttpRequest &request, const ni::tcp::endpoint &endpoint, Option &option)
      : io_ctx_(1), cs_(io_ctx_, std::move(endpoint), std::move(request)), option_(option) {}

  std::list<ClientReport> Start() {
    cs_.Spawn(option_.protocol(), option_.clients());
    io_ctx_.run_for(std::chrono::seconds{option_.timeout()});

    return cs_.GetResult();
  }
 private:
  n::io_context io_ctx_;
  Option &option_;
  ClientScheduler cs_;
};

#endif // BISHE_BENCHMARKER_H
