#ifndef BISHE_WRITE_BUFFER_ADAPTER_H
#define BISHE_WRITE_BUFFER_ADAPTER_H

#include "net.h"

class WriteBufferAdapter {
 public:
  void SetLength(size_t length) {
    length_ = length;
  }

  void SubstractLength(size_t diff) {
    if (length_ >= diff) {
      length_ -= diff;
    } else {
      length_ = 0;
    }
  }

  size_t GetContentLength() const {
    return length_;
  }

  n::mutable_buffer MutableBuffer() {
    return buf_.prepare(length_);
  }

  n::mutable_buffer MutableBuffer(size_t size) {
    return buf_.prepare(size);
  }

  template<typename DataType>
  const DataType *data() {
    return reinterpret_cast<const DataType *>(buf_.data().data());
  }

  size_t Size() const {
    return buf_.data().size();
  }

  size_t Commit(size_t size) {
    buf_.commit(size);
    return size;
  }

  size_t Consume(size_t size) {
    buf_.consume(size);
    return size;
  }
 private:
  n::streambuf buf_;
  size_t length_;
};

#endif // BISHE_WRITE_BUFFER_ADAPTER_H
