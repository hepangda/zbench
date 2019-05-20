#ifndef BISHE_COMMAND_OPTION_H
#define BISHE_COMMAND_OPTION_H

#include "option.h"

#include <memory>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

class CommandOptions {
 public:
  CommandOptions() : option_(std::make_unique<Option>()) {}
  std::pair<bool, std::unique_ptr<Option>> Parse(int argc, char **argv);
  void ParseDoc(const rapidjson::Document &doc);
  std::unique_ptr<Option> GetOption() {
    return std::move(option_);
  }
 private:
  void ParseDslStr(const char *str);
  void ParseDslFile(const char *filename);

  std::unique_ptr<Option> option_;

  static HttpMethod ConvertHttpMethod(const std::string &method_string);
};

namespace detail {

std::pair<std::string, std::string> CutKV(const char *str);
std::pair<std::string, std::string> CutKV(const std::string &str);

}

#endif // BISHE_COMMAND_OPTION_H
