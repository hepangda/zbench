#ifndef BISHE_CLIENT_SCHEDULER_H
#define BISHE_CLIENT_SCHEDULER_H

#include <list>
#include "client_report.h"
#include "../utils/http_request.h"
#include "https_session.h"

class ClientScheduler {
 public:
  ClientScheduler(n::io_context &io_ctx, ni::tcp::endpoint endpoint, HttpRequest request)
      : io_ctx_(io_ctx), endpoint_(std::move(endpoint)), request_(std::move(request)) {
     request_buffer_ = std::make_shared<ReadBufferAdapter>(request_.Dump());
  }

  void Report(ClientReport report) {
    printf("Report: %lu %lu\n", report.GetReadBytes(), report.GetWriteBytes());
    if (report.GetErrorMessage() != nullptr) {
      printf("Error: %s\n", report.GetErrorMessage());
    }
    list_.emplace_back(report);
    if (!io_ctx_.stopped()) {
      Spawn();
    }
  }

  void Spawn() {
    std::make_shared<HttpsSession>(*this)->Start();
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

  size_t GetRequestSize() const {
    return request_buffer_->GetSize();
  }
 private:
  n::io_context &io_ctx_;
  ni::tcp::endpoint endpoint_;
  HttpRequest request_;
  std::shared_ptr<ReadBufferAdapter> request_buffer_;
  std::list<ClientReport> list_;

};

#endif // BISHE_CLIENT_SCHEDULER_H
