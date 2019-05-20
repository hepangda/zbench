#include "command_option.h"
#include "optfwd.h"

#include <getopt.h>
#include <cstring>

#include <fstream>
#include <iostream>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/istreamwrapper.h>

static bool cli_http = true;
static bool cli_trace = false;
static int cli_pid = -1;
static bool cli_trace_syscall = false;
static bool cli_trace_memory = false;
static bool cli_trace_cpu = false;
static bool cli_daemon = false;

/**
 * The core function of CommandOptions to parse the arguments.
 * @param argc  the count of command arguments
 * @param argv  the command arguments
 */
std::pair<bool, std::unique_ptr<Option>> CommandOptions::Parse(int argc, char **argv) {
  HttpTrace cli_http_trace{};
  std::pair<std::string, std::string> pheader;
  static struct option long_option[] = {
      {"GET", no_argument, nullptr, 'G'},
      {"POST", no_argument, nullptr, 'P'},
      {"PUT", no_argument, nullptr, 'U'},
      {"DELETE", no_argument, nullptr, 'D'},
      {"version", required_argument, nullptr, 'v'},
      {"entity", required_argument, nullptr, 'e'},
      {"header", required_argument, nullptr, 'h'},
      {"param", required_argument, nullptr, 'p'},
      {"url", required_argument, nullptr, 'u'},
      {"timeout", required_argument, nullptr, 't'},
      {"Threads", required_argument, nullptr, 'T'},
      {"clients", required_argument, nullptr, 'c'},
      {"auto", required_argument, nullptr, 'a'},
      {"server-pid", required_argument, nullptr, 's'},
      {"dsl", required_argument, nullptr, 'd'},
      {"file", required_argument, nullptr, 'f'},
      {"http", no_argument, reinterpret_cast<int *>(&cli_http), true},
      {"https", no_argument, reinterpret_cast<int *>(&cli_http), false},
      {"daemon", no_argument, reinterpret_cast<int *>(&cli_daemon), true},
      {"enable-trace", no_argument, reinterpret_cast<int *>(&cli_trace), true},
      {"trace-syscall", no_argument, reinterpret_cast<int *>(&cli_trace_syscall), true},
      {"trace-memory", no_argument, reinterpret_cast<int *>(&cli_trace_memory), true},
      {"trace-cpu", no_argument, reinterpret_cast<int *>(&cli_trace_cpu), true},
  };

  int res{-1};
  while ((res = ::getopt_long(argc, argv, ":GPUDv:e:h:p:u:t:T:c:a:s:d:f:", long_option, nullptr)) != -1) {
    switch (res) {
      case 'G':
        option_->SetMethod(HttpMethod::kHMGet);
        break;
      case 'P':
        option_->SetMethod(HttpMethod::kHMPost);
        break;
      case 'U':
        option_->SetMethod(HttpMethod::kHMPut);
        break;
      case 'D':
        option_->SetMethod(HttpMethod::kHMDelete);
        break;
      case 'e':
        option_->SetEntity(optarg);
        break;
      case 'h':
        pheader = detail::CutKV(optarg);
        option_->SetHeader(pheader.first, pheader.second);
        break;
      case 'p':
        option_->SetParam(optarg);
        break;
      case 'u':
        option_->SetUrl(optarg);
        break;
      case 't':
        option_->SetTimeout(std::stol(optarg));
        break;
      case 'T':
        option_->SetThreads(std::stol(optarg));
        break;
      case 'c':
        option_->SetClients(std::stol(optarg));
        break;
      case 'a':
        option_->SetAutoClients(std::stol(optarg));
        break;
      case 'v':
        if (strcmp(optarg, "1.0") == 0) {
          option_->SetVersion(HttpVersion::kHV1_0);
        } else {
          option_->SetVersion(HttpVersion::kHV1_1);
        }

      case 's':
        cli_http_trace = {std::stoi(optarg), cli_trace_syscall, cli_trace_memory, cli_trace_cpu};
        option_->SetTrace(cli_trace, cli_http_trace);
        break;

      case 'd':
        try {
          ParseDslStr(optarg);
          return {true, std::move(option_)};
        } catch (const std::runtime_error &err) {
          std::cerr << err.what() << std::endl;
          std::exit(EXIT_FAILURE);
        }

      case 'f':
        try {
          ParseDslFile(optarg);
          return {true, std::move(option_)};
        } catch (const std::runtime_error &err) {
          std::cerr << err.what() << std::endl;
          std::exit(EXIT_FAILURE);
        }

      case ':':
        throw std::runtime_error("missing arguments");
      case '?':
        throw std::runtime_error("unrecognized option");
      default :
        break;
    }
  }

  // handling CLI mode and daemon mode are set at the same time
  if (cli_daemon) {
    return {false, nullptr};
  }

  // handling some unreasonable options settings
  // set Http version
  option_->SetProtocol(cli_http ? Protocol::kPHttp : Protocol::kPHttps);

  // set HttpTrace
  if (!cli_trace) {
    option_->SetTrace(false, {-1, false, false, false});
  } else {
    cli_http_trace.trace_syscall_ = cli_trace_syscall;
    cli_http_trace.trace_memory_ = cli_trace_memory;
    cli_http_trace.trace_cpu_ = cli_trace_cpu;
    option_->SetTrace(true, cli_http_trace);
  }

  if (option_->threads() <= 0) {
    option_->SetThreads(1);
  }

  if (option_->clients() <= 0) {
    option_->SetClients(1);
  }

  return {true, std::move(option_)};
}

