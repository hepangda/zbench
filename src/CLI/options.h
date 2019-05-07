/**
 * Created by Crow on 4/30/19.
 * Copyright (c) 2019 Crow All rights reserved.
 * @author Crow 
 */

#ifndef BISHE_CLI_OPTIONS_H
#define BISHE_CLI_OPTIONS_H

#include <unistd.h>

#include <cstring>
#include <map>
#include <string>
#include <memory>

enum HttpMethod {
  kHMGet,
  kHMPost,
  kHMPut,
  kHMDelete,
};

static const std::map<std::string, enum HttpMethod> method_map {
    {"get", HttpMethod::kHMGet},
    {"post", HttpMethod::kHMPost},
    {"put", HttpMethod::kHMPut},
    {"delete", HttpMethod::kHMDelete},
};

enum Protocol {
  kPHttp,
  kPHttps,
};

enum HttpVersion {
  kHV1_0,
  kHV1_1,
};

struct HttpTrace {
  int server_pid_;
  bool trace_syscall_;
  bool trace_memory_;
  bool trace_cpu_;
};

class Option {
 public:
  Option()
      : protocol_(Protocol::kPHttp),
        method_(HttpMethod::kHMGet),
        version_(HttpVersion::kHV1_0),
        http_trace_({-1, false, false, false}),
        timeout_(0), threads_(0), clients_(0), trace_(false) {}

  Protocol protocol()   const     { return protocol_; }
  HttpMethod method()   const     { return method_; }
  HttpVersion version() const     { return version_; }
  int timeout()         const     { return timeout_; }
  int threads()         const     { return threads_; }
  int clients()         const     { return clients_; }

  const std::string url()                           const { return url_ + params_; }
  std::map<std::string, std::string> headers()      const { return headers_; }
  const std::string header(const std::string &key)        { return headers_[key]; }
//  std::unique_ptr<char []> entity()                       { return entity_; }
  auto trace() -> std::pair<bool, HttpTrace>        const {
    return { trace_, trace_ ? http_trace_ : HttpTrace{} };
  }

  void SetProtocol(const Protocol &protocol)  { protocol_ = protocol; }
  void SetMethod(const HttpMethod &method)    { method_   = method; }
  void SetVersion(const HttpVersion &version) { version_  = version; }
  void SetTimeout(const int &timeout)         { timeout_  = timeout; }
  void SetThreads(const int &threads)         { threads_  = threads; }
  void SetClients(const int &clients)         { clients_  = clients; }

  void SetAutoClients(const int &autoclients) {
    /* TODO: calculate the clients and threads number
     * TODO: then use SetThreads() + SetClients() to set attr.
     */
  }

  void SetUrl(const std::string &url) {
    url_ += url;
  }

  void SetParam(const char *params) {
    if (params_.empty()) {
      params_ += '?' + params;
    } else {
      params_ += '&' + params;
    }
  }

  void SetParam(const std::string &params) {
    SetParam(params.c_str());
  }

  void SetHeader(const std::string &key, const std::string &value) {
    headers_.insert({key, value});
  }

  void SetEntity(const char *entity) {
    auto len = std::strlen(entity);
    entity_ = std::make_unique<char []>(len);
    ::memcpy(entity_.get(), entity, len);
  }

  void SetEntity(const std::string &entity) {
    SetEntity(entity.c_str());
  }

  void SetTrace(bool trace, HttpTrace http_trace) {
    trace_ = trace;
    http_trace_ = trace ? http_trace : HttpTrace{};
  }

 private:
  Protocol protocol_;
  HttpMethod method_;
  HttpVersion version_;

  int timeout_;
  int threads_;  // 若为autoclient模式，这里要根据客户端数自行计算好应当的线程数和客户端数
  int clients_;

  bool trace_;
  HttpTrace http_trace_;

  std::map<std::string, std::string> headers_;
  std::string url_; // params在这里要拼好直接放在url里
  std::string params_;
  std::unique_ptr<char[]> entity_;
};

#endif //BISHE_CLI_OPTIONS_H
