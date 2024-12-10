#ifndef AI_SRC_POKER_DEFINITIONS_H_
#define AI_SRC_POKER_DEFINITIONS_H_

#include <array>
#include <cstdint>
#include <ostream>
#include <string_view>

extern "C" {
  #include "hand-isomorphism/src/hand_index.h"
}
#include "utils/array.h"
#include "utils/math.h"

namespace fishbait {

using RoundId = uint8_t;
using RoundN = RoundId;
enum class Round : RoundId { kPreFlop = 0, kFlop = 1, kTurn = 2, kRiver = 3 };
constexpr RoundN kNRounds = 4;
constexpr RoundId operator+(const Round round) {
  return static_cast<RoundId>(round);
}
constexpr std::array<std::string_view, kNRounds> kRoundNames =
    {"Preflop", "Flop", "Turn", "River"};
inline std::ostream& operator<<(std::ostream& os, Round r) {
  os << kRoundNames[+r];
  return os;
}

using PlayCount = uint8_t;
using PlayerId = uint8_t;
using PlayerN = PlayerId;
using Chips = uint32_t;

enum class Action : uint8_t { kFold = 0, kCheckCall = 1, kBet = 2, kAllIn = 3 };
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

/* Snapshot of a Node object. Refer to the Node class for more information about
   the member variables */
template <PlayerId kPlayers>
struct NodeSnapshot {
  PlayerN players;
  Chips big_blind;
  Chips small_blind;
  Chips ante;
  bool big_blind_ante;
  bool blind_before_ante;
  double rake;
  Chips rake_cap;
  bool no_flop_no_drop;
  PlayerId button;
  bool in_progress;
  RoundId round;
  PlayerId acting_player;
  bool folded[kPlayers];
  PlayerN players_left;
  PlayerN players_all_in;
  Chips pot;
  Chips bets[kPlayers];
  Chips stack[kPlayers];
  Chips min_raise;
  Chips needed_to_call;
  ISO_Card hands[kPlayers][kHandCards];
  ISO_Card board[kBoardCards];
};

}  // namespace fishbait

#endif  // AI_SRC_POKER_DEFINITIONS_H_