/**
 * @brief Parse DSL string which came from commandline arguments
 * @param str the DSL string to be parsed
 */
void CommandOptions::ParseDslStr(const char *str) {
  rapidjson::Document doc;
  doc.Parse(str);

  if (doc.HasParseError()) {
    throw std::runtime_error(rapidjson::GetParseError_En(doc.GetParseError()));
  }
  ParseDoc(doc);
}

/**
 * @brief Parse DSL file by rapidjson::IStreamWrapper
 * @param filename
 */
void CommandOptions::ParseDslFile(const char *filename) {
  std::ifstream stream(filename);
  rapidjson::IStreamWrapper fstream(stream);
  rapidjson::Document doc;

  doc.ParseStream(fstream);
  if (doc.HasParseError()) {
    throw std::runtime_error(rapidjson::GetParseError_En(doc.GetParseError()));
  }
  ParseDoc(doc);
}

/**
 * @brief The core of private function
 *            - CommandOptions::ParseDslStr()
 *            - CommandOptions::ParseDslFile()
 * @param doc rapidjson::Document doc is a JSON DOM tree
 *            used for for search K-V and modify K-V
 */
void CommandOptions::ParseDoc(const rapidjson::Document &doc) {
  // set HTTP method
  if (!doc.HasMember("method") || !doc["method"].IsString()) {
    throw std::runtime_error("Parse DSL failed, when parsing \"method\".");
  }
  option_->SetMethod(ConvertHttpMethod(doc["method"].GetString()));

  // set url params
  if (doc.HasMember("params")) {
    if (!doc["params"].IsArray()) {
      throw std::runtime_error("Parse DSL failed, when parsing \"params\".");
    }

    for (auto &var : doc["params"].GetArray()) {
      if (!var.IsString()) {
        throw std::runtime_error("Parse DSL failed, when parsing \"params item\".");
      }
      option_->SetParam(var.GetString());
    }
  }

  // set Http headers
  if (doc.HasMember("headers")) {
    if (!doc["headers"].IsArray()) {
      throw std::runtime_error("Parse DSL failed, when parsing \"headers\".");
    }

    for (auto &var : doc["headers"].GetArray()) {
      if (!var.IsString()) {
        throw std::runtime_error("Parse DSL failed, when parsing \"headers item\".");
      }
      auto pair = detail::CutKV(var.GetString());
      option_->SetHeader(pair.first, pair.second);
    }
  }

  // set HTTP/HTTPS
  if (doc.HasMember("protocol")) {
    if (!doc["protocol"].IsString()) {
      throw std::runtime_error("Parse DSL failed, when parsing \"protocol\".");
    }

    if (strcmp(doc["protocol"].GetString(), "http") == 0) {
      option_->SetProtocol(Protocol::kPHttp);
    } else if (strcmp(doc["protocol"].GetString(), "https") == 0) {
      option_->SetProtocol(Protocol::kPHttps);
    } else {
      throw std::runtime_error("Unsupported protocol.");
    }
  } else {
    option_->SetProtocol(Protocol::kPHttp);
  }

  // set version
  if (doc.HasMember("version")) {
    if (!doc["version"].IsString()) {
      throw std::runtime_error("Parse DSL failed, when parsing \"version\".");
    }

    if (strcmp(doc["version"].GetString(), "1.0") == 0) {
      option_->SetVersion(HttpVersion::kHV1_0);
    } else if (strcmp(doc["protocol"].GetString(), "1.1") == 0) {
      option_->SetVersion(HttpVersion::kHV1_1);
    } else {
      throw std::runtime_error("Unsupported version.");
    }
  } else {
    option_->SetVersion(HttpVersion::kHV1_0);
  }

  // set url
  if (!doc.HasMember("url") || !doc["url"].IsString()) {
    throw std::runtime_error("Parse DSL failed, when parsing \"url\".");
  }
  option_->SetUrl(doc["url"].GetString());

  // set entity
  if (doc.HasMember("entity")) {
    if (doc["entity"].IsString()) {
      option_->SetEntity(doc["entity"].GetString());
    } else {
      throw std::runtime_error("Parse DSL failed, when parsing \"entity\".");
    }
  }

  // set timeout
  if (!doc.HasMember("timeout") || !doc["timeout"].IsInt()) {
    throw std::runtime_error("Parse DSL failed, when parsing \"timeout\".");
  }
  option_->SetTimeout(doc["timeout"].GetInt());

  // set concurrency
  if (!doc.HasMember("concurrency") || !doc["concurrency"].IsObject()) {
    throw std::runtime_error("Parse DSL failed, when parsing \"concurrency\".");
  }
  auto con_doc = doc["concurrency"].GetObject();
  if (!con_doc.HasMember("mode") || !con_doc["mode"].IsString()) {
    throw std::runtime_error("Parse DSL failed, when parsing \"mode\".");
  }

  if (strcmp(con_doc["mode"].GetString(), "auto") == 0) {
    if (!con_doc.HasMember("autoclients") || !con_doc["autoclients"].IsInt()) {
      throw std::runtime_error("Parse DSL failed, when parsing \"autoclients\".");
    }
    option_->SetAutoClients(con_doc["autoclients"].GetInt());
  } else if (strcmp(con_doc["mode"].GetString(), "explicit") == 0) {
    if (!con_doc["clients"].IsInt() || !con_doc["threads"].IsInt()) {
      throw std::runtime_error("Parse DSL failed, when parsing \"autoclients\".");
    }
    option_->SetThreads(con_doc["threads"].GetInt());
    option_->SetClients(con_doc["clients"].GetInt());
  } else {
    option_->SetThreads(1);
    option_->SetClients(1);
  }

  // set trace
  if (doc.HasMember("trace") && doc["trace"].IsObject()) {
    auto trace_doc = doc["trace"].GetObject();
    if (!trace_doc.HasMember("enabled") || !trace_doc["enabled"].IsBool()) {
      throw std::runtime_error("Parse DSL failed, when parsing \"enabled\".");
    }

    if (trace_doc["enabled"].GetBool()) {
      option_->SetTrace(true, {trace_doc["serverpid"].GetInt(), trace_doc["syscall"].GetBool(),
          trace_doc["memory"].GetBool(), trace_doc["cpu"].GetBool()});
    } else {
      option_->SetTrace(false, HttpTrace{});
    }
  }
}

