#include "https_session.h"
#include "client_scheduler.h"

using std::size_t;

HttpsSession::HttpsSession(ClientScheduler &cs) :
    socket_(cs.GetIoContext(), ssl_ctx_), cs_(cs), read_buffer_(cs.GetRequest(), cs.GetRequestSize()) {}

void HttpsSession::AsyncConnect(std::shared_ptr<HttpsSession> self) {
  self->socket_.lowest_layer().async_connect(self->cs_.GetEndpoint(), [self](const std::error_code &ec) {
    if (!ec) {
      self->AsyncHandshake(self);
    } else {
      self->report_.ReportError("Connect failed");
      self->Bench(self, kError);
    }
  });
}

void HttpsSession::AsyncHandshake(std::shared_ptr<HttpsSession> self) {
  self->socket_.async_handshake(ns::stream_base::handshake_type::client, [self](const std::error_code &ec) {
    if (!ec) {
      self->Bench(self, kToWrite);
    } else {
      self->report_.ReportError("SSL Handshake failed");
      self->Bench(self, kError);
    }
  });
}

void HttpsSession::Bench(std::shared_ptr<HttpsSession> self, BenchState state) {
  switch (state) {
    case kToWrite:
      self->report_.ReportConnectedTime();
      self->report_.ReportWriteStartTime();
      self->BenchWrite(self);
      break;
    case kToRead:
      self->report_.ReportWriteEndTime();
      self->report_.ReportReadStartTime();
      self->BenchRead(self);
      break;
    case kDone:
      self->report_.ReportReadEndTime();
    case kError:
      self->Report(self);
      break;
  }
}

void HttpsSession::BenchRead(std::shared_ptr<HttpsSession> self) {
  constexpr int kErrorEof = 2;
  constexpr size_t kBufSize = 8192;
  self->socket_.async_read_some(self->write_buffer_.MutableBuffer(kBufSize),
      [self](const std::error_code &ec, size_t bytes) {
        if (!ec) {
          self->write_buffer_.Commit(bytes);
//          printf("\n Recv: \n %s \n\n", self->write_buffer_.data<char>());
          self->write_buffer_.Consume(bytes);
          self->report_.ReportReadByte(bytes);
          self->Bench(self, kToRead);
        } else {
          if (ec.value() == kErrorEof) {
            self->Bench(self, kDone);
          } else {
            self->report_.ReportError("I/O error when resposing");
            self->Bench(self, kError);
          }
        }
      });
}

void HttpsSession::BenchWrite(std::shared_ptr<HttpsSession> self) {
  self->socket_.async_write_some(self->read_buffer_.ConstBuffer(), [self](const std::error_code &ec, size_t bytes) {
    if (!ec) {
      self->read_buffer_.IncreaseCursor(bytes);
      self->report_.ReportWriteByte(bytes);

      if (self->read_buffer_.IsFinish()) {
        self->read_buffer_.ResetCursor();
        self->Bench(self, kToRead);
      } else {
        self->Bench(self, kToWrite);
      }
    } else {
      self->report_.ReportError("I/O error when requesting");
      self->Bench(self, kError);
    }
  });
}

void HttpsSession::Report(std::shared_ptr<HttpsSession> self) {
  self->report_.ReportEndTime();
  self->cs_.Report(self->report_);
}