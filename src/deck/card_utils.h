// Copyright 2021 Marzuk Rashid

#ifndef SRC_DECK_CARD_UTILS_H_
#define SRC_DECK_CARD_UTILS_H_

#include <cstdint>
#include <string>

#include "SKPokerEval/src/Deckcards.h"
#include "deck/definitions.h"

namespace deck {

inline std::string SKCardPretty(Card i) {
  return pretty_card[i];
}

inline Card ConvertSKtoISO(Card sk_card) {
  uint8_t res = 51 - sk_card;
  uint8_t rem = sk_card % 4;
  return res + (rem - 3 + rem);
}

inline Card ConvertISOtoSK(Card iso_card) {
  return ConvertSKtoISO(iso_card);
}

Card ISOCardFromStr(const std::string& card_str);

Card SKCardFromStr(const std::string& card_str);

}  // namespace deck

#endif  // SRC_DECK_CARD_UTILS_H_
