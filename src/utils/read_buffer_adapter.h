#ifndef BISHE_READ_BUFFER_ADAPTER_H
#define BISHE_READ_BUFFER_ADAPTER_H

#include <memory>
#include "net.h"

class ReadBufferAdapter {
 public:
  explicit ReadBufferAdapter(size_t size) : ptr_(MakeSharedCharArray(size)), size_(size) {}
  ReadBufferAdapter(std::shared_ptr<char> buf, size_t size) : ptr_(std::move(buf)), size_(size) {}
  ReadBufferAdapter(ReadBufferAdapter &&rhs) noexcept
      : ptr_(std::move(rhs.ptr_)), size_(rhs.size_), cursor_(rhs.cursor_) {}

  bool IsFinish() {
    return cursor_ == size_;
  }

  void IncreaseCursor(size_t diff) {
    if (cursor_ + diff <= size_) {
      cursor_ += diff;
    } else {
      cursor_ = size_;
    }
  }

  void ResetCursor() {
    cursor_ = 0;
  }

  size_t GetCursor() const {
    return cursor_;
  }

  size_t GetSize() const {
    return size_;
  }

  n::const_buffer ConstBuffer() const {
    return {ptr_.get() + cursor_, size_};
  }

  template<typename T = char>
  T *CursorPointer() {
    return ptr_.get() + cursor_;
  }

  std::shared_ptr<char> GetBuffer() const {
    return ptr_;
  }
 private:
  std::shared_ptr<char> ptr_;
  size_t size_;
  size_t cursor_{0};
};

#endif // BISHE_READ_BUFFER_ADAPTER_H
