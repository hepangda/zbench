#ifndef BISHE_HTTP_SESSION_H
#define BISHE_HTTP_SESSION_H

#include <memory>

class HttpSession: public std::enable_shared_from_this<HttpSession> {
 public:
  HttpSession(ClientScheduler &cs, ReadBufferAdapter request_buffer) :
  socket_(cs_.GetIoContext(), ssl_ctx_), cs_(cs), read_buffer_(cs_.GetRequest(), cs_.GetRequestSize()) {}

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

  static void AsyncConnect(std::shared_ptr<HttpsSession> self);
  static void Bench(std::shared_ptr<HttpsSession> self, BenchState state);
  static void BenchWrite(std::shared_ptr<HttpsSession> self);
  static void BenchRead(std::shared_ptr<HttpsSession> self);
  static void Report(std::shared_ptr<HttpsSession> self) {
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
