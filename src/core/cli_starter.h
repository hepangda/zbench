#ifndef BISHE_CLI_STARTER_H
#define BISHE_CLI_STARTER_H

#include "../utils/net.h"
#include "../cli/command_option.h"
#include "../utils/http_request.h"

class CliStarter {
 public:
  explicit CliStarter(Option &option) : option_(option) {}

  void Start();
  std::string GetPrinter();
  void PrintStart();
  ni::tcp::endpoint GetEndpoint();
  HttpRequest GetHttpRequest();
 private:
  Option &option_;
};

#endif // BISHE_CLI_STARTER_H
