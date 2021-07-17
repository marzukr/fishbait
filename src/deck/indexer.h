// Copyright 2021 Marzuk Rashid

#ifndef SRC_DECK_INDEXER_H_
#define SRC_DECK_INDEXER_H_

#include <array>
#include <cstdint>
#include <initializer_list>
#include <numeric>

extern "C" {
  #include "hand-isomorphism/src/hand_index.h"
}
#include "deck/definitions.h"
#include "poker_engine/definitions.h"

namespace deck {

class Indexer {
 public:
  /*
    @brief Constructs an Indexer object.

    @param rounds The number of rounds in the game to index.
    @param cpr An array of the number of cards in each round.
  */
  Indexer(const poker_engine::RoundN rounds,
          const std::initializer_list<CardN> cpr) {
    isocalc_ = new hand_indexer_t();
    hand_indexer_init(rounds, cpr.begin(), isocalc_);
  }
  ~Indexer() { delete isocalc_; }

  Indexer(const Indexer&) = delete;
  Indexer& operator=(const Indexer&) = delete;
  Indexer(Indexer&&) = delete;
  Indexer& operator=(Indexer&&) = delete;

  /*
    @brief Returns the index of the given cards.
  */
  hand_index_t index(const std::initializer_list<ISO_Card> cards) {
    return hand_index_last(isocalc_, cards.begin());
  }

  /*
    @brief Returns the index of the given cards.
  */
  template <std::size_t kN>
  hand_index_t index(const std::array<ISO_Card, kN>& cards) {
    return hand_index_last(isocalc_, cards.data());
  }

  /*
    @brief Writes the indices of the cards at each round to the given array.
  */
  template <std::size_t kN, std::size_t kR>
  hand_index_t index(const std::array<ISO_Card, kN>& cards,
                     std::array<hand_index_t, kR>* indicies) {
    return hand_index_all(isocalc_, cards.data(), indicies->data());
  }

  /*
    @brief Writes the cards of the given index to the given array.
  */
  template <std::size_t kN>
  void unindex(poker_engine::RoundN round, hand_index_t index,
               std::array<ISO_Card, kN>* cards) {
    hand_unindex(isocalc_, round, index, cards->data());
  }

 private:
  hand_indexer_t* isocalc_;
};  // class Indexer

}  // namespace deck

#endif  // SRC_DECK_INDEXER_H_
