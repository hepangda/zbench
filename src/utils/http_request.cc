#include "http_request.h"
#include "read_buffer_adapter.h"

ReadBufferAdapter HttpRequest::Dump() {
  ReadBufferAdapter ret{CalcReqeustSize()};

  ret.IncreaseCursor(sprintf(ret.CursorPointer(), "%s %s %s\r\n", method_.data(), request_uri_.data(), version_.data()));
  for (const auto &i: headers_) {
    ret.IncreaseCursor(sprintf(ret.CursorPointer(), "%s: %s\r\n", i.first.data(), i.second.data()));
  }

  ret.IncreaseCursor(sprintf(ret.CursorPointer(), "\r\n"));
  if (body_) {
    memcpy(ret.CursorPointer(), body_.get(), body_length_);
    ret.IncreaseCursor(body_length_);
  }

  ret.ResetCursor();
  return std::move(ret);
}

size_t HttpRequest::CalcReqeustSize() {
  size_t ret{0};
  // GET /uri HTTP/1.1\r\n
  ret += method_.size() + 1 + request_uri_.size() + 1 + version_.size() + 2;

  // Key: Value\r\n
  for (const auto &i: headers_) {
    ret += i.first.size() + 2 + i.second.size() + 2;
  }

  ret += 2; // CR LF
  ret += body_length_;
  return ret;
}