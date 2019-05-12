/**
 * Created by Crow on 4/30/19.
 * Copyright (c) 2019 Crow All rights reserved.
 * @author Crow 
 */

#include <iostream>
#include "options.h"
#include "command_options.h"
#include "web_server.h"

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>

int main(int argc, char *argv[]) {
  CommandOptions op;
  auto pair = op.Parse(argc, argv);
  if (pair.first) {
    auto option = std::move(pair.second);
    std::cout << option->url() << std::endl;
  } else {
    WebServer server;
    server.Start();
  }

}
