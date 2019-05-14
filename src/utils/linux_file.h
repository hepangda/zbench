#ifndef BISHE_LINUX_FILE_H
#define BISHE_LINUX_FILE_H

#include <fcntl.h>
#include <unistd.h>

#include <cstddef>

enum FileMode {
  kFMReadOnly,
  kFMWriteOnly,
  kFMReadWrite,
};

class LinuxFile {
 public:
  LinuxFile(const char *path, FileMode mode) : fd_(open(path, mode)) {}
  LinuxFile(LinuxFile &&rhs) noexcept : fd_(rhs.fd_) { rhs.fd_ = -1; }

  ~LinuxFile() {
    if (fd_ != -1) {
      close(fd_);
    }
  }

  int Write(void *src, std::size_t size) {
    return write(fd_, src, size);
  }

  int Read(void *dest, std::size_t size) {
    return read(fd_, dest, size);
  }
 protected:
  explicit LinuxFile(int fd) : fd_(fd) {}
 private:
  int fd_;
};

#endif // BISHE_LINUX_FILE_H
