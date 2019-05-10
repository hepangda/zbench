/**
 * Created by Crow on 5/9/19.
 * Copyright (c) 2019 Crow All rights reserved.
 * @author Crow 
 */

#ifndef BISHE_CLI_WEB_SERVER_H
#define BISHE_CLI_WEB_SERVER_H

#include <crow.h>

#include "buffer.hpp"

class WebServer {
 public:
  WebServer() = default;
  void Start();

 private:
  void NewBenchmark();                // POST (browser)
  void SendBenchmarkResponse();       // GET  (browser)
  void ReceiveBenchmarkResult();      // POST (server)
  void SendBenchmarkRequest();        // GET  (server)

  std::unique_ptr<Buffer<crow::request>>  request_buffer_;
  std::unique_ptr<Buffer<crow::response>> response_buffer_;
  crow::SimpleApp app_;
};

#endif //BISHE_CLI_WEB_SERVER_H
