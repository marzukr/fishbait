// Copyright 2021 Marzuk Rashid

#ifndef SRC_UTILS_LOOP_ITERATOR_H_
#define SRC_UTILS_LOOP_ITERATOR_H_

#include <iterator>

namespace utils {

template <typename T>
class LoopIterator {
 public:
  using iterator_category = std::forward_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = T;
  using pointer = T;
  using reference = T;

  explicit LoopIterator(T init) : value(init) {}

  // prefix increment
  LoopIterator<T>& operator++() {
    ++value;
    return *this;
  }
  // postfix increment
  LoopIterator<T> operator++(int) {
    LoopIterator<T> old = *this;
    operator++();
    return old;
  }
  // prefix decrement
  LoopIterator<T>& operator--() {
    value += -1;
    return *this;
  }
  // postfix decrement
  LoopIterator<T> operator--(int) {
    LoopIterator<T> old = *this;
    operator--();
    return old;
  }

  T operator[](std::size_t idx) { return value + idx; }
  T operator*() { return value; }
  T operator->() { return value; }

  bool operator==(const LoopIterator<T>& other) const {
    return value == other.value;
  }
  bool operator!=(const LoopIterator<T>& other) const {
    return !(*this == other);
  }

 private:
  T value;
};

}  // namespace utils

#endif  // SRC_UTILS_LOOP_ITERATOR_H_