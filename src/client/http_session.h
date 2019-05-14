#ifndef BISHE_HTTP_SESSION_H
#define BISHE_HTTP_SESSION_H

#include <memory>
#include "client_scheduler.h"
#include "../utils/read_buffer_adapter.h"

class ClientScheduler;

class HttpSession : public std::enable_shared_from_this<HttpSession> {
 public:
  HttpSession(ClientScheduler &cs, ReadBufferAdapter request_buffer) :
      cs_(cs), socket_(cs.GetIoContext()), read_buffer_(cs.GetRequest(), cs.GetRequestSize()) {}

  void Start() {
    auto self = shared_from_this();
    AsyncConnect(self);
  }
 protected:
  enum BenchState {
    kToWrite,
    kToRead,
    kDone,
    kError,
  };

  static void AsyncConnect(std::shared_ptr<HttpSession> self);
  static void Bench(std::shared_ptr<HttpSession> self, BenchState state);
  static void BenchWrite(std::shared_ptr<HttpSession> self);
  static void BenchRead(std::shared_ptr<HttpSession> self);
  static void Report(std::shared_ptr<HttpSession> self) {
    self->cs_.Report(self->report_);
  }
 private:
  ni::tcp::socket socket_;

  ReadBufferAdapter read_buffer_;
  WriteBufferAdapter write_buffer_;

  ClientReport report_;
  ClientScheduler &cs_;
};

#endif // BISHE_HTTP_SESSION_H