HttpMethod CommandOptions::ConvertHttpMethod(const std::string &method_string) {
  static const std::map<std::string, HttpMethod> method_map{
      {"get", HttpMethod::kHMGet},
      {"post", HttpMethod::kHMPost},
      {"put", HttpMethod::kHMPut},
      {"delete", HttpMethod::kHMDelete},
  };

  return method_map.find(method_string)->second;
}

namespace detail {

/**
 * @brief CutKV() is used to parse the string to KV pair,
 *        this can also be extended to be cut by ch(arguments)
 * @param str the string to be cut
 * @return KV pair pair.first is key, and pair.second is value
 */
std::pair<std::string, std::string> CutKV(const char *str) {
  std::size_t pos = 0;
  for (std::size_t i = 0; i < std::strlen(str); ++i) {
    pos = i;
    if (str[i] == ':')
      break;
  }
  return {std::string(str, 0, pos), std::string(str, pos + 1, std::strlen(str))};
}

/**
 * @brief CutKV() overload function, it can be entrust to CutKV(const char *str) by std::string::c_str()
 * @param str the string to be cut
 * @return KV pair pair.first is key, and pair.second is value
 */
std::pair<std::string, std::string> CutKV(const std::string &str) {
  auto pos = str.find(':');
  return {std::string(str, 0, pos), std::string(str, pos + 1, str.size())};
}

}