// Copyright 2021 Marzuk Rashid

#include "catch2/catch.hpp"
#include "deck/card_utils.h"
#include "poker_engine/node.h"

TEST_CASE("Triton cash game first 3 hands", "[poker_engine][node]") {
  /* 
    Link: https://www.youtube.com/watch?v=FrUdWj8B-nk

    Each player started with 500k chips. 2000 small blind, 4000 big blind, and
    4000 big blind ante. The player's started off in the following order and
    positions:
      0. Loeliger (button)
      1. Cates (small blind)
      2. Kuznetsov (big blind)
      3. Tan Xuan
      4. Paul Phua
      5. Tony G
      6. Haxton
      7. Badziakouski
  */

  // Construct the initial Node
  auto Card = deck::SKCardFromStr;
  uint8_t cards0[8][2] = {{/*     Loeliger     */}, {/*      Cates       */},
                          {Card("Qh"), Card("Js")}, {Card("Td"), Card("5s")},
                          {Card("Kc"), Card("8h")}, {Card("Jc"), Card("7h")},
                          {Card("Kh"), Card("Jh")}, {Card("7s"), Card("2d")}};
  uint8_t board0[5] = {Card("8c"), Card("6c"), Card("Ks"), Card("5c"),
                       Card("4d")};
  poker_engine::Node<8> triton(0, 4000, 2000, 500, true, true, 500000, cards0,
                               board0);

  // Verify that the constructor worked correctly
  REQUIRE(triton.big_blind() == 4000);
  REQUIRE(triton.small_blind() == 2000);
  REQUIRE(triton.ante() == 500);
  REQUIRE(triton.big_blind_ante() == true);
  REQUIRE(triton.blind_before_ante() == true);
  REQUIRE(triton.button() == 0);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 0);
  REQUIRE(triton.acting_player() == 3);
  REQUIRE(triton.folded(3) == false);
  REQUIRE(triton.players_left() == 8);
  REQUIRE(triton.pot() == 10000);
  REQUIRE(triton.bets(3) == 500);
  REQUIRE(triton.stack(3) == 500000);

  /* ------------------------------- Preflop -------------------------------- */

  // Test Tan Xuan's action (fold)
  REQUIRE(triton.PlayerIndex(3) == 3);
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply({poker_engine::Action::kFold, 0}) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 1);
  REQUIRE(triton.acting_player() == 4);
  REQUIRE(triton.folded(3) == true);
  REQUIRE(triton.players_left() == 7);
  REQUIRE(triton.pot() == 10000);
  REQUIRE(triton.bets(3) == 500);
  REQUIRE(triton.stack(3) == 500000);

  // Test Paul Phua's action (fold)
  REQUIRE(triton.PlayerIndex(4) == 4);
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply({poker_engine::Action::kFold, 0}) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 2);
  REQUIRE(triton.acting_player() == 5);
  REQUIRE(triton.folded(4) == true);
  REQUIRE(triton.players_left() == 6);
  REQUIRE(triton.pot() == 10000);
  REQUIRE(triton.bets(4) == 500);
  REQUIRE(triton.stack(4) == 500000);

  // Test Tony G's action (raise to 10000)
  REQUIRE(triton.PlayerIndex(5) == 5);
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.CanBet(10000) == true);
  REQUIRE(triton.Apply({poker_engine::Action::kBet, 10000}) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 3);
  REQUIRE(triton.acting_player() == 6);
  REQUIRE(triton.folded(5) == false);
  REQUIRE(triton.players_left() == 6);
  REQUIRE(triton.pot() == 20000);
  REQUIRE(triton.bets(5) == 10500);
  REQUIRE(triton.stack(5) == 490000);

  // Test Haxton's action (call)
  REQUIRE(triton.PlayerIndex(6) == 6);
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply({poker_engine::Action::kCheckCall, 0}) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 4);
  REQUIRE(triton.acting_player() == 7);
  REQUIRE(triton.folded(6) == false);
  REQUIRE(triton.players_left() == 6);
  REQUIRE(triton.pot() == 30000);
  REQUIRE(triton.bets(6) == 10500);
  REQUIRE(triton.stack(6) == 490000);

  // Test Badziakouski's action (fold)
  REQUIRE(triton.PlayerIndex(7) == 7);
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply({poker_engine::Action::kFold, 0}) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 5);
  REQUIRE(triton.acting_player() == 0);
  REQUIRE(triton.folded(7) == true);
  REQUIRE(triton.players_left() == 5);
  REQUIRE(triton.pot() == 30000);
  REQUIRE(triton.bets(7) == 500);
  REQUIRE(triton.stack(7) == 500000);

  // Test Loeliger's action (fold)
  REQUIRE(triton.PlayerIndex(0) == 0);
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply({poker_engine::Action::kFold, 0}) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 6);
  REQUIRE(triton.acting_player() == 1);
  REQUIRE(triton.folded(0) == true);
  REQUIRE(triton.players_left() == 4);
  REQUIRE(triton.pot() == 30000);
  REQUIRE(triton.bets(0) == 500);
  REQUIRE(triton.stack(0) == 500000);

  // Test Cates's action (fold)
  REQUIRE(triton.PlayerIndex(1) == 1);
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply({poker_engine::Action::kFold, 0}) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 7);
  REQUIRE(triton.acting_player() == 2);
  REQUIRE(triton.folded(1) == true);
  REQUIRE(triton.players_left() == 3);
  REQUIRE(triton.pot() == 30000);
  REQUIRE(triton.bets(1) == 2500);
  REQUIRE(triton.stack(1) == 498000);

  // Test Kuznetsov's action (fold)
  REQUIRE(triton.PlayerIndex(2) == 2);
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply({poker_engine::Action::kCheckCall, 0}) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kFlop);
  REQUIRE(triton.cycled() == 1);
  REQUIRE(triton.acting_player() == 2);
  REQUIRE(triton.folded(2) == false);
  REQUIRE(triton.players_left() == 3);
  REQUIRE(triton.pot() == 36000);
  REQUIRE(triton.bets(2) == 10500);
  REQUIRE(triton.stack(2) == 486000);

  /* -------------------------------- Flop ---------------------------------- */

  // Test Kuznetsov's action (check)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply({poker_engine::Action::kCheckCall, 0}) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kFlop);
  REQUIRE(triton.cycled() == 4);
  REQUIRE(triton.acting_player() == 5);
  REQUIRE(triton.folded(2) == false);
  REQUIRE(triton.players_left() == 3);
  REQUIRE(triton.pot() == 36000);
  REQUIRE(triton.bets(2) == 10500);
  REQUIRE(triton.stack(2) == 486000);

  // Test Tony G's action (bet 30000)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.CanBet(30000) == true);
  REQUIRE(triton.Apply({poker_engine::Action::kBet, 30000}) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kFlop);
  REQUIRE(triton.cycled() == 5);
  REQUIRE(triton.acting_player() == 6);
  REQUIRE(triton.folded(5) == false);
  REQUIRE(triton.players_left() == 3);
  REQUIRE(triton.pot() == 66000);
  REQUIRE(triton.bets(5) == 40500);
  REQUIRE(triton.stack(5) == 460000);

  // Test Haxton's action (call)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply({poker_engine::Action::kCheckCall, 0}) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kFlop);
  REQUIRE(triton.cycled() == 9);
  REQUIRE(triton.acting_player() == 2);
  REQUIRE(triton.folded(6) == false);
  REQUIRE(triton.players_left() == 3);
  REQUIRE(triton.pot() == 96000);
  REQUIRE(triton.bets(6) == 40500);
  REQUIRE(triton.stack(6) == 460000);

  // Test Kuznetsov's action (fold)
  REQUIRE(triton.Rotation() == 1);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply({poker_engine::Action::kFold, 0}) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kTurn);
  REQUIRE(triton.cycled() == 4);
  REQUIRE(triton.acting_player() == 5);
  REQUIRE(triton.folded(2) == true);
  REQUIRE(triton.players_left() == 2);
  REQUIRE(triton.pot() == 96000);
  REQUIRE(triton.bets(2) == 10500);
  REQUIRE(triton.stack(2) == 486000);

  /* -------------------------------- Turn ---------------------------------- */

  // Test Tony G's action (bet 100000)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.CanBet(100000));
  REQUIRE(triton.Apply({poker_engine::Action::kBet, 100000}) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kTurn);
  REQUIRE(triton.cycled() == 5);
  REQUIRE(triton.acting_player() == 6);
  REQUIRE(triton.folded(5) == false);
  REQUIRE(triton.players_left() == 2);
  REQUIRE(triton.pot() == 196000);
  REQUIRE(triton.bets(5) == 140500);
  REQUIRE(triton.stack(5) == 360000);

  // Test Haxton's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply({poker_engine::Action::kFold, 0}) == false);
  REQUIRE(triton.in_progress() == false);
  REQUIRE(triton.round() == poker_engine::Round::kTurn);
  REQUIRE(triton.cycled() == 12);
  REQUIRE(triton.acting_player() == 5);
  REQUIRE(triton.folded(6) == true);
  REQUIRE(triton.players_left() == 1);
  REQUIRE(triton.pot() == 0);
  REQUIRE(triton.bets(6) == 0);
  REQUIRE(triton.stack(6) == 460000);

  /* ------------------------------------------------------------------------ */
  /* ------------------------------- Hand 2 --------------------------------- */
  /* ------------------------------------------------------------------------ */
  /*
    0. Loeliger
    1. Cates (button)
    2. Kuznetsov (small blind)
    3. Tan Xuan (big blind)
    4. Paul Phua
    5. Tony G
    6. Haxton
    7. Badziakouski
  */

  uint8_t cards1[8][2] = {{Card("Qd"), Card("6c")}, {Card("Ah"), Card("8d")},
                          {/*     Kuznetsov    */}, {Card("8c"), Card("6h")},
                          {Card("Kc"), Card("4s")}, {Card("7c"), Card("6s")},
                          {Card("7s"), Card("3s")}, {Card("Tc"), Card("8h")}};
  uint8_t board1[5] = {/*Flop1*/ /*Flop2*/ /*Flop3*/ /*Turn*/ /*River*/};
  triton.NewHand(cards1, board1);

  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 0);
  REQUIRE(triton.acting_player() == 4);
  REQUIRE(triton.players_left() == 8);
  REQUIRE(triton.pot() == 10000);

  // Test Paul Phua's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply({poker_engine::Action::kFold, 0}) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 1);
  REQUIRE(triton.acting_player() == 5);
  REQUIRE(triton.folded(4) == true);
  REQUIRE(triton.players_left() == 7);
  REQUIRE(triton.pot() == 10000);
  REQUIRE(triton.bets(4) == 500);
  REQUIRE(triton.stack(4) == 500000);

  // Test Tony G's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply({poker_engine::Action::kFold, 0}) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 2);
  REQUIRE(triton.acting_player() == 6);
  REQUIRE(triton.folded(5) == true);
  REQUIRE(triton.players_left() == 6);
  REQUIRE(triton.pot() == 10000);
  REQUIRE(triton.bets(5) == 500);
  REQUIRE(triton.stack(5) == 556000);

  // Test Haxton's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply({poker_engine::Action::kFold, 0}) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 3);
  REQUIRE(triton.acting_player() == 7);
  REQUIRE(triton.folded(6) == true);
  REQUIRE(triton.players_left() == 5);
  REQUIRE(triton.pot() == 10000);
  REQUIRE(triton.bets(6) == 500);
  REQUIRE(triton.stack(6) == 460000);

  // Test Badziakouski's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply({poker_engine::Action::kFold, 0}) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 4);
  REQUIRE(triton.acting_player() == 0);
  REQUIRE(triton.folded(7) == true);
  REQUIRE(triton.players_left() == 4);
  REQUIRE(triton.pot() == 10000);
  REQUIRE(triton.bets(7) == 500);
  REQUIRE(triton.stack(7) == 500000);

  // Test Loeliger's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply({poker_engine::Action::kFold, 0}) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 5);
  REQUIRE(triton.acting_player() == 1);
  REQUIRE(triton.folded(0) == true);
  REQUIRE(triton.players_left() == 3);
  REQUIRE(triton.pot() == 10000);
  REQUIRE(triton.bets(0) == 500);
  REQUIRE(triton.stack(0) == 500000);

  // Test Cates's action (raise to 12000)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.CanBet(12000) == true);
  REQUIRE(triton.Apply({poker_engine::Action::kBet, 12000}) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 6);
  REQUIRE(triton.acting_player() == 2);
  REQUIRE(triton.folded(1) == false);
  REQUIRE(triton.players_left() == 3);
  REQUIRE(triton.pot() == 22000);
  REQUIRE(triton.bets(1) == 12500);
  REQUIRE(triton.stack(1) == 486000);

  // Test Kuznetsov's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply({poker_engine::Action::kFold, 0}) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 7);
  REQUIRE(triton.acting_player() == 3);
  REQUIRE(triton.folded(2) == true);
  REQUIRE(triton.players_left() == 2);
  REQUIRE(triton.pot() == 22000);
  REQUIRE(triton.bets(2) == 2500);
  REQUIRE(triton.stack(2) == 484000);

  // Test Tan Xuan's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply({poker_engine::Action::kFold, 0}) == false);
  REQUIRE(triton.in_progress() == false);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 13);
  REQUIRE(triton.acting_player() == 1);
  REQUIRE(triton.folded(3) == true);
  REQUIRE(triton.players_left() == 1);
  REQUIRE(triton.pot() == 0);
  REQUIRE(triton.bets(3) == 0);
  REQUIRE(triton.stack(3) == 492000);

  /* ------------------------------------------------------------------------ */
  /* ------------------------------- Hand 3 --------------------------------- */
  /* ------------------------------------------------------------------------ */
  /*
    0. Loeliger
    1. Cates
    2. Kuznetsov (button)
    3. Tan Xuan (small blind)
    4. Paul Phua (big blind)
    5. Tony G
    6. Haxton
    7. Badziakouski
  */

  uint8_t cards2[8][2] = {{Card("Kc"), Card("Th")}, {Card("Ah"), Card("9d")},
                          {Card("As"), Card("5d")}, {Card("4h"), Card("2d")},
                          {Card("Qd"), Card("8h")}, {Card("Tc"), Card("4s")},
                          {Card("5s"), Card("4d")}, {Card("9c"), Card("3d")}};
  uint8_t board2[5] = {Card("Kd"), Card("3s"), Card("7d") /*Turn*/ /*River*/};
  triton.NewHand(cards2, board2);

  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 0);
  REQUIRE(triton.acting_player() == 5);
  REQUIRE(triton.players_left() == 8);
  REQUIRE(triton.pot() == 10000);

  // Test Tony G's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply({poker_engine::Action::kFold, 0}) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 1);
  REQUIRE(triton.acting_player() == 6);
  REQUIRE(triton.folded(5) == true);
  REQUIRE(triton.players_left() == 7);
  REQUIRE(triton.pot() == 10000);
  REQUIRE(triton.bets(5) == 500);
  REQUIRE(triton.stack(5) == 556000);

  // Test Haxton's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply({poker_engine::Action::kFold, 0}) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 2);
  REQUIRE(triton.acting_player() == 7);
  REQUIRE(triton.folded(6) == true);
  REQUIRE(triton.players_left() == 6);
  REQUIRE(triton.pot() == 10000);
  REQUIRE(triton.bets(6) == 500);
  REQUIRE(triton.stack(6) == 460000);

  // Test Badziakouski's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply({poker_engine::Action::kFold, 0}) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 3);
  REQUIRE(triton.acting_player() == 0);
  REQUIRE(triton.folded(7) == true);
  REQUIRE(triton.players_left() == 5);
  REQUIRE(triton.pot() == 10000);
  REQUIRE(triton.bets(7) == 500);
  REQUIRE(triton.stack(7) == 500000);

  // Test Loeliger's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply({poker_engine::Action::kFold, 0}) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 4);
  REQUIRE(triton.acting_player() == 1);
  REQUIRE(triton.folded(0) == true);
  REQUIRE(triton.players_left() == 4);
  REQUIRE(triton.pot() == 10000);
  REQUIRE(triton.bets(0) == 500);
  REQUIRE(triton.stack(0) == 500000);

  // Test Cates's action (raise to 12000)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.CanBet(120000) == true);
  REQUIRE(triton.Apply({poker_engine::Action::kBet, 12000}) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 5);
  REQUIRE(triton.acting_player() == 2);
  REQUIRE(triton.folded(1) == false);
  REQUIRE(triton.players_left() == 4);
  REQUIRE(triton.pot() == 22000);
  REQUIRE(triton.bets(1) == 12500);
  REQUIRE(triton.stack(1) == 496000);

  // Test Kuznetsov's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply({poker_engine::Action::kFold, 0}) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 6);
  REQUIRE(triton.acting_player() == 3);
  REQUIRE(triton.folded(2) == true);
  REQUIRE(triton.players_left() == 3);
  REQUIRE(triton.pot() == 22000);
  REQUIRE(triton.bets(2) == 500);
  REQUIRE(triton.stack(2) == 484000);

  // Test Tan Xuan's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply({poker_engine::Action::kFold, 0}) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 7);
  REQUIRE(triton.acting_player() == 4);
  REQUIRE(triton.folded(3) == true);
  REQUIRE(triton.players_left() == 2);
  REQUIRE(triton.pot() == 22000);
  REQUIRE(triton.bets(3) == 2500);
  REQUIRE(triton.stack(3) == 490000);

  // Test Paul Phua's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply({poker_engine::Action::kCheckCall, 0}) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kFlop);
  REQUIRE(triton.cycled() == 1);
  REQUIRE(triton.acting_player() == 4);
  REQUIRE(triton.folded(4) == false);
  REQUIRE(triton.players_left() == 2);
  REQUIRE(triton.pot() == 30000);
  REQUIRE(triton.bets(4) == 12500);
  REQUIRE(triton.stack(4) == 484000);

  /* -------------------------------- Flop ---------------------------------- */

  // Test Paul Phua's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply({poker_engine::Action::kCheckCall, 0}) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kFlop);
  REQUIRE(triton.cycled() == 6);
  REQUIRE(triton.acting_player() == 1);
  REQUIRE(triton.folded(4) == false);
  REQUIRE(triton.players_left() == 2);
  REQUIRE(triton.pot() == 30000);
  REQUIRE(triton.bets(4) == 12500);
  REQUIRE(triton.stack(4) == 484000);

  // Test Cates's action (bet 10000)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.CanBet(10000) == true);
  REQUIRE(triton.Apply({poker_engine::Action::kBet, 10000}) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kFlop);
  REQUIRE(triton.cycled() == 9);
  REQUIRE(triton.acting_player() == 4);
  REQUIRE(triton.folded(1) == false);
  REQUIRE(triton.players_left() == 2);
  REQUIRE(triton.pot() == 40000);
  REQUIRE(triton.bets(1) == 22500);
  REQUIRE(triton.stack(1) == 486000);

  // Test Paul Phua's action (fold)
  REQUIRE(triton.Rotation() == 1);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply({poker_engine::Action::kFold, 0}) == false);
  REQUIRE(triton.in_progress() == false);
  REQUIRE(triton.round() == poker_engine::Round::kFlop);
  REQUIRE(triton.cycled() == 14);
  REQUIRE(triton.acting_player() == 1);
  REQUIRE(triton.folded(4) == true);
  REQUIRE(triton.players_left() == 1);
  REQUIRE(triton.pot() == 0);
  REQUIRE(triton.bets(4) == 0);
  REQUIRE(triton.stack(4) == 484000);
}  // TEST_CASE "Triton cash game first 3 hands"

/*
  things to test:
    + Node()
    + NewHand()
    + PlayerIndex()
    + Rotation()
    + CanCheckCall()
    + CanBet()
    + Apply()
    + big_blind()
    + small_blind()
    + ante()
    + big_blind_ante()
    + blind_before_ante()
    + button()
    + in_progress()
    + round()
    + cycled()
    + acting_player()
    + folded()
    + players_left()
    + pot()
    + bets()
    + stack()
*/
