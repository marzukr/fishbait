// Copyright 2021 Marzuk Rashid

#ifndef SRC_HAND_STRENGTHS_CARDCOMBINATIONS_H_
#define SRC_HAND_STRENGTHS_CARDCOMBINATIONS_H_

#include <cstdint>
#include <vector>
#include <iostream>

#include "utils/VectorView.h"

namespace hand_strengths {

const uint8_t kDeckSize = 52;

class CardCombinations {
 public:
  explicit CardCombinations(uint8_t r);

  template <typename T>
  CardCombinations(uint8_t r, T&& exclude)
      : state_(r+1, kDeckSize), included_(kDeckSize, true), r_(r),
        is_done_(false) {
    Reset(exclude, true);
  }

  template <typename T>
  void Reset(T&& exclude, bool constructor = false) {
    if (!constructor) {
      std::fill(included_.begin(), included_.end(), true);
      std::fill(state_.begin(), state_.end(), kDeckSize);
      is_done_ = false;
    }

    for (auto it = exclude.begin(); it != exclude.end(); ++it) {
      included_[*it] = false;
    }

    state_[0] = -1;
    bool moved = MoveToNextIncluded(0);
    assert(moved);
    for (uint8_t i = 1; i < r_; ++i) {
      state_[i] = state_[i-1];
      moved = MoveToNextIncluded(i);
      assert(moved);
    }
  }

  static uint32_t N_Choose_K(uint32_t n, uint32_t k);

  uint8_t operator()(uint8_t idx) const { return state_[idx]; }

  CardCombinations& operator++() {
    IncrementState(r_-1);
    return *this;
  }

  bool is_done() const { return is_done_; }
  uint8_t r() const { return r_; }
  const std::vector<uint8_t>& state() const { return state_; }

 private:
  void IncrementState(uint8_t i);
  bool MoveToNextIncluded(uint8_t i);

  std::vector<uint8_t> state_;
  std::vector<bool> included_;
  const uint8_t r_;
  bool is_done_;
};  // class CardCombinations

}  // namespace hand_strengths

#endif  // SRC_HAND_STRENGTHS_CARDCOMBINATIONS_H_
