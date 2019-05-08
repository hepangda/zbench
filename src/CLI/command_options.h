/**
 * Created by Crow on 4/30/19.
 * Copyright (c) 2019 Crow All rights reserved.
 * @author Crow 
 */

#ifndef BISHE_CLI_COMMAND_OPTIONS_H
#define BISHE_CLI_COMMAND_OPTIONS_H

#include "options.h"

class CommandOptions {
 public:
  CommandOptions() noexcept = default;
  const Option &Parse(int argc, char **argv);

 private:
  void ParseDoc(const rapidjson::Document &doc);
  void ParseDslStr(const char *str);
  void ParseDslFile(const char *filename);

  Option option_;
};

namespace detail {

std::pair<std::string, std::string> CutKV(const char *str);
std::pair<std::string, std::string> CutKV(const std::string &str);

}

#endif //BISHE_CLI_COMMAND_OPTIONS_H
