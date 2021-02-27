// Copyright 2021 Marzuk Rashid

#ifndef SRC_HAND_STRENGTHS_CARD_UTILS_H_
#define SRC_HAND_STRENGTHS_CARD_UTILS_H_

#include <cstdint>
#include <string>

#include "SKPokerEval/src/Deckcards.h"

namespace hand_strengths {

const uint8_t kDeckSize = 52;

inline std::string SKCardStr(uint8_t i) {
  return pretty_card[i];
}

inline uint8_t ConvertSKtoISO(uint8_t sk_card) {
  uint8_t res = 51 - sk_card;
  uint8_t rem = sk_card % 4;
  return res + (rem - 3 + rem);
}

inline uint8_t ConvertISOtoSK(uint8_t iso_card) {
  return ConvertSKtoISO(iso_card);
}

uint8_t ISOCardFromStr(const std::string& card_str);

}  // namespace hand_strengths

#endif  // SRC_HAND_STRENGTHS_CARD_UTILS_H_
