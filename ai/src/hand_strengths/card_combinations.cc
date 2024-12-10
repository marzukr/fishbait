#include "hand_strengths/card_combinations.h"

#include <algorithm>
#include <cassert>
#include <cstdint>

#include "poker/card_utils.h"
#include "poker/definitions.h"

namespace fishbait {

CardCombinations::CardCombinations(CardN r) : state_(r+1, 0),
                                              included_(kDeckSize, true), r_(r),
                                              is_done_(false) {
  for (CardN i = 0; i < r; ++i) {
    state_[i] = i;
  }
  state_[r] = kDeckSize;
}

void CardCombinations::IncrementState(CardN i) {
  if (MoveToNextIncluded(i)) {
    for (CardN j = i + 1; j < r_; ++j) {
      state_[j] = state_[j-1];
      [[maybe_unused]] bool moved = MoveToNextIncluded(j);
      assert(moved);
    }
    return;
  } else if (i > 0) {
    return IncrementState(i-1);
  } else {
    is_done_ = true;
    return;
  }
}

bool CardCombinations::MoveToNextIncluded(CardN i) {
  CardN move_size = 0;
  bool can_move = true;
  bool should_move = true;
  do {
    ++move_size;
    Card potential_location = state_[i] + move_size;
    can_move = potential_location < state_[i + 1];
    should_move = can_move && included_[potential_location];
  } while (can_move && !should_move);

  if (can_move && should_move) {
    state_[i] += move_size;
    return true;
  } else {
    return false;
  }
}

}  // namespace fishbait
