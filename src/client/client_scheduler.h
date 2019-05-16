#ifndef BISHE_CLIENT_SCHEDULER_H
#define BISHE_CLIENT_SCHEDULER_H

#include <list>
#include <queue>
#include "client_report.h"
#include "../utils/http_request.h"
#include "https_session.h"
#include "http_session.h"

class ClientScheduler {
 public:
  ClientScheduler(n::io_context &io_ctx, ni::tcp::endpoint endpoint, HttpRequest request)
      : io_ctx_(io_ctx), endpoint_(std::move(endpoint)), request_(std::move(request)) {
    request_buffer_ = std::make_shared<ReadBufferAdapter>(request_.Dump());
  }

  void Report(ClientReport report) {
//    printf("Report<%s>[Read: %lubytes, Write: %lubytes, IO: %lums, Connect: %lums, Dealing: %lums, Whole: %lums]\n",
//        report.IsRight() ? "Valid" : "INVALID",
//        report.GetReadBytes(), report.GetWriteBytes(), report.GetIoTime(),
//        report.GetConnectTime(), report.GetDealingTime(), report.GetWholeTime());
    if (report.GetErrorMessage() != nullptr) {
      printf("Error: %s\n", report.GetErrorMessage());
    }
    list_.emplace_back(report);
    if (!io_ctx_.stopped()) {
      Spawn();
    }
  }

  void Spawn() {
    std::make_shared<HttpSession>(*this)->Start();
  }

  n::io_context &GetIoContext() {
    return io_ctx_;
  }

  const ni::tcp::endpoint &GetEndpoint() const {
    return endpoint_;
  }

  std::shared_ptr<char> GetRequest() const {
    return request_buffer_->GetBuffer();
  }

  std::size_t GetRequestSize() const {
    return request_buffer_->GetSize();
  }

  void PrintResult(int seconds) {
    std::int64_t total_read_bytes = 0;
    std::int64_t total_write_bytes = 0;
    std::int64_t errors_request = 0;
    std::vector<long> connect_time_q;
    std::vector<long> deal_time_q;
    std::vector<long> io_time_q;
    std::vector<long> total_time_q;

    for (const ClientReport &i : list_) {
      total_read_bytes += i.GetReadBytes();
      total_write_bytes += i.GetWriteBytes();
      if (i.IsRight()) {
        connect_time_q.emplace_back(i.GetConnectTime());
        deal_time_q.emplace_back(i.GetDealingTime());
        io_time_q.emplace_back(i.GetIoTime());
        total_time_q.emplace_back(i.GetWholeTime());
      } else {
        errors_request++;
      }
    }

    double bytes_per_second = double(total_read_bytes + total_write_bytes) / seconds;
    double request_per_second = double(list_.size()) / seconds;

    printf("Total %zu request, %ld errors.\n", list_.size(), errors_request);
    printf("%.3lf Byte(s) per second, total %ld bytes.\n", bytes_per_second, total_read_bytes + total_write_bytes);
    printf("%.3lf Request(s) per second, total %zu requests.\n", request_per_second, list_.size());
    puts("");

    std::sort(connect_time_q.begin(), connect_time_q.end());
    std::sort(deal_time_q.begin(), deal_time_q.end());
    std::sort(io_time_q.begin(), io_time_q.end());
    std::sort(total_time_q.begin(), total_time_q.end());

    int middle = io_time_q.size() / 2;
    printf("%-17s %-7s %-7s %-7s\n", "", "Min", "Middle", "Max");
    printf("%-17s %-7ld %-7ld %-7ld\n", "Connect Time(ms)", connect_time_q.front(),
        connect_time_q[middle], connect_time_q.back());
    printf("%-17s %-7ld %-7ld %-7ld\n", "Dealing Time(ms)", deal_time_q.front(),
        deal_time_q[middle], deal_time_q.back());
    printf("%-17s %-7ld %-7ld %-7ld\n", "I/O Time(ms)", io_time_q.front(), io_time_q[middle], io_time_q.back());
    printf("%-17s %-7ld %-7ld %-7ld\n", "Total Time(ms)", total_time_q.front(),
        total_time_q[middle], total_time_q.back());
    puts("");
pthread_t;
    int percent_50 = (total_time_q.size() - 1) * 0.5;
    int percent_66 = (total_time_q.size() - 1) * 0.66;
    int percent_75 = (total_time_q.size() - 1) * 0.75;
    int percent_80 = (total_time_q.size() - 1) * 0.8;
    int percent_90 = (total_time_q.size() - 1) * 0.9;
    int percent_95 = (total_time_q.size() - 1) * 0.95;
    int percent_98 = (total_time_q.size() - 1) * 0.98;
    int percent_99 = (total_time_q.size() - 1) * 0.99;
    int percent_100 = (total_time_q.size() - 1) * 1;

    printf("%-11s Time(ms)\n", "Percentage");
    printf("%-11s %ld\n", "50%", total_time_q[percent_50]);
    printf("%-11s %ld\n", "66%", total_time_q[percent_66]);
    printf("%-11s %ld\n", "75%", total_time_q[percent_75]);
    printf("%-11s %ld\n", "80%", total_time_q[percent_80]);
    printf("%-11s %ld\n", "90%", total_time_q[percent_90]);
    printf("%-11s %ld\n", "95%", total_time_q[percent_95]);
    printf("%-11s %ld\n", "98%", total_time_q[percent_98]);
    printf("%-11s %ld\n", "99%", total_time_q[percent_99]);
    printf("%-11s %ld (slowest request)\n", "100%", total_time_q[percent_100]);
    puts("");
  }
 private:
  n::io_context &io_ctx_;
  ni::tcp::endpoint endpoint_;
  HttpRequest request_;
  std::shared_ptr<ReadBufferAdapter> request_buffer_;
  std::list<ClientReport> list_;
};


#endif // BISHE_CLIENT_SCHEDULER_H
