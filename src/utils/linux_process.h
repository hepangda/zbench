//
// Created by pangda on 5/9/19.
//

#ifndef BISHE_LINUX_PROCESS_H
#define BISHE_LINUX_PROCESS_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <utility>
#include <vector>
#include <dirent.h>

class LinuxProcess {
 public:
  int GetPid() const { return child_id_; }

  template<typename ...Args>
  int BlockExec(const char *path, Args... args) {
    child_id_ = fork();
    execl(path, std::forward<Args>(args)..., nullptr);

    if (child_id_ != 0) {
      int ret;
      waitpid(child_id_, &ret, 0);
      return ret;
    } else {
      _exit(-1);
    }
  }

  template<typename ...Args>
  int BlockExecCommand(const char *command, Args... args) {
    std::string path = GetCompletePath(command);
    if (path.empty()) {
      return -1;
    }

    child_id_ = fork();
    execl(path.c_str(), std::forward<Args>(args)..., nullptr);

    if (child_id_ != 0) {
      int ret;
      waitpid(child_id_, &ret, 0);
      return ret;
    } else {
      _exit(-1);
    }
  }

  void SetStdout(int fd) { stdout_fd_ = fd; }
  void SetStdin(int fd) { stdin_fd_ = fd; }

  static std::string GetCompletePath(const char *command) {
    const std::vector<std::string> &path = GetPath();

    for (const auto &it : path) {
      std::string complete_path = it + '/' + command;
      if (access(complete_path.c_str(), F_OK) == 0) {
        return complete_path;
      }
    }

    return {};
  }

 private:
  static std::vector<std::string> GetPath() {
    static std::vector<std::string> path_vector;
    static constexpr char sep = ':';

    if (path_vector.empty()) {
      const std::string path_string = getenv("PATH");

      unsigned int start = 0;
      for (auto i = 0u; i < path_string.length(); i++) {
        if (path_string[i] == sep) {
          path_vector.push_back(path_string.substr(start, i - start));
          start = i + 1;
        }
      }
      path_vector.push_back(path_string.substr(start, path_string.length()));
    }

    return path_vector;
  }

  int child_id_{-1};
  int stdin_fd_{-1};
  int stdout_fd_{-1};
};

#endif // BISHE_LINUX_PROCESS_H
