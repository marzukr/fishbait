// Copyright 2021 Marzuk Rashid

#include "hand_strengths/card_utils.h"

#include <string>

extern "C" {
  #include "hand-isomorphism/src/deck.h"
}

namespace hand_strengths {

uint8_t ISOCardFromStr(const std::string& card_str) {
  uint32_t suit = 0;
  if (card_str[1] == 'h') {
    suit = 1;
  } else if (card_str[1] == 'd') {
    suit = 2;
  } else if (card_str[1] == 'c') {
    suit = 3;
  }

  uint32_t rank = 0;
  if (card_str[0] == '3') {
    rank = 1;
  } else if (card_str[0] == '4') {
    rank = 2;
  } else if (card_str[0] == '5') {
    rank = 3;
  } else if (card_str[0] == '6') {
    rank = 4;
  } else if (card_str[0] == '7') {
    rank = 5;
  } else if (card_str[0] == '8') {
    rank = 6;
  } else if (card_str[0] == '9') {
    rank = 7;
  } else if (card_str[0] == 'T') {
    rank = 8;
  } else if (card_str[0] == 'J') {
    rank = 9;
  } else if (card_str[0] == 'Q') {
    rank = 10;
  } else if (card_str[0] == 'K') {
    rank = 11;
  } else if (card_str[0] == 'A') {
    rank = 12;
  }

  return deck_make_card(suit, rank);
}  // ISOCardFromStr()

}  // namespace hand_strengths
