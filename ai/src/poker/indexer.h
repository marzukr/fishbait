#ifndef AI_SRC_POKER_INDEXER_H_
#define AI_SRC_POKER_INDEXER_H_

#include <array>
#include <cstdint>
#include <initializer_list>
#include <numeric>

extern "C" {
  #include "hand-isomorphism/src/hand_index.h"
}
#include "poker/definitions.h"
#include "utils/meta.h"

namespace fishbait {

template <CardN... kCardsPerRound>
class Indexer {
 private:
  /*
    @brief Get the total number of cards up to the given round.
  */
  static constexpr CardN TotalCards(std::size_t round = Rounds() - 1) {
    return PackSum<kCardsPerRound...>(round);
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
  ~Indexer() {
    hand_indexer_free(isocalc_);
    delete isocalc_;
  }

  Indexer(const Indexer&) = delete;
  Indexer& operator=(const Indexer&) = delete;
  Indexer(Indexer&&) = delete;
  Indexer& operator=(Indexer&&) = delete;

  /*
    @brief Returns the index of the given cards at the last round.
  */
  template <std::size_t kArrSize>
  hand_index_t IndexLast(const std::array<ISO_Card, kArrSize>& cards) const {
    static_assert(kArrSize >= TotalCards());
    return hand_index_last(isocalc_, cards.data());
  }

  /*
    @brief Returns the index of the given cards at the last round.
  */
  template <std::size_t kArrSize>
  hand_index_t IndexLast(const ISO_Card(&cards)[kArrSize]) const {
    static_assert(kArrSize >= TotalCards());
    return hand_index_last(isocalc_, cards);
  }

  /*
    @brief Returns the index of the given cards at each round.
  */
  template <std::size_t kArrSize>
  IndexArray Index(const ISO_Card(&cards)[kArrSize]) const {
    static_assert(kArrSize >= TotalCards());
    IndexArray indicies;
    hand_index_all(isocalc_, cards, indicies.data());
    return indicies;
  }

  /*
    @brief Returns the indices of the cards at each round.
  */
  template <std::size_t kArrSize>
  IndexArray Index(const std::array<ISO_Card, kArrSize>& cards) const {
    static_assert(kArrSize >= TotalCards());
    IndexArray indicies;
    hand_index_all(isocalc_, cards.data(), indicies.data());
    return indicies;
  }

  /*
    @brief Returns the cards of the given index.
  */
  template <std::size_t kRound>
  CardArray<kRound> Unindex(hand_index_t index) const {
    CardArray<kRound> cards;
    hand_unindex(isocalc_, kRound, index, cards.data());
    return cards;
  }
};  // class Indexer

}  // namespace fishbait

#endif  // AI_SRC_POKER_INDEXER_H_
