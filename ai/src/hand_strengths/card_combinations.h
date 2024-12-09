#ifndef AI_SRC_HAND_STRENGTHS_CARD_COMBINATIONS_H_
#define AI_SRC_HAND_STRENGTHS_CARD_COMBINATIONS_H_

#include <cassert>
#include <cstdint>
#include <vector>

#include "poker/card_utils.h"
#include "poker/definitions.h"

namespace fishbait {

class CardCombinations {
 public:
  explicit CardCombinations(CardN r);

  template <typename T>
  CardCombinations(CardN r, T&& exclude) : state_(r+1, kDeckSize),
                                           included_(kDeckSize, true), r_{r},
                                           is_done_{false} {
    Reset(exclude, true);
  }

  CardCombinations(const CardCombinations& other) = default;
  CardCombinations& operator=(const CardCombinations& other) = default;

  /* Reset the card combination generator and exclude the given cards.
     Constructor should only be true if this function is being called from the
     constructor. */
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
    [[maybe_unused]] bool moved = MoveToNextIncluded(0);
    assert(moved);
    for (Card i = 1; i < r_; ++i) {
      state_[i] = state_[i-1];
      moved = MoveToNextIncluded(i);
      assert(moved);
    }
  }

  Card operator()(CardN idx) const { return state_[idx]; }
  Card* begin() { return state_.data(); }
  Card* end() { return state_.data() + r_; }

  CardCombinations& operator++() {
    IncrementState(r_-1);
    return *this;
  }

  bool is_done() const { return is_done_; }
  CardN r() const { return r_; }
  const std::vector<Card>& state() const { return state_; }

 private:
  void IncrementState(CardN i);
  bool MoveToNextIncluded(CardN i);

  std::vector<Card> state_;
  std::vector<bool> included_;
  CardN r_;
  bool is_done_;
};  // class CardCombinations

}  // namespace fishbait

#endif  // AI_SRC_HAND_STRENGTHS_CARD_COMBINATIONS_H_
