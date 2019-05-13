#ifndef BISHE_CLI_BUFFER_H
#define BISHE_CLI_BUFFER_H

#include <deque>
#include <mutex>

constexpr int kMaxBufferSize = 1024;

template<typename Type>
class CliBuffer {
 public:
  CliBuffer() noexcept : queue_(kMaxBufferSize) {}

  std::pair<bool, Type> Front() {
    std::lock_guard<std::mutex> lk(mtx_);

    if (!IsEmpty()) {
      return {true, queue_[0]};
    } else {
      return {false, Type{}};
    }
  }

  std::pair<bool, Type> Back() {
    std::lock_guard<std::mutex> lk(mtx_);

    if (!IsEmpty()) {
      return {true, queue_[queue_.size() - 1]};
    } else {
      return {false, Type{}};
    }
  }

  bool PushData(const Type &data) {
    std::lock_guard<std::mutex> lk(mtx_);

    if (IsFull()) {
      throw std::runtime_error("Error: over the size of buffer");
    } else {
      queue_.push_back(data);
      return true;
    }
  }

  void PopData() {
    std::lock_guard<std::mutex> lk(mtx_);

    if (IsEmpty()) {
      return;
    } else {
      queue_.pop_front();
    }
  }

 private:
  bool IsEmpty() {
    std::lock_guard<std::mutex> lk(mtx_);
    return queue_.empty();
  }
  bool IsFull() {
    std::lock_guard<std::mutex> lk(mtx_);
    return queue_.size() == kMaxBufferSize;
  }

  std::mutex mtx_;
  std::deque<Type> queue_;
};

#endif // BISHE_CLI_BUFFER_H
