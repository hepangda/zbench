#ifndef BISHE_CLIENT_REPORT_H
#define BISHE_CLIENT_REPORT_H

#include <cstddef>
#include "../utils/time_utils.h"

class ClientReport {
 public:
  std::size_t GetReadBytes() const {
    return read_bytes_;
  }

  std::size_t GetWriteBytes() const {
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

  void ReportConnectedTime() {
    if (connected_time_ == -1) {
      connected_time_ = TimeUtils::Now();
    }
  }

  void ReportWriteStartTime() {
    if (write_start_time_ == -1) {
      write_start_time_ = TimeUtils::Now();
    }
  }

  void ReportWriteEndTime() {
    if (write_end_time_ == -1) {
      write_end_time_ = TimeUtils::Now();
    }
  }

  void ReportReadStartTime() {
    if (read_start_time_ == -1) {
      read_start_time_ = TimeUtils::Now();
    }
  }

  void ReportReadEndTime() {
    if (read_end_time_ == -1) {
      read_end_time_ = TimeUtils::Now();
    }
  }

  void ReportEndTime() {
    if (end_time_ == -1) {
      end_time_ = TimeUtils::Now();
    }
  }

  long GetConnectTime() const {
    return connected_time_ - start_time_;
  }

  long GetWritingTime() const {
    return write_end_time_ - write_start_time_;
  }

  long GetReadingTime() const {
    return read_end_time_ - read_start_time_;
  }

  long GetDealingTime() const {
    return read_start_time_ - write_end_time_;
  }

  long GetWholeTime() const {
    return end_time_ - start_time_;
  }

  long GetIoTime() const {
//    printf("  {start=%ld,connect=%ld,wstart=%ld,wend=%ld,rstart=%ld,rend=%ld,end=%ld,msg=%p}\n",
//        start_time_, connected_time_, write_start_time_, write_end_time_, read_start_time_,
//        read_end_time_, end_time_, error_msg_);

    return GetReadingTime() + GetWritingTime();
  }

  bool IsRight() const {
    return error_msg_ == nullptr && connected_time_ != -1 && write_start_time_ != -1 && write_end_time_ != -1 &&
        read_start_time_ != -1 && read_end_time_ != -1 && end_time_ != -1;
  }

  bool IsReportedWriteStartTime() {
    return write_start_time_ != -1;
  }

  bool IsReportedReadStartTime() {
    return read_start_time_ != -1;
  }

  void Reset() {
    read_bytes_ = 0;
    write_bytes_ = 0;
    error_msg_ = nullptr;
    start_time_ = TimeUtils::Now();
    connected_time_ = -1;
    write_start_time_ = -1;
    write_end_time_ = -1;
    read_start_time_ = -1;
    read_end_time_ = -1;
    end_time_ = -1;
  }
 private:
  std::size_t read_bytes_{0};
  std::size_t write_bytes_{0};
  time_t start_time_{TimeUtils::Now()};
  time_t connected_time_{-1};
  time_t write_start_time_{-1};
  time_t write_end_time_{-1};
  time_t read_start_time_{-1};
  time_t read_end_time_{-1};
  time_t end_time_{-1};
  const char *error_msg_{nullptr};
};

#endif // BISHE_CLIENT_REPORT_H
