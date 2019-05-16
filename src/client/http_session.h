#ifndef BISHE_HTTP_SESSION_H
#define BISHE_HTTP_SESSION_H

#include <memory>
#include "../utils/read_buffer_adapter.h"
#include "../utils/write_buffer_adapter.h"
#include "client_report.h"

class ClientScheduler;

class HttpSession : public std::enable_shared_from_this<HttpSession> {
 public:
  HttpSession(ClientScheduler &cs);

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
  static void Report(std::shared_ptr<HttpSession> self);
 private:
  ni::tcp::socket socket_;

  ReadBufferAdapter read_buffer_;
  WriteBufferAdapter write_buffer_;

  ClientReport report_;
  ClientScheduler &cs_;
};

#endif // BISHE_HTTP_SESSION_H
