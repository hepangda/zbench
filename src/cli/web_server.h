#ifndef BISHE_WEB_SERVER_H
#define BISHE_WEB_SERVER_H


#include <crow.h>

class WebServer {
 public:
  WebServer() = default;
  void Start();

 private:
  void NewBenchmark();                  // POST (browser)
  void SendBenchResult();               // GET  (browser)
  void ReceiveBenchResult();            // POST (bench core)
  void SendBenchmarkRequest();          // GET  (bench core)

  std::deque<std::unique_ptr<std::string>>     requests_;
  std::map<long, std::unique_ptr<std::string>> responses_;
  crow::SimpleApp app_;
};

#endif // BISHE_WEB_SERVER_H
