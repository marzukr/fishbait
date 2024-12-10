#ifndef AI_SRC_POKER_CARD_UTILS_H_
#define AI_SRC_POKER_CARD_UTILS_H_

#include <array>
#include <cstdint>
#include <numeric>
#include <string>

#include "SKPokerEval/src/Deckcards.h"
#include "poker/definitions.h"

namespace fishbait {

inline std::string SKCardPretty(Card i) {
  return pretty_card[i];
}

inline ISO_Card ConvertSKtoISO(SK_Card sk_card) {
  uint8_t res = 51 - sk_card;
  uint8_t rem = sk_card % 4;
  return res + (rem - 3 + rem);
}

inline SK_Card ConvertISOtoSK(ISO_Card iso_card) {
  return ConvertSKtoISO(iso_card);
}

ISO_Card ISOCardFromStr(const std::string& card_str);

SK_Card SKCardFromStr(const std::string& card_str);

template <typename IndexScheme>
Deck<IndexScheme> UnshuffledDeck() {
  Deck<IndexScheme> card_deck;
  std::iota(card_deck.begin(), card_deck.end(), 0);
  return card_deck;
}

}  // namespace fishbait

#endif  // AI_SRC_POKER_CARD_UTILS_H_
