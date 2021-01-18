// Copyright 2021 Marzuk Rashid

#ifndef SRC_HAND_STRENGTHS_CARDCOMBINATIONS_H_
#define SRC_HAND_STRENGTHS_CARDCOMBINATIONS_H_

#include <cstdint>
#include <vector>

#include "utils/VectorView.h"

namespace hand_strengths {

const uint8_t kDeckSize = 5;

class CardCombinations {
 public:
  explicit CardCombinations(uint32_t r);
  CardCombinations(uint32_t r, utils::VectorView<uint32_t> exclude);

  static uint32_t N_Choose_K(uint32_t n, uint32_t k);

  uint32_t operator()(uint32_t idx) const { return state_[idx]; }

  CardCombinations& operator++() {
    IncrementState(r_-1);
    return *this;
  }

  bool is_done() const { return is_done_; }
  uint32_t r() const { return r_; }

 private:
  void IncrementState(uint32_t i);
  bool MoveToNextIncluded(uint32_t i);

  std::vector<uint32_t> state_;
  std::vector<bool> included_;
  uint32_t r_;
  bool is_done_;
};  // class CardCombinations

}  // namespace hand_strengths

#endif  // SRC_HAND_STRENGTHS_CARDCOMBINATIONS_H_
