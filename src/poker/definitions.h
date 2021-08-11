// Copyright 2021 Marzuk Rashid

#ifndef SRC_POKER_DEFINITIONS_H_
#define SRC_POKER_DEFINITIONS_H_

#include <array>
#include <cstdint>

extern "C" {
  #include "hand-isomorphism/src/hand_index.h"
}
#include "utils/math.h"

namespace fishbait {

using RoundId = uint8_t;
using RoundN = RoundId;
enum class Round : RoundId { kPreFlop = 0, kFlop = 1, kTurn = 2, kRiver = 3 };
constexpr RoundN kNRounds = 4;
constexpr RoundId operator+(const Round round) {
  return static_cast<RoundId>(round);
}

using PlayCount = uint8_t;
using PlayerId = uint8_t;
using PlayerN = PlayerId;
using Chips = uint32_t;

enum class Action : uint8_t { kFold, kCheckCall, kBet, kAllIn };

using Card = uint8_t;
using CardN = Card;
using ISO_Card = Card;
using SK_Card = Card;

constexpr CardN kDeckSize = 52;
template <typename IndexScheme>
using Deck = std::array<IndexScheme, kDeckSize>;

constexpr std::array<CardN, kNRounds> kCardsPerRound = {2, 3, 1, 1};
constexpr CardN kHandCards = kCardsPerRound[+Round::kPreFlop];
constexpr CardN kBoardCards = kCardsPerRound[+Round::kFlop] +
                              kCardsPerRound[+Round::kTurn] +
                              kCardsPerRound[+Round::kRiver];
constexpr CardN kPlayerCards = kHandCards + kBoardCards;

template <typename IndexScheme>
using Hand = std::array<IndexScheme, kHandCards>;
template <typename IndexScheme, PlayerN kPlayers>
using HandArray = std::array<Hand<IndexScheme>, kPlayers>;
template <typename IndexScheme>
using BoardArray = std::array<IndexScheme, kBoardCards>;
template <typename IndexScheme, int kRuns>
using MultiBoardArray = std::array<BoardArray<IndexScheme>, kRuns>;
template <typename IndexScheme>
using PublicHand = std::array<IndexScheme, kPlayerCards>;

constexpr uint32_t kOpHandsN = N_Choose_K(kDeckSize - kHandCards - kBoardCards,
                                          kHandCards);  // 45 choose 2

constexpr std::array<hand_index_t, kNRounds> kImperfectRecallHands = {169,
    1286792, 13960050, 123156254};
inline constexpr hand_index_t ImperfectRecallHands(Round r) {
  return kImperfectRecallHands[+r];
}
constexpr hand_index_t kUniqueHands =
    ImperfectRecallHands(Round::kPreFlop);

}  // namespace fishbait

#endif  // SRC_POKER_DEFINITIONS_H_
