#ifndef BISHE_OPT_CONVERT_H
#define BISHE_OPT_CONVERT_H

#include "optfwd.h"

class OptConvert {
 public:
  static const char *ConvertVersion(HttpVersion version);
  static const char *ConvertVersionString(HttpVersion version);
  static const char *ConvertMethod(HttpMethod method);
  static const char *ConvertProtocol(Protocol protocol);
  static const char *ConvertService(Protocol protocol);
};

#endif // BISHE_OPT_CONVERT_H
