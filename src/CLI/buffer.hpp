/**
 * Created by Crow on 5/10/19.
 * Copyright (c) 2019 Crow All rights reserved.
 * @author Crow 
 */

#ifndef BISHE_CLI_REQUEST_BUFFER_H
#define BISHE_CLI_REQUEST_BUFFER_H

#include <deque>
#include <mutex>

constexpr int KMaxBufferSize = 1024;

template <typename Type>
class Buffer {
 public:
  Buffer() noexcept;
  auto Front() -> std::pair<bool, Type>;
  auto Back()  -> std::pair<bool, Type>;
  bool PushData(const Type &data);
  void PopData();

 private:
  bool IsEmpty();
  bool IsFull();

  std::mutex mtx_;
  std::deque<Type> queue_;
};

template <typename Type>
Buffer<Type>::Buffer() : queue_(KMaxBufferSize) {}

template <typename Type>
auto Buffer<Type>::Front() -> std::pair<bool, Type> {
  std::lock_guard<std::mutex> lk(mtx_);

  if (!IsEmpty()) {
    return {true, queue_[0]};
  } else {
    return {false, Type{}};
  }
}

template <typename Type>
auto Buffer<Type>::Back() -> std::pair<bool, Type> {
  std::lock_guard<std::mutex> lk(mtx_);

  if (!IsEmpty()) {
    return {true, queue_[queue_.size() - 1]};
  } else {
    return {false, Type{}};
  }
}

template <typename Type>
bool Buffer<Type>::PushData(const Type &data) {
  std::lock_guard<std::mutex> lk(mtx_);

  if (IsFull()) {
    throw std::runtime_error("Error: over the size of buffer");
  } else {
    queue_.push_back(data);
    return true;
  }
}

template <typename Type>
void Buffer<Type>::PopData() {
  std::lock_guard<std::mutex> lk(mtx_);

  if (IsEmpty()) {
    return ;
  } else {
    queue_.pop_front();
  }
}

template <typename Type>
bool Buffer<Type>::IsFull() {
  std::lock_guard<std::mutex> lk(mtx_);
  return queue_.size() == KMaxBufferSize;
}

template <typename Type>
bool Buffer<Type>::IsEmpty() {
  std::lock_guard<std::mutex> lk(mtx_);
  return queue_.empty();
}

#endif //BISHE_CLI_REQUEST_BUFFER_H
