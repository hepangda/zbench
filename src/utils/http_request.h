#ifndef BISHE_HTTP_REQUEST_H
#define BISHE_HTTP_REQUEST_H

#include <unordered_map>
#include <string>
#include <memory>
#include "read_buffer_adapter.h"

class HttpRequest {
 public:
  HttpRequest() = default;
  HttpRequest(std::string method, std::string uri, std::string version) :
      method_(std::move(method)), request_uri_(std::move(uri)), version_(std::move(version)) {}

  HttpRequest(const HttpRequest &rhs) = default;

  HttpRequest(HttpRequest &&rhs) noexcept:
      method_(std::move(rhs.method_)), request_uri_(std::move(rhs.request_uri_)), version_(std::move(rhs.version_)),
      headers_(std::move(rhs.headers_)), body_(std::move(rhs.body_)), body_length_(rhs.body_length_) {}

  ReadBufferAdapter Dump();
  size_t CalcReqeustSize();

  void SetMethod(std::string method) {
    method_ = std::move(method);
  }

  void SetVersion(std::string version) {
    version_ = std::move(version);
  }

  void SetRequestUri(std::string uri) {
    request_uri_ = std::move(uri);
  }

  void SetHeaderItem(const std::string &key, std::string value) {
    headers_[key] = std::move(value);
  }

  void PutBody(std::shared_ptr<char[]> body, size_t size) {
    body_ = std::move(body);
    body_length_ = size;
  }

  void ResetBody() {
    body_.reset();
    body_length_ = 0;
  }

  std::string GetMethod() const {
    return method_;
  }

  std::string GetVersion() const {
    return version_;
  }

  std::string GetRequestUri() const {
    return request_uri_;
  }

  std::string GetHeaderItem(const std::string &key) const {
    auto iter = headers_.find(key);
    if (iter != headers_.cend()) {
      return iter->second;
    }
    return {};
  }

  decltype(auto) GetHeadersBegin() const {
    return headers_.cbegin();
  }

  decltype(auto) GetHeadersEnd() const {
    return headers_.cend();
  }

  std::shared_ptr<char[]> GetBody() const {
    return body_;
  }

  size_t GetBodyLength() const {
    return body_length_;
  }
 private:
  std::string method_;
  std::string version_;
  std::string request_uri_;
  std::unordered_map<std::string, std::string> headers_;
  std::shared_ptr<char[]> body_{nullptr};
  size_t body_length_{0};
};

#endif // BISHE_HTTP_REQUEST_H
