#ifndef BISHE_CLI_STARTER_H
#define BISHE_CLI_STARTER_H

#include "../cli/command_option.h"
class CliStarter {
 public:
  explicit CliStarter(Option &option) : option_(option) {}

  void Start();
 private:
  Option &option_;
};

#endif // BISHE_CLI_STARTER_H
