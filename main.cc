#include <iostream>
#include "src/cli/command_option.h"
#include "src/cli/web_server.h"
#include "src/core/cli_starter.h"

int main(int argc, char *argv[]) {
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
//  HttpRequest request("GET", "/", "HTTP/1.1");
//  request.SetHeaderItem("Connection", "close");
//  ni::tcp::endpoint ep{ ni::make_address_v4("123.125.114.144"), 443 };
//  Benchmarker bm(request, ep);
//  bm.Start();


//int main() {
//  n::io_context ioc;
//  ns::context ctx{ ns::context::method::sslv23_client};
//  ns::stream<ni::tcp::socket> ssock(ioc, ctx);
//
//  ni::tcp::resolver resolver(ioc);
//  resolver.async_resolve({"www.qq.com", "443"}, [&] (const std::error_code &ec, auto it) {
//    n::async_connect(ssock.lowest_layer(), it, [&] (const std::error_code &ec, auto it) {
//      if (!ec) {
//        ssock.async_handshake(ns::stream_base::handshake_type::client, [&] (const std::error_code &ec) {
//          if (!ec) {
//            n::async_write(ssock, n::buffer("GET / HTTP/1.0\r\n\r\n"), [&] (auto ec, auto bytes) {
//              if (!ec) {
//                char buf[4096];
//                ssock.async_read_some(n::buffer(buf), [&] (auto ec, auto bytes) {
//                  printf("buffer: \n====\n%s\n====\n", buf);
//                  char nbuf[4096];
//                  ssock.async_read_some(n::buffer(nbuf), [&] (std::error_code ec, auto bytes) {
//                    if (!ec) {
//                      puts("not ec");
//                    } else {
//                      std::cout << ec.message() << std::endl;
//                      std::cout << ec.value() << std::endl;
////                      perror("read3");
//                    }
//                  });
//                });
//              } else {
//                perror("Write");
//              }
//            });
//          } else {
//            perror("handshake");
//          }
//        });
//      } else {
//        perror("connect");
//      }
//    });
//
//  });

//  asio::write(ssock, buffer(request));
//
//  std::string response;
//
//  do {
//    char buf[1024];
//    size_t bytes_transferred = ssock.read_some(buffer(buf), ec);
//    if (!ec) response.append(buf, buf + bytes_transferred);
//  } while (!ec);
//
//  std::cout << response;

//  ioc.run();
//}

//int main() {
//  std::error_code ec;
//  using namespace asio;
//
//  // what we need
//  io_service svc;
//  ssl::context ctx{ ssl::context::method::sslv23_client};
//  ssl::stream<ip::tcp::socket> ssock(svc, ctx);
//
//  ip::tcp::resolver resolver(svc);
//  auto it = resolver.resolve({"www.qq.com", "443"}); // http://localhost:8087 for test
//
//  asio::connect(ssock.lowest_layer(), it);
////  ssock.lowest_layer().connect({ ip::make_address_v4("www.baidu.com"), 443 }); // http://localhost:8087 for test
//  ssock.handshake(ssl::stream_base::handshake_type::client);
//
//  // send request
//  std::string request("GET / HTTP/1.1\r\n\r\n");
//
////  asio::ip::tcp::socket *socket = &ssock;
//  asio::write(ssock, buffer(request));
//
//  // read response
//  std::string response;
//
//  do {
//    char buf[1024];
//    size_t bytes_transferred = ssock.read_some(buffer(buf), ec);
//    if (!ec) response.append(buf, buf + bytes_transferred);
//  } while (!ec);
//
//  // print and exit
//  std::cout << response;
//}