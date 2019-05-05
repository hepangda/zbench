/**
 * Created by Crow on 4/30/19.
 * Copyright (c) 2019 Crow All rights reserved.
 * @author Crow 
 */

#ifndef BISHE_CLI_CLI_OPTIONS_H
#define BISHE_CLI_CLI_OPTIONS_H

#include "options.h"

class CLIOptions {
 public:
  CLIOptions() noexcept = default;
  void Parse(int argc, char **argv);
  void SetOption(Option &option);

 private:
  Option option_;
};

namespace detail {

std::pair<std::string, std::string> CutKV(const char *str);
std::pair<std::string, std::string> CutKV(const std::string &str);

}

#endif //BISHE_CLI_CLI_OPTIONS_H
