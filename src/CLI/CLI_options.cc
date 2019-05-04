/**
 * Created by Crow on 4/30/19.
 * Copyright (c) 2019 Crow All rights reserved.
 * @author Crow 
 */


#include <getopt.h>
#include <cstring>

#include "CLI_options.h"

static bool cli_http = true;
static bool cli_trace = false;
static int  cli_pid = -1;
static bool cli_trace_syscall = false;
static bool cli_trace_memory = false;
static bool cli_trace_cpu = false;

void CLIOptions::Parse(int argc, char **argv) {
  static struct option long_option[] = {
      {"GET",        no_argument, nullptr, 'G'},
      {"POST",       no_argument, nullptr, 'P'},
      {"PUT",        no_argument, nullptr, 'U'},
      {"DELETE",     no_argument, nullptr, 'D'},
      {"version",    no_argument, nullptr, 'v'},
      {"http",       no_argument, static_cast<int *>(&cli_http), true},
      {"https",      no_argument, static_cast<int *>(&cli_http), false},
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
      {"enable-trace",  no_argument, static_cast<int *>(&cli_trace), true},
      {"trace-syscall", no_argument, static_cast<int *>(&cli_trace_syscall), true},
      {"trace-memory",  no_argument, static_cast<int *>(&cli_trace_memory), true},
      {"trace-cpu",     no_argument, static_cast<int *>(&cli_trace_cpu), true},
  };

  int res(-1);
  while ((res = ::getopt_long(argc, argv, ":GPUDve:h:p:u:t:T:c:a:s:d:f:", long_option, nullptr)) != -1) {
    switch (res) {
      case 'G': option_.SetMethod(HttpMethod::kHMGet);    break;
      case 'P': option_.SetMethod(HttpMethod::kHMPost);   break;
      case 'U': option_.SetMethod(HttpMethod::kHMPut);    break;
      case 'D': option_.SetMethod(HttpMethod::kHMDelete); break;
      case 'v': /*TODO: print version */ break;
      case 'e': option_.SetEntity(optarg);                 break;
      case 'h':
        auto pheader = detail::CutKV(optarg, ':');
        option_.SetHeader(pheader.first, pheader.second);
        break;
      case 'p':
        auto pparam = detail::CutKV(optarg, '=');
        option_.Set


    }
  }
}

namespace detail {

std::pair<std::string, std::string> CutKV(const char *str, char ch) {
  size_t pos = 0;

  for (size_t i = 0; i < std::strlen(str); ++i) {
    pos = i;
    if (str[i] == ch)
      break;
  }

  return {std::string(str, 0, pos),
          std::string(str, pos + 1, std::strlen(str))};
};

std::pair<std::string, std::string> CutKV(const std::string &str, char ch) {
  auto pos = str.find(ch);

  return {std::string(str, 0, pos),
          std::string(str, pos + 1, str.size())};
};

}