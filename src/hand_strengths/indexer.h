// Copyright 2021 Marzuk Rashid

#ifndef SRC_HAND_STRENGTHS_INDEXER_H_
#define SRC_HAND_STRENGTHS_INDEXER_H_

#include <cstdint>
#include <numeric>
#include <initializer_list>
#include <array>

extern "C" {
  #include "hand-isomorphism/src/hand_index.h"
}

namespace hand_strengths {

class Indexer {
 public:
  Indexer(const uint8_t rounds, const std::initializer_list<uint8_t> cpr) {
    isocalc_ = new hand_indexer_t();
    hand_indexer_init(rounds, cpr.begin(), isocalc_);
  }
  ~Indexer() { delete isocalc_; }

  uint32_t operator()(const std::initializer_list<uint8_t> cards) {
    return hand_index_last(isocalc_, cards.begin());
  }

  template <std::size_t kN>
  uint32_t operator()(const std::array<uint8_t, kN>& cards) {
    return hand_index_last(isocalc_, cards.data());
  }

  template <std::size_t kN>
  uint32_t operator()(uint32_t round, uint32_t index,
                      std::array<uint8_t, kN>* cards) {
    return hand_unindex(isocalc_, round, index, cards->data());
  }

 private:
  hand_indexer_t* isocalc_;
};  // class Indexer

}  // namespace hand_strengths

#endif  // SRC_HAND_STRENGTHS_INDEXER_H_
