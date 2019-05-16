#include <iostream>
#include "src/cli/command_option.h"
#include "src/cli/web_server.h"
#include "src/core/cli_starter.h"

int main(int argc, char *argv[]) {
  CommandOptions op;
  try {
    auto pair = op.Parse(argc, argv);
    if (pair.first) {
      CliStarter starter{*pair.second};
      starter.Start();
    } else {
      WebServer server;
      server.Start();
    }
  } catch (const std::exception &ex) {
    std::printf("Occured error: %s\n", ex.what());
  }
}