#include "http_session.h"
#include "client_scheduler.h"
#include "client_scheduler.h"

HttpSession::HttpSession(ClientScheduler &cs) :
    cs_(cs), socket_(cs.GetIoContext()), read_buffer_(cs.GetRequest(), cs.GetRequestSize()) {}

void HttpSession::AsyncConnect(std::shared_ptr<HttpSession> self) {
  self->socket_.lowest_layer().async_connect(self->cs_.GetEndpoint(), [self](const std::error_code &ec) {
    if (!ec) {
      self->Bench(self, kToWrite);
    } else {
      self->report_.ReportError("Connect failed");
      self->Bench(self, kError);
    }
  });
}

void HttpSession::Bench(std::shared_ptr<HttpSession> self, BenchState state) {
  switch (state) {
    case kToWrite:
      self->report_.ReportConnectedTime();
      if (!self->report_.IsReportedWriteStartTime()) {
        self->report_.ReportWriteStartTime();
      }
      self->BenchWrite(self);
      break;
    case kToRead:
      self->report_.ReportWriteEndTime();
      if (!self->report_.IsReportedReadStartTime()) {
        self->report_.ReportReadStartTime();
      }
      self->BenchRead(self);
      break;
    case kDone:
      self->report_.ReportReadEndTime();
    case kError:
      self->Report(self);
      break;
  }
}

void HttpSession::BenchRead(std::shared_ptr<HttpSession> self) {
  constexpr int kErrorEof = 2;
  constexpr size_t kBufSize = 8192;
  self->socket_.async_read_some(self->write_buffer_.MutableBuffer(kBufSize),
      [self](const std::error_code &ec, size_t bytes) {
        if (!ec) {
          self->write_buffer_.Commit(bytes);
          self->write_buffer_.Consume(bytes);
          self->report_.ReportReadByte(bytes);
          self->Bench(self, kToRead);
        } else {
          if (ec.value() == kErrorEof) {
            self->Bench(self, kDone);
          } else {
//            self->report_.ReportError("I/O error when resposing");
            self->report_.ReportError(ec.message().c_str());
            self->Bench(self, kError);
          }
        }
      });
}

void HttpSession::BenchWrite(std::shared_ptr<HttpSession> self) {
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

void HttpSession::Report(std::shared_ptr<HttpSession> self) {
  self->report_.ReportEndTime();
  self->cs_.Report(self->report_);
}
