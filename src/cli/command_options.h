#ifndef BISHE_COMMAND_OPTIONS_H
#define BISHE_COMMAND_OPTIONS_H

#include "option.h"

class CommandOptions {
 public:
  CommandOptions() = default;
  const Option &Parse(int argc, char **argv);

 private:
  void ParseDoc(const rapidjson::Document &doc);
  void ParseDslStr(const char *str);
  void ParseDslFile(const char *filename);

  static HttpMethod ConvertMethod(const std::string &method);

  Option option_;
};

namespace detail {

std::pair<std::string, std::string> CutKV(const char *str);
std::pair<std::string, std::string> CutKV(const std::string &str);

}

#endif // BISHE_COMMAND_OPTIONS_H
