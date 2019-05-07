/**
 * Created by Crow on 4/30/19.
 * Copyright (c) 2019 Crow All rights reserved.
 * @author Crow 
 */


#include <getopt.h>
#include <cstring>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

#include "CLI_options.h"

static bool cli_http = true;
static bool cli_trace = false;
static int  cli_pid = -1;
static bool cli_trace_syscall = false;
static bool cli_trace_memory = false;
static bool cli_trace_cpu = false;

void CLIOptions::Parse(int argc, char **argv) {
  HttpTrace cli_http_trace{};
  std::pair<std::string, std::string> pheader;
  static struct option long_option[] = {
      {"GET",        no_argument, nullptr, 'G'},
      {"POST",       no_argument, nullptr, 'P'},
      {"PUT",        no_argument, nullptr, 'U'},
      {"DELETE",     no_argument, nullptr, 'D'},
      {"version",    no_argument, nullptr, 'v'},
      {"http",       no_argument, reinterpret_cast<int *>(&cli_http), true},
      {"https",      no_argument, reinterpret_cast<int *>(&cli_http), false},
      {"entity",     required_argument, nullptr, 'e'},
      {"header",     required_argument, nullptr, 'h'},
      {"param",      required_argument, nullptr, 'p'},
      {"url",        required_argument, nullptr, 'u'},
      {"timeout",    required_argument, nullptr, 't'},
      {"Threads",    required_argument, nullptr, 'T'},
      {"clients",    required_argument, nullptr, 'c'},
      {"auto",       required_argument, nullptr, 'a'},
      {"server-pid", required_argument, nullptr, 's'},
      {"dsl",        required_argument, nullptr, 'd'},
      {"file",       required_argument, nullptr, 'f'},
      {"enable-trace",  no_argument, reinterpret_cast<int *>(&cli_trace), true},
      {"trace-syscall", no_argument, reinterpret_cast<int *>(&cli_trace_syscall), true},
      {"trace-memory",  no_argument, reinterpret_cast<int *>(&cli_trace_memory), true},
      {"trace-cpu",     no_argument, reinterpret_cast<int *>(&cli_trace_cpu), true},
  };

  int res(-1);
  while ((res = ::getopt_long(argc, argv, ":GPUDve:h:p:u:t:T:c:a:s:d:f:", long_option, nullptr)) != -1) {
    switch (res) {
      case 'G': option_.SetMethod(HttpMethod::kHMGet);    break;
      case 'P': option_.SetMethod(HttpMethod::kHMPost);   break;
      case 'U': option_.SetMethod(HttpMethod::kHMPut);    break;
      case 'D': option_.SetMethod(HttpMethod::kHMDelete); break;
      case 'v': /*TODO: print version */                 break;
      case 'e': option_.SetEntity(optarg);                 break;
      case 'h':
        pheader = detail::CutKV(optarg);
        option_.SetHeader(pheader.first, pheader.second);
        break;
      case 'p': option_.SetParam(optarg);                  break;
      case 'u': option_.SetUrl(optarg);                    break;
      case 't': option_.SetTimeout(atoi(optarg));          break;
      case 'T': option_.SetThreads(atoi(optarg));          break;
      case 'c': option_.SetClients(atoi(optarg));          break;
      case 'a': option_.SetAutoClients(atoi(optarg));      break;
      case 's':
        cli_http_trace = {atoi(optarg), cli_trace_syscall, cli_trace_memory, cli_trace_cpu};
        option_.SetTrace(cli_trace, cli_http_trace);
        break;
      case 'd': break;
      case 'f': break;
      case ':': break;
      case '?': break;
      default : break;

    }
  }
}

void CLIOptions::PraseDSL(const char *str) {
  rapidjson::Document doc;
  doc.Parse(str);

  if (doc.HasParseError()) {
    throw std::runtime_error("DSL file parse error");
  }

  // set HTTP method
  assert(doc["method"].IsString());
  option_.SetMethod(method_map.find(doc["method"].GetString())->second);

  // set url params
  assert(doc["params"].IsArray());
  for (auto &var : doc["params"].GetArray()) {
    assert(var.IsString());
    option_.SetParam(var.GetString());
  }

  // set Http headers
  assert(doc["headers"].IsArray());
  for (auto &var : doc["headers"].GetArray()) {
    assert(var.IsString());
    auto pair = detail::CutKV(var.GetString());
    option_.SetHeader(pair.first, pair.second);
  }

  // set HTTP/HTTPS
  assert(doc["protocol"].IsString());
  cli_http = doc["protocol"].GetString() == "http";

  // set url
  assert(doc["url"].IsString());
  option_.SetUrl(doc["url"].GetString());

  // set entity
  assert(doc["entity"].IsString());
  option_.SetEntity(doc["entity"].GetString());

  // set timeout
  assert(doc["timeout"].IsInt());
  option_.SetTimeout(doc["timeout"].GetInt());

  // set concurrency
  assert(doc["concurrency"].IsObject());
  auto con_doc = doc["concurrency"].GetObject();
  assert(con_doc["mode"].IsString());

  if (con_doc["mode"].GetString() == "auto") {
    assert(con_doc["autoclients"].IsInt());
    option_.SetAutoClients(con_doc["autoclinets"].GetInt());
  } else {
    assert(con_doc["threads"].IsInt());
    assert(con_doc["clients"].IsInt());
    option_.SetThreads(con_doc["threads"].GetInt());
    option_.SetClients(con_doc["clients"].GetInt());
  }

  // set trace
  assert(doc["trace"].IsObject());
  auto trace_doc = doc["trace"].GetObject();
  assert(trace_doc["enabled"].IsBool());

  if (trace_doc["enabled"].GetBool()) {
    assert(trace_doc["serverpid"].IsInt());
    assert(trace_doc["syscall"].IsBool());
    assert(trace_doc["memory"].IsBool());
    assert(trace_doc["cpu"].IsBool());

    option_.SetTrace(true, {trace_doc["syscall"].GetBool(),
                            trace_doc["memory"].GetBool(),
                            trace_doc["cpu"].GetBool()});
  } else {
    option_.SetTrace(false, HttpTrace{});
  }

}

namespace detail {

std::pair<std::string, std::string> CutKV(const char *str) {
  size_t pos = 0;

  for (size_t i = 0; i < std::strlen(str); ++i) {
    pos = i;
    if (str[i] == ':')
      break;
  }

  return {std::string(str, 0, pos),
          std::string(str, pos + 1, std::strlen(str))};
};

std::pair<std::string, std::string> CutKV(const std::string &str) {
  auto pos = str.find(':');

  return {std::string(str, 0, pos),
          std::string(str, pos + 1, str.size())};
};

}