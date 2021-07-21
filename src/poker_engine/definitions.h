// Copyright 2021 Marzuk Rashid

#ifndef SRC_POKER_ENGINE_DEFINITIONS_H_
#define SRC_POKER_ENGINE_DEFINITIONS_H_

#include <array>
#include <cstdint>

#include "deck/definitions.h"

namespace poker_engine {

using RoundId = uint8_t;
using RoundN = RoundId;
using PlayCount = uint8_t;
using PlayerId = uint8_t;
using PlayerN = PlayerId;
using Chips = uint32_t;

enum class Round : RoundId { kPreFlop, kFlop, kTurn, kRiver };
enum class Action : uint8_t { kFold, kCheckCall, kBet, kAllIn };

constexpr RoundN kNRounds = 4;

inline RoundId GetRoundId(Round round) {
  return static_cast<RoundId>(round);
}

inline Round GetRound(RoundId round_id) {
  return Round{round_id};
}

constexpr deck::CardN kHandCards = 2;
constexpr deck::CardN kBoardCards = 5;
constexpr deck::CardN kPlayerCards = kHandCards + kBoardCards;

template <typename IndexScheme>
using Hand = std::array<IndexScheme, kHandCards>;

template <typename IndexScheme, PlayerN kPlayers>
using HandArray = std::array<Hand<IndexScheme>, kPlayers>;

template <typename IndexScheme>
using BoardArray = std::array<IndexScheme, kBoardCards>;

template <typename IndexScheme, int kRuns>
using MultiBoardArray = std::array<BoardArray<IndexScheme>, kRuns>;

template <typename IndexScheme>
using PlayerCardArray = std::array<IndexScheme, kPlayerCards>;

}  // namespace poker_engine

#endif  // SRC_POKER_ENGINE_DEFINITIONS_H_
