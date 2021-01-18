// Copyright 2021 Marzuk Rashid

#include "hand_strengths/CardCombinations.h"

#include <cstdint>
#include <cassert>

#include "utils/VectorView.h"

namespace hand_strengths {

CardCombinations::CardCombinations(uint32_t r)
    : state_(r+1, 0), included_(kDeckSize, true), r_(r), is_done_(false) {
  for (uint32_t i = 0; i < r; ++i) {
    state_[i] = i;
  }
  state_[r] = kDeckSize;
}

CardCombinations::CardCombinations(uint32_t r,
                                   utils::VectorView<uint32_t> exclude)
    : state_(r+1, kDeckSize), included_(kDeckSize, true), r_(r),
      is_done_(false) {
  for (auto it = exclude.begin(); it != exclude.end(); ++it) {
    included_[*it] = false;
  }

  state_[0] = -1;
  assert(MoveToNextIncluded(0) == true);
  for (uint32_t i = 1; i < r; ++i) {
    state_[i] = state_[i-1];
    assert(MoveToNextIncluded(i) == true);
  }
}

uint32_t CardCombinations::N_Choose_K(uint32_t n, uint32_t k) {
  if (k > n) return 0;
  if (k * 2 > n) k = n-k;
  if (k == 0) return 1;

  int32_t result = n;
  for (int32_t i = 2; i <= k; ++i) {
    result *= (n-i+1);
    result /= i;
  }
  return result;
}

void CardCombinations::IncrementState(uint32_t i) {
  if (MoveToNextIncluded(i)) {
    for (uint32_t j = i + 1; j < r_; ++j) {
      state_[j] = state_[j-1];
      assert(MoveToNextIncluded(j) == true);
    }
    return;
  } else if (i > 0) {
    return IncrementState(i-1);
  } else {
    is_done_ = true;
    return;
  }
}

bool CardCombinations::MoveToNextIncluded(uint32_t i) {
  uint32_t move_size = 0;
  bool can_move = true;
  bool should_move = true;
  do {
    ++move_size;
    can_move = state_[i] + move_size < state_[i + 1];
    should_move = can_move && included_[state_[i] + move_size];
  } while (can_move && !should_move);

  if (can_move && should_move) {
    state_[i] += move_size;
    return true;
  } else {
    return false;
  }
}

}  // namespace hand_strengths
