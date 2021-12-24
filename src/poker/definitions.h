// Copyright 2021 Marzuk Rashid

#ifndef SRC_POKER_DEFINITIONS_H_
#define SRC_POKER_DEFINITIONS_H_

#include <array>
#include <cstdint>
#include <ostream>

extern "C" {
  #include "hand-isomorphism/src/hand_index.h"
}
#include "utils/array.h"
#include "utils/math.h"

namespace fishbait {

using RoundId = uint8_t;
using RoundN = RoundId;
enum class Round : RoundId { kPreFlop = 0, kFlop = 1, kTurn = 2, kRiver = 3 };
inline std::ostream& operator<<(std::ostream& os, Round r) {
  switch (r) {
    case Round::kPreFlop:
      os << "Preflop";
      break;
    case Round::kFlop:
      os << "Flop";
      break;
    case Round::kTurn:
      os << "Turn";
      break;
    case Round::kRiver:
      os << "River";
      break;
  }
  return os;
}
constexpr RoundN kNRounds = 4;
constexpr RoundId operator+(const Round round) {
  return static_cast<RoundId>(round);
}

using PlayCount = uint8_t;
using PlayerId = uint8_t;
using PlayerN = PlayerId;
using Chips = uint32_t;

enum class Action : uint8_t { kFold, kCheckCall, kBet, kAllIn };
inline std::ostream& operator<<(std::ostream& os, Action a) {
  switch (a) {
    case Action::kFold:
      os << "Fold";
      break;
    case Action::kCheckCall:
      os << "Check/Call";
      break;
    case Action::kBet:
      os << "Bet";
      break;
    case Action::kAllIn:
      os << "All In";
      break;
  }
  return os;
}

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

template <PlayerN N>
inline std::array<Chips, N> StackArray(Chips stack_size) {
  return FilledArray<Chips, N>(stack_size);
}

}  // namespace fishbait

#endif  // SRC_POKER_DEFINITIONS_H_
