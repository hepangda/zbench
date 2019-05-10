/**
 * Created by Crow on 5/9/19.
 * Copyright (c) 2019 Crow All rights reserved.
 * @author Crow 
 */

#include <crow.h>

#include "web_server.h"

void WebServer::Start() {
  app_.loglevel(crow::LogLevel::INFO);
  app_.port(10800)
      .multithreaded()
      .run();
}

void WebServer::NewBenchmark() {
  CROW_ROUTE(app_, "/benchmark")
      .methods("POST"_method) (
      [](const crow::request &request) {

      }
  );
}
