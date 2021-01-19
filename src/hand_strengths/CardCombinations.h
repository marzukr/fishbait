// Copyright 2021 Marzuk Rashid

#ifndef SRC_HAND_STRENGTHS_CARDCOMBINATIONS_H_
#define SRC_HAND_STRENGTHS_CARDCOMBINATIONS_H_

#include <cstdint>
#include <vector>

#include "utils/VectorView.h"

namespace hand_strengths {

const uint8_t kDeckSize = 52;

class CardCombinations {
 public:
  explicit CardCombinations(uint8_t r);
  CardCombinations(uint8_t r, utils::VectorView<uint8_t> exclude);

  void Reset(utils::VectorView<uint8_t> exclude, bool constructor = false);

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
