/**
 * Created by Crow on 4/30/19.
 * Copyright (c) 2019 Crow All rights reserved.
 * @author Crow 
 */

#ifndef BISHE_CLI_OPTIONS_H
#define BISHE_CLI_OPTIONS_H

#include <unistd.h>

#include <map>
#include <string>
#include <memory>

enum HttpMethod {
  kHMGet,
  kHMPost,
  kHMPut,
  kHMDelete,
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
  Protocol protocol()   const     { return protocol_; }
  HttpMethod method()   const     { return method_; }
  HttpVersion version() const     { return version_; }
  int timeout()         const     { return timeout_; }
  int threads()         const     { return threads_; }
  int clients()         const     { return clients_; }

  const std::string url()                           const { return url_; }
  std::map<std::string, std::string> headers()      const { return headers_; }
  const std::string header(const std::string &key)  const { return headers_[key]; }
  std::unique_ptr<char []> entity()                       { return entity_; }
  auto trace() -> std::pair<bool, HttpTrace>        const {
    return { trace_, trace_ ? http_trace_ : nullptr};
  }

  void set_protocol(const Protocol &protocol)  { protocol_ = protocol; }
  void set_method(const HttpMethod &method)    { method_ = method; }
  void set_version(const HttpVersion &version) { version_ = version; }
  void set_timeout(const int &timeout)         { timeout_ = timeout; }
  void set_threads(const int &threads)         { threads_ = threads; }
  void set_clients(const int &clients)         { clients_ = clients; }
  void set_url(const std::string &url, const std::map<std::string, std::string> &param) {}
  void set_header(const std::string &key, const std::string &value) {
    headers_.insert({key, value});
  }
  void set_entity(const std::string &entity) {}
  void set_trace(bool trace, HttpTrace http_trace) {
    trace_ = trace;
    http_trace_ = trace ? http_trace : nullptr;
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
  std::unique_ptr<char[]> entity_;
};

#endif //BISHE_CLI_OPTIONS_H
