// Copyright 2021 Emily Dale

#include <stdint.h>

#include <memory>
#include <iostream>

#include "Catch2/single_include/catch2/catch.hpp"

#include "src/poker_enigine/GameState.h"
 //   GameState(char num_players, char num_rounds, int32_t small_blind,
  //int32_t big_blind, int32_t starting_amounts, char small_blind_pos);
  // char small_blind_pos_;  // index of person who has small blind
  // int32_t* chip_amounts_;  // each player's remaining chips
  // bool* in_game_;  // false if player has folded, true otherwise
  // int32_t* total_bets_;  // amount of chips this player has in pot for this hand
  // char acting_player_;  // current acting player
  // char current_round_;  // 0 = preflop, 1 = flop, 2 = turn, 3 = river
  // int32_t min_raise_;  // min raise for the hand
  // int32_t max_bet_;  // max amount that anyone has put in the pot this hand
  // int32_t pot_;  // amount in pot
  // bool is_done_;  // false if game is over, true otherwise
  // char betting_round_;  // number of times betting has gone around this round
  // char num_players_;  // total number of players
  // char num_rounds_;  // total rounds for the game
  // int32_t starting_amounts_;  // starting chip amounts in terms of big blind
  // char num_left_;  // num of players not folded (all-in players are not folded)
  // char num_all_in_;  // number of players that are all in
  // bool needs_card_;  // game needs to have a card dealt
  // char pot_good_;  // number of players that still need to check/bet for this
  //                  // round. Folded and all in players are automatically good.
  //                  // the round increments when pot_good_ = 0
  // int32_t small_blind_;
  // int32_t big_blind_;
  // int32_t number_of_raises_;
// test cases:
TEST_CASE("3 player game sb = 1 ", "[poker_engine][GameState.cc]") {
  GameState state { 3, 4, 1, 2, 1, 12 , 1};
  SECTION("Verify all starting values") {
    REQUIRE(state.chip_amounts_[1] == 11);
    REQUIRE(state.total_bets_[1] == 1);
    REQUIRE(state.chip_amounts_[2] == 10);
    REQUIRE(state.total_bets_[2] == 2);
    REQUIRE(state.chip_amounts_[0] == 12);
    REQUIRE(state.total_bets_[0] == 0);
    REQUIRE(state.acting_player_ == 0);
    REQUIRE(state.min_raise_ == 2);
    REQUIRE(state.max_bet_ == 1);
    REQUIRE(state.pot_ == 3);
  }
  state.TakeAction(2);
  state.TakeAction(1);
  SECTION("Verify bb can raise after check/call from all others") {
    REQUIRE(state.acting_player_ == 2);
    REQUIRE(state.current_round_ == 0);
    REQUIRE(state.min_raise_ == 2);
    REQUIRE(state.max_bet_ == 1);
    REQUIRE(state.number_of_raises_ == 0);
    REQUIRE(state.pot_good_== 1);
    REQUIRE(state.pot_ == 6);
    REQUIRE(state.chip_amounts_[1] == 10);
    REQUIRE(state.total_bets_[1] == 2);
    REQUIRE(state.chip_amounts_[2] == 10);
    REQUIRE(state.total_bets_[2] == 2);
    REQUIRE(state.chip_amounts_[0] == 10);
    REQUIRE(state.total_bets_[0] == 2);
  }
  state.TakeAction(4);
  SECTION("Verify bb raising stays in current round") {
    REQUIRE(state.acting_player_ == 0);
    REQUIRE(state.current_round_ == 0);
    REQUIRE(state.min_raise_ == 4);
    REQUIRE(state.max_bet_ == 6);
    REQUIRE(state.number_of_raises_ == 1);
    REQUIRE(state.pot_good_== 2);
    REQUIRE(state.pot_ == 10);
  }
  state.TakeAction(-1);
  SECTION("Verify all relevant variables update after folding") {
    REQUIRE(state.acting_player_ == 1);
    REQUIRE(state.current_round_ == 0);
    REQUIRE(state.min_raise_ == 4);
    REQUIRE(state.max_bet_ == 6);
    REQUIRE(state.number_of_raises_ == 1);
    REQUIRE(state.pot_good_== 1);
    REQUIRE(state.num_left_ == 2);
    REQUIRE(state.pot_ == 10);
  }
  state.TakeAction(4);
  SECTION("Verify that call updates next round correctly") {
    REQUIRE(state.acting_player_ == 1);
    REQUIRE(state.current_round_ == 1);
    REQUIRE(state.min_raise_ == 2);
    REQUIRE(state.max_bet_ == 6);
    REQUIRE(state.number_of_raises_ == 0);
    REQUIRE(state.pot_good_== 2);
    REQUIRE(state.num_left_ == 2);
    REQUIRE(state.pot_ == 14);
  }
  state.TakeAction(4);
  state.TakeAction(6)
  SECTION("Verify correct for all in less than min raise") {
    REQUIRE(state.acting_player_ == 1);
    REQUIRE(state.current_round_ == 1);
    REQUIRE(state.min_raise_ == 4);
    REQUIRE(state.max_bet_ == 12);
    REQUIRE(state.number_of_raises_ == 2);
    REQUIRE(state.pot_good_== 1);
    REQUIRE(state.num_left_ == 1);
    REQUIRE(state.pot_ == 24);
  }

}