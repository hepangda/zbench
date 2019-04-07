#ifndef BISHE_NET_H
#define BISHE_NET_H

#include <asio.hpp>
#include <asio/ssl.hpp>

namespace n = asio;
namespace ni = asio::ip;
namespace ns = asio::ssl;

inline std::shared_ptr<char> MakeSharedCharArray(size_t size) {
  return std::move(std::shared_ptr<char>{ new char[size], std::default_delete<char[]>()});
}

#endif // BISHE_NET_H
