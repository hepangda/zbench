#include "cli_starter.h"
#include "../cli/opt_convert.h"
#include "../utils/http_request.h"
#include "benchmarker.h"

#include <regex>
#include <iostream>

void CliStarter::Start() {
  PrintStart();

  puts("Producing Http Request...");
  HttpRequest request = GetHttpRequest();

  puts("Searching endpoint...");
  ni::tcp::endpoint ep = GetEndpoint();

  puts("\n===================\nPress Enter to start...");
  std::getchar();

  Benchmarker{request, ep, std::move(option_)}.Start();
}

void CliStarter::PrintStart() {
  puts("WebServer Test v1.0 --- CommandLine Version");
  printf("  Url: %s\n", option_.url().c_str());
  printf("  Timeout: %d\n", option_.timeout());

  const char *protocol = OptConvert::ConvertProtocol(option_.protocol());
  printf("  %s Version: %s\n", protocol, OptConvert::ConvertVersion(option_.version()));
  printf("  %s Method: %s\n", protocol, OptConvert::ConvertMethod(option_.method()));

  if (option_.headers().empty()) {
    printf("  %s Headers: (null)\n", protocol);
  } else {
    printf("  %s Headers: \n", protocol);
    for (const auto &i: option_.headers()) {
      printf("    %s: %s\n", i.first.c_str(), i.second.c_str());
    }
  }

  if (!option_.entity()) {
    printf("  %s Entity: (null)\n", protocol);
  } else {
    printf("  %s Entity: %s", protocol, option_.entity().get());
  }

  printf("  Threads: %d\n", option_.threads());
  printf("  Clients per thread: %d\n", option_.clients());

  auto trace = option_.trace();
  printf("  Trace: %s\n", (trace.first) ? "enable" : "disable");
  if (trace.first) {
    printf("  Server pid: %d\n", trace.second.server_pid_);
    printf("  Trace syscall: %s\n", (trace.second.trace_syscall_) ? "enable" : "disable");
    printf("  Trace memory: %s\n", (trace.second.trace_memory_) ? "enable" : "disable");
    printf("  Trace cpu: %s\n", (trace.second.trace_cpu_) ? "enable" : "disable");
  }

  puts("");
}

HttpRequest CliStarter::GetHttpRequest() {
  HttpRequest request;
  request.SetMethod(OptConvert::ConvertMethod(option_.method()));
  request.SetRequestUri(option_.url());
  request.SetVersion(OptConvert::ConvertVersionString(option_.version()));

  for (const auto &i:option_.headers()) {
    request.SetHeaderItem(i.first, i.second);
  }

  auto rawptr = option_.entity().release();
  if (rawptr != nullptr) {
    auto ptr = MakeSharedCharArray(strlen(rawptr));
    std::strcpy(ptr.get(), rawptr);
    request.PutBody(ptr, strlen(rawptr));
  }

  return std::move(request);
}

ni::tcp::endpoint CliStarter::GetEndpoint() {
  const std::string &url = option_.url();
  std::string use_url{};
  std::string port{};

  int iter = -1;
  for (iter = url.find("://") + 3; iter < url.size(); iter++) {
    if (url[iter] == '/') {
      break;
    } else if (url[iter] == ':') {
      for (int j = iter + 1; url[j] != '/' && j < url.size(); j++) {
        port += url[j];
      }
      break;
    } else {
      use_url += url[iter];
    }
  }

  if (iter == -1) {
    throw std::runtime_error("Invalid url.");
  }

  // printf("url = %s\nport = %s\n", use_url.c_str(), port.c_str());

  n::io_context io_ctx;
  ni::tcp::resolver resolver(io_ctx);
  ni::tcp::resolver::query query(use_url, OptConvert::ConvertService(option_.protocol()));

  ni::tcp::endpoint ep = *resolver.resolve(query).begin();

  if (!port.empty()) {
    ep.port(std::stoi(port));
  }
  return ep;
}