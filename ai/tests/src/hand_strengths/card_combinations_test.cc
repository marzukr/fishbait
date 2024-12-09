#include <iostream>
#include <set>

#include "array/array.h"
#include "array/matrix.h"
#include "catch2/catch.hpp"
#include "hand_strengths/card_combinations.h"
#include "poker/definitions.h"
#include "utils/math.h"

TEST_CASE("card_combinations test", "[hand_strengths][card_combinations]") {
  uint32_t n;
  uint32_t k;
  uint32_t n_choose_k;
  bool reset;
  bool exclude;
  std::vector<fishbait::Card> to_exclude;

  SECTION("Basic 52 choose 2 use case without reset") {
    n = 52;
    k = 2;
    n_choose_k = 1326;
    reset = false;
    exclude = false;
  }
  SECTION("Basic 52 choose 2 use case with reset") {
    n = 52;
    k = 2;
    n_choose_k = 1326;
    reset = true;
    exclude = false;
  }
  SECTION("6 choose 3 use case with exclude") {
    n = 6;
    k = 3;
    n_choose_k = 20;
    reset = false;
    exclude = true;
    to_exclude = {1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 12, 13, 14, 15, 16, 17, 19,
                  20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 35,
                  36, 37, 38, 39, 40, 41, 42, 44, 45, 46, 47, 48, 49, 50};
  }
  SECTION("6 choose 3 use case with reset") {
    n = 6;
    k = 3;
    n_choose_k = 20;
    reset = true;
    exclude = false;
    to_exclude = {1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 12, 13, 14, 15, 16, 17, 19,
                  20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 35,
                  36, 37, 38, 39, 40, 41, 42, 44, 45, 46, 47, 48, 49, 50};
  }
  SECTION("50 choose 5 use case with reset and exclude") {
    n = 50;
    k = 3;
    n_choose_k = 19600;
    reset = true;
    exclude = true;
    to_exclude = {47, 51};
  }

  fishbait::CardCombinations test_deck(k);
  if (exclude) {
    test_deck = fishbait::CardCombinations(k, to_exclude);
  }

  uint32_t total = fishbait::N_Choose_K(n, k);
  REQUIRE(total == n_choose_k);

  std::set<nda::vector_ref<fishbait::Card>> unique_cards;
  nda::matrix<fishbait::Card> unique_card_data({n_choose_k, k});

  if (reset) {
    test_deck.Reset(to_exclude);
  }

  uint32_t i = 0;
  for (; !test_deck.is_done(); ++test_deck) {
    uint32_t j = 0;
    fishbait::Card last_card = 0;
    bool first_card = true;
    for (fishbait::Card* card = test_deck.begin(); card != test_deck.end();
         ++card) {
      REQUIRE(*card == test_deck(j));
      REQUIRE(*card >= 0);
      REQUIRE(*card < 52);
      REQUIRE((first_card || *card > last_card));
      first_card = false;
      last_card = *card;
      if (i < n_choose_k && j < k) {
        unique_card_data(i, j) = *card;
      }
      ++j;
    }  // for card
    REQUIRE(j == k);

    bool inserted = unique_cards.insert(unique_card_data(i, nda::all)).second;
    REQUIRE(inserted == true);

    ++i;
  }  // for test_deck
  REQUIRE(i == total);
  REQUIRE(unique_cards.size() == n_choose_k);
}  // TEST_CASE "Basic 52 choose 2 use case"
