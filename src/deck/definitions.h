// Copyright 2021 Marzuk Rashid

#ifndef SRC_DECK_DEFINITIONS_H_
#define SRC_DECK_DEFINITIONS_H_

#include <cstdint>

namespace deck {

using Card = uint8_t;
using CardN = Card;

using ISO_Card = Card;
using SK_Card = Card;

constexpr CardN kDeckSize = 52;
constexpr uint8_t kUniqueHands = 169;
constexpr uint32_t kIsoRivers = 123156254;
constexpr uint32_t kOpHandsN = 990;  // 45 choose 2

}  // namespace deck

#endif  // SRC_DECK_DEFINITIONS_H_
