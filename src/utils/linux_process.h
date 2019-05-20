#ifndef BISHE_LINUX_PROCESS_H
#define BISHE_LINUX_PROCESS_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <utility>
#include <vector>
#include <cstring>
#include <stdexcept>
#include <dirent.h>

class LinuxProcess {
 public:
  ~LinuxProcess() {
    Close();
  }

  int GetPid() const { return child_id_; }

  template<typename ...Args>
  int Exec(const char *path, Args... args) {
    constexpr int READ_PORT = 0;
    constexpr int WRITE_PORT = 1;
    int out_fd[2] = {0}, err_fd[2] = {0};
    pipe(out_fd);
    pipe(err_fd);

    child_id_ = fork();
    if (child_id_ != 0) {
      close(out_fd[WRITE_PORT]);
      close(err_fd[WRITE_PORT]);
      stdout_fd_ = out_fd[READ_PORT];
      stderr_fd_ = err_fd[READ_PORT];
      if (!async_) {
        int ret;
        waitpid(child_id_, &ret, 0);
        return ret;
      } else {
        return -2;
      }
    } else if (child_id_ == 0) {
      close(out_fd[READ_PORT]);
      dup2(err_fd[WRITE_PORT], STDERR_FILENO);
      dup2(out_fd[WRITE_PORT], STDOUT_FILENO);
      execl(path, path, std::forward<Args>(args)..., nullptr);
    } else {
      throw std::runtime_error("fork failed.");
    }

    return 0;
  }

  template<typename ...Args>
  int ExecCommand(const char *command, Args... args) {
    std::string path = GetCompletePath(command);
    if (path.empty()) {
      return -1;
    }

    return Exec(path.c_str(), std::forward<Args>(args)...);
  }

  void SetAsync() {
    async_ = true;
  }

  void Kill() {
    kill(child_id_, SIGINT);
  }

  int ReadStdout(void *dest, int bytes) {
    return read(stdout_fd_, dest, bytes);
  }

  std::string GetAllStdout() {
    std::string result;
    char buffer[4096]{0};

    while (ReadStdout(buffer, 4096) > 0) {
      result += buffer;
      memset(buffer, 0, sizeof(buffer));
    }

    return result;
  }

  int ReadStderr(void *dest, int bytes) {
    return read(stderr_fd_, dest, bytes);
  }

  std::string GetAllStderr() {
    std::string result;
    char buffer[4096]{0};

    while (ReadStderr(buffer, 4096) > 0) {
      result += buffer;
      memset(buffer, 0, sizeof(buffer));
    }

    return result;
  }

  void Close() {
    if (stdout_fd_ != -1) {
      close(stdout_fd_);
    }

    if (stderr_fd_ != -1) {
      close(stderr_fd_);
    }
  }

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
  int stdout_fd_{-1};
  int stderr_fd_{-1};
  bool async_{false};
};

#endif // BISHE_LINUX_PROCESS_H
