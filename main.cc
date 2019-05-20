#include <iostream>
#include "src/cli/command_option.h"
#include "src/cli/web_server.h"
#include "src/core/cli_starter.h"
#include "src/core/syscall_result.h"
#include "src/utils/linux_process.h"
#include <unistd.h>

int main(int argc, char *argv[]) {
//  LinuxProcess lp;
//  lp.SetAsync();
//  lp.ExecCommand("strace", "-f", "-c", "-p", "942");
//  sleep(4);
//  lp.Kill();
////  lp.ExecCommand("/root/a.out", "-eo", "pid", "");
//  auto p = lp.GetAllStderr();
//
//  auto sr = SyscallResult::FromString(p);
//  sr.Print();

  CommandOptions op;
  try {
    auto pair = op.Parse(argc, argv);
    if (pair.first) {
      CliStarter starter{*pair.second};
      starter.Start();
    } else {
      WebServer server;
      server.Start();
    }
  } catch (const std::exception &ex) {
    std::printf("Occured error: %s\n", ex.what());
  }
}