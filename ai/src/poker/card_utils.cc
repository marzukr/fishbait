#include "poker/card_utils.h"

#include <stdexcept>
#include <string>

extern "C" {
  #include "hand-isomorphism/src/deck.h"
}
#include "poker/definitions.h"

namespace fishbait {

void CheckCardLength(const std::string& func, const std::string& card_str) {
  if (card_str.length() != 2) {
    std::string err = func + " called with invalid string of length " +
                      std::to_string(card_str.length()) + ". Only strings of "
                      "length 2 can be valid cards.";
    throw std::invalid_argument(err);
  }
}

void InvalidSuit(const std::string& func, const char suit) {
  std::string err = func + " called with invalid suit '" + suit + "'. Only "
                    "'s', 'h', 'd', and 'c' are valid suits.";
  throw std::invalid_argument(err);
}

void InvalidRank(const std::string& func, const char rank) {
  std::string err = func + " called with invalid rank '" + rank + "'. Only "
                    "'A', 'K', 'Q', 'J', 'T', '9', '8', '7', '6', '5', '4', "
                    "'3', and '2' are valid ranks.";
  throw std::invalid_argument(err);
}

ISO_Card ISOCardFromStr(const std::string& card_str) {
  CheckCardLength(__func__, card_str);

  card_t suit = 0;
  if (card_str[1] == 's') {
    suit = 0;
  } else if (card_str[1] == 'h') {
    suit = 1;
  } else if (card_str[1] == 'd') {
    suit = 2;
  } else if (card_str[1] == 'c') {
    suit = 3;
  } else {
    InvalidSuit(__func__, card_str[1]);
  }

  card_t rank = 0;
  if (card_str[0] == '2') {
    rank = 0;
  } else if (card_str[0] == '3') {
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
  } else {
    InvalidRank(__func__, card_str[0]);
  }

  return deck_make_card(suit, rank);
}  // ISOCardFromStr()

SK_Card SKCardFromStr(const std::string& card_str) {
  CheckCardLength(__func__, card_str);

  CardN suit = 0;
  if (card_str[1] == 's') {
    suit = 0;
  } else if (card_str[1] == 'h') {
    suit = 1;
  } else if (card_str[1] == 'd') {
    suit = 2;
  } else if (card_str[1] == 'c') {
    suit = 3;
  } else {
    InvalidSuit(__func__, card_str[1]);
  }

  CardN rank = 0;
  if (card_str[0] == 'A') {
    rank = 0;
  } else if (card_str[0] == 'K') {
    rank = 4;
  } else if (card_str[0] == 'Q') {
    rank = 8;
  } else if (card_str[0] == 'J') {
    rank = 12;
  } else if (card_str[0] == 'T') {
    rank = 16;
  } else if (card_str[0] == '9') {
    rank = 20;
  } else if (card_str[0] == '8') {
    rank = 24;
  } else if (card_str[0] == '7') {
    rank = 28;
  } else if (card_str[0] == '6') {
    rank = 32;
  } else if (card_str[0] == '5') {
    rank = 36;
  } else if (card_str[0] == '4') {
    rank = 40;
  } else if (card_str[0] == '3') {
    rank = 44;
  } else if (card_str[0] == '2') {
    rank = 48;
  } else {
    InvalidRank(__func__, card_str[0]);
  }

  return suit + rank;
}  // SKCardFromStr()

}  // namespace fishbait
