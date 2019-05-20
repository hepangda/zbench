#ifndef BISHE_CLIENT_SCHEDULER_H
#define BISHE_CLIENT_SCHEDULER_H

#include <list>
#include <queue>
#include "client_report.h"
#include "../utils/http_request.h"
#include "https_session.h"
#include "http_session.h"
#include "../cli/optfwd.h"

class ClientScheduler {
 public:
  ClientScheduler(n::io_context &io_ctx, ni::tcp::endpoint endpoint, HttpRequest request)
      : io_ctx_(io_ctx), endpoint_(endpoint), request_(request) {
    request_buffer_ = std::make_shared<ReadBufferAdapter>(request_.Dump());
  }

  void Report(ClientReport report) {
    // printf("Report<%s>[Read: %lubytes, Write: %lubytes, IO: %lums, Connect: %lums, Dealing: %lums, Whole: %lums]\n",
    //    report.IsRight() ? "Valid" : "INVALID",
    //    report.GetReadBytes(), report.GetWriteBytes(), report.GetIoTime(),
    //    report.GetConnectTime(), report.GetDealingTime(), report.GetWholeTime());

    list_.emplace_back(report);
    if (!io_ctx_.stopped()) {
      SpawnOne();
    }
  }

  void Spawn(Protocol protocol, int numbers) {
    protocol_ = protocol;

    for (int i = 0; i < numbers; i++) {
      SpawnOne();
    }
  }

  void SpawnOne() {
    if (protocol_ == kPHttp) {
      std::make_shared<HttpSession>(*this)->Start();
    } else {
      std::make_shared<HttpsSession>(*this)->Start();
    }
  }

  n::io_context &GetIoContext() {
    return io_ctx_;
  }

  const ni::tcp::endpoint &GetEndpoint() const {
    return endpoint_;
  }

  std::shared_ptr<char> GetRequest() const {
    return request_buffer_->GetBuffer();
  }

  std::size_t GetRequestSize() const {
    return request_buffer_->GetSize();
  }

  std::list<ClientReport> GetResult() {
    return list_;
  }
 private:
  n::io_context &io_ctx_;
  ni::tcp::endpoint endpoint_;
  HttpRequest request_;
  std::shared_ptr<ReadBufferAdapter> request_buffer_;
  std::list<ClientReport> list_;
  Protocol protocol_{kPHttp};
};


#endif // BISHE_CLIENT_SCHEDULER_H
