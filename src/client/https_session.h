#ifndef BISHE_HTTPS_SESSION_H
#define BISHE_HTTPS_SESSION_H

#include "../utils/net.h"
#include "../utils/read_buffer_adapter.h"
#include "../utils/write_buffer_adapter.h"
#include "client_report.h"

class ClientScheduler;
class HttpsSession : public std::enable_shared_from_this<HttpsSession> {
 public:
  explicit HttpsSession(ClientScheduler &cs);

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
  static void AsyncHandshake(std::shared_ptr<HttpsSession> self);
  static void Bench(std::shared_ptr<HttpsSession> self, BenchState state);
  static void BenchWrite(std::shared_ptr<HttpsSession> self);
  static void BenchRead(std::shared_ptr<HttpsSession> self);
  static void Report(std::shared_ptr<HttpsSession> self);
 private:
  ns::context ssl_ctx_{ns::context::sslv23_client};
  ns::stream<ni::tcp::socket> socket_;

  ReadBufferAdapter read_buffer_;
  WriteBufferAdapter write_buffer_;

  ClientReport report_;
  ClientScheduler &cs_;
};

#endif // BISHE_HTTPS_SESSION_H