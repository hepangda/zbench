#include "opt_convert.h"

const char* OptConvert::ConvertMethod(HttpMethod method) {
  const char *convert_map[] = {
    "GET", "POST", "PUT", "DELETE"
  };

  return convert_map[method];
}

const char* OptConvert::ConvertVersion(HttpVersion version) {
  const char *convert_map[] = {
    "1.0", "1.1"
  };

  return convert_map[version];
}

const char* OptConvert::ConvertProtocol(Protocol protocol) {
  const char *convert_map[] = {
      "HTTP", "HTTPS"
  };

  return convert_map[protocol];
}

const char* OptConvert::ConvertService(Protocol protocol) {
  const char *convert_map[] = {
      "http", "https"
  };

  return convert_map[protocol];
}

const char *OptConvert::ConvertVersionString(HttpVersion version) {
  const char *convert_map[] = {
      "HTTP/1.0", "HTTP/1.1"
  };

  return convert_map[version];
}
