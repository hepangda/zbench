#ifndef BISHE_CLIENT_REPORT_H
#define BISHE_CLIENT_REPORT_H

#include <cstddef>

class ClientReport {
 public:
  size_t GetReadBytes() const {
    return read_bytes_;
  }

  size_t GetWriteBytes() const {
    return write_bytes_;
  }

  const char *GetErrorMessage() const {
    return error_msg_;
  }

  void ReportReadByte(int diff = 1) {
    read_bytes_ += diff;
  }

  void ReportWriteByte(int diff = 1) {
    write_bytes_ += diff;
  }

  void ReportError(const char *error) {
    error_msg_ = error;
  }

  void Reset() {
    read_bytes_ = 0;
    write_bytes_ = 0;
    error_msg_ = nullptr;
  }
 private:
  size_t read_bytes_{0};
  size_t write_bytes_{0};
  const char *error_msg_{nullptr};
};

#endif // BISHE_CLIENT_REPORT_H
