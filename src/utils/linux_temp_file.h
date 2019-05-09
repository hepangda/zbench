//
// Created by pangda on 5/9/19.
//

#ifndef BISHE_LINUX_TEMP_FILE_H
#define BISHE_LINUX_TEMP_FILE_H

#include <cstdio>
#include <fcntl.h>
#include "linux_file.h"

class LinuxTempfile: LinuxFile {
 public:
  static LinuxTempfile Create() {
    return LinuxTempfile{open(tmpnam(nullptr), O_CREAT | O_RDWR)};
  }

 private:
  explicit LinuxTempfile(int fd): LinuxFile(fd) {}

};

#endif // BISHE_LINUX_TEMP_FILE_H
