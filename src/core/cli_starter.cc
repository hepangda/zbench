#include "cli_starter.h"
#include "../cli/opt_convert.h"
#include "../utils/http_request.h"
#include "benchmarker.h"
#include "syscall_tracer.h"
#include "trace_result.h"
#include "tracer.h"
#include "cli_printer.h"

#include <mutex>
#include <vector>
#include <iostream>

void CliStarter::Start() {
  PrintStart();

  puts("Searching endpoint...");
  ni::tcp::endpoint ep = GetEndpoint();

  puts("\n===================\nPress Enter to start...");
  std::getchar();

  puts(GetPrinter().c_str());
}

std::string CliStarter::GetPrinter() {
  HttpRequest request = GetHttpRequest();
  ni::tcp::endpoint ep = GetEndpoint();

  std::vector<std::thread> threads;

  std::list<ClientReport> bench_result;
  std::mutex mutex;

  for (int i = 0; i < option_.threads(); i++) {
    threads.emplace_back([this, request, ep, &bench_result, &mutex] {
      auto result = Benchmarker{request, ep, option_}.Start();
      {
        std::lock_guard<std::mutex> guard{mutex};
        bench_result.insert(bench_result.end(), result.begin(), result.end());
      }
    });
  }

  std::list<TraceResult> trace_result;
  SyscallResult syscall_result;
  if (option_.trace().first) {
    if (option_.trace().second.trace_cpu_) {
      threads.emplace_back([this, &trace_result] {
        Tracer it{option_.trace().second.server_pid_, 1, trace_result};
        for (int i = 0; i < option_.timeout(); i++) {
          it.DoTrace();
          sleep(1);
        }
      });
    }

    if (option_.trace().second.trace_syscall_) {
      syscall_result = SyscallTracer{}.Start(option_.trace().second.server_pid_, option_.timeout());
    }
  }

  for (auto &i: threads) {
    i.join();
  }

  return CliPrinter{bench_result, trace_result, syscall_result, option_}.Print();
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
    printf("  %s Entity: %s\n", protocol, option_.entity().get());
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

  if (option_.entity()) {
    auto ptr = MakeSharedCharArray(strlen(option_.entity().get()));
    std::strcpy(ptr.get(), option_.entity().get());
    request.PutBody(ptr, strlen(option_.entity().get()));
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

  n::io_context io_ctx;
  ni::tcp::resolver resolver(io_ctx);
  ni::tcp::resolver::query query(use_url, OptConvert::ConvertService(option_.protocol()));

  ni::tcp::endpoint ep = *resolver.resolve(query).begin();

  if (!port.empty()) {
    ep.port(std::stoi(port));
  }
  return ep;
}