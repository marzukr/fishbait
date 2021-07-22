// Copyright 2021 Marzuk Rashid

#ifndef SRC_ENGINE_INDEXER_H_
#define SRC_ENGINE_INDEXER_H_

#include <array>
#include <cstdint>
#include <initializer_list>
#include <numeric>

extern "C" {
  #include "hand-isomorphism/src/hand_index.h"
}
#include "engine/definitions.h"
#include "utils/meta.h"

namespace engine {

template <CardN... kCardsPerRound>
class Indexer {
 private:
  /*
    @brief Get the total number of cards up to the given round.
  */
  static constexpr CardN TotalCards(std::size_t round = Rounds() - 1) {
    return utils::PackSum<kCardsPerRound...>(round);
  }

  static constexpr std::size_t Rounds() {
    return sizeof...(kCardsPerRound);
  }

  template <std::size_t kRound = Rounds() - 1>
  using CardArray = std::array<ISO_Card, TotalCards(kRound)>;
  using IndexArray = std::array<hand_index_t, Rounds()>;

  hand_indexer_t* isocalc_;

 public:
  Indexer() {
    isocalc_ = new hand_indexer_t();
    std::array<CardN, Rounds()> cpr_array = {kCardsPerRound...};
    hand_indexer_init(Rounds(), cpr_array.data(), isocalc_);
  }
  ~Indexer() { delete isocalc_; }

  Indexer(const Indexer&) = delete;
  Indexer& operator=(const Indexer&) = delete;
  Indexer(Indexer&&) = delete;
  Indexer& operator=(Indexer&&) = delete;

  /*
    @brief Returns the index of the given cards at the last round.
  */
  hand_index_t IndexLast(const CardArray<>& cards) {
    return hand_index_last(isocalc_, cards.data());
  }

  /*
    @brief Returns the index of the given cards at the last round.
  */
  hand_index_t IndexLast(const ISO_Card(&cards)[TotalCards(Rounds() - 1)]) {
    return hand_index_last(isocalc_, cards);
  }

  /*
    @brief Returns the index of the given cards at each round.
  */
  IndexArray Index(const ISO_Card(&cards)[TotalCards(Rounds() - 1)]) {
    IndexArray indicies;
    hand_index_all(isocalc_, cards, indicies.data());
    return indicies;
  }

  /*
    @brief Returns the indices of the cards at each round.
  */
  IndexArray Index(const CardArray<>& cards) {
    IndexArray indicies;
    hand_index_all(isocalc_, cards.data(), indicies.data());
    return indicies;
  }

  /*
    @brief Returns the cards of the given index.
  */
  template <std::size_t kRound>
  CardArray<kRound> Unindex(hand_index_t index) {
    CardArray<kRound> cards;
    hand_unindex(isocalc_, kRound, index, cards.data());
    return cards;
  }
};  // class Indexer

}  // namespace engine

#endif  // SRC_ENGINE_INDEXER_H_
