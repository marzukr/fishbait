// Copyright 2021 Marzuk Rashid

#include "catch2/catch.hpp"
#include "deck/card_utils.h"
#include "poker_engine/node.h"
#include "utils/fraction.h"

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
  poker_engine::Node<8, double> triton(0, 4000, 2000, 500, true, true, 500000);
  REQUIRE_THROWS(triton.NewHand());

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
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 10000);
  REQUIRE(triton.bets(3) == 500);
  REQUIRE(triton.stack(3) == 500000);

  /* ------------------------------- Preflop -------------------------------- */

  // Test Tan Xuan's action (fold)
  REQUIRE(triton.PlayerIndex(3) == 3);
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply(poker_engine::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 1);
  REQUIRE(triton.acting_player() == 4);
  REQUIRE(triton.folded(3) == true);
  REQUIRE(triton.players_left() == 7);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 10000);
  REQUIRE(triton.bets(3) == 500);
  REQUIRE(triton.stack(3) == 500000);
  REQUIRE_THROWS(triton.NewHand());

  // Test Paul Phua's action (fold)
  REQUIRE(triton.PlayerIndex(4) == 4);
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply(poker_engine::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 2);
  REQUIRE(triton.acting_player() == 5);
  REQUIRE(triton.folded(4) == true);
  REQUIRE(triton.players_left() == 6);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 10000);
  REQUIRE(triton.bets(4) == 500);
  REQUIRE(triton.stack(4) == 500000);

  // Test Tony G's action (raise to 10000)
  REQUIRE(triton.PlayerIndex(5) == 5);
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.CanBet(10000) == true);
  REQUIRE(triton.Apply(poker_engine::Action::kBet, 10000) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 3);
  REQUIRE(triton.acting_player() == 6);
  REQUIRE(triton.folded(5) == false);
  REQUIRE(triton.players_left() == 6);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 20000);
  REQUIRE(triton.bets(5) == 10500);
  REQUIRE(triton.stack(5) == 490000);

  // Test Haxton's action (call)
  REQUIRE(triton.PlayerIndex(6) == 6);
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply(poker_engine::Action::kCheckCall) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 4);
  REQUIRE(triton.acting_player() == 7);
  REQUIRE(triton.folded(6) == false);
  REQUIRE(triton.players_left() == 6);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 30000);
  REQUIRE(triton.bets(6) == 10500);
  REQUIRE(triton.stack(6) == 490000);

  // Test Badziakouski's action (fold)
  REQUIRE(triton.PlayerIndex(7) == 7);
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply(poker_engine::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 5);
  REQUIRE(triton.acting_player() == 0);
  REQUIRE(triton.folded(7) == true);
  REQUIRE(triton.players_left() == 5);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 30000);
  REQUIRE(triton.bets(7) == 500);
  REQUIRE(triton.stack(7) == 500000);

  // Test Loeliger's action (fold)
  REQUIRE(triton.PlayerIndex(0) == 0);
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply(poker_engine::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 6);
  REQUIRE(triton.acting_player() == 1);
  REQUIRE(triton.folded(0) == true);
  REQUIRE(triton.players_left() == 4);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 30000);
  REQUIRE(triton.bets(0) == 500);
  REQUIRE(triton.stack(0) == 500000);

  // Test Cates's action (fold)
  REQUIRE(triton.PlayerIndex(1) == 1);
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply(poker_engine::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 7);
  REQUIRE(triton.acting_player() == 2);
  REQUIRE(triton.folded(1) == true);
  REQUIRE(triton.players_left() == 3);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 30000);
  REQUIRE(triton.bets(1) == 2500);
  REQUIRE(triton.stack(1) == 498000);

  // Test Kuznetsov's action (fold)
  REQUIRE(triton.PlayerIndex(2) == 2);
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply(poker_engine::Action::kCheckCall) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kFlop);
  REQUIRE(triton.cycled() == 1);
  REQUIRE(triton.acting_player() == 2);
  REQUIRE(triton.folded(2) == false);
  REQUIRE(triton.players_left() == 3);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 36000);
  REQUIRE(triton.bets(2) == 10500);
  REQUIRE(triton.stack(2) == 486000);

  /* -------------------------------- Flop ---------------------------------- */

  // Test Kuznetsov's action (check)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply(poker_engine::Action::kCheckCall) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kFlop);
  REQUIRE(triton.cycled() == 4);
  REQUIRE(triton.acting_player() == 5);
  REQUIRE(triton.folded(2) == false);
  REQUIRE(triton.players_left() == 3);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 36000);
  REQUIRE(triton.bets(2) == 10500);
  REQUIRE(triton.stack(2) == 486000);

  // Test Tony G's action (bet 30000)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.CanBet(30000) == true);
  REQUIRE(triton.Apply(poker_engine::Action::kBet, 30000) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kFlop);
  REQUIRE(triton.cycled() == 5);
  REQUIRE(triton.acting_player() == 6);
  REQUIRE(triton.folded(5) == false);
  REQUIRE(triton.players_left() == 3);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 66000);
  REQUIRE(triton.bets(5) == 40500);
  REQUIRE(triton.stack(5) == 460000);

  // Test Haxton's action (call)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply(poker_engine::Action::kCheckCall) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kFlop);
  REQUIRE(triton.cycled() == 9);
  REQUIRE(triton.acting_player() == 2);
  REQUIRE(triton.folded(6) == false);
  REQUIRE(triton.players_left() == 3);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 96000);
  REQUIRE(triton.bets(6) == 40500);
  REQUIRE(triton.stack(6) == 460000);

  // Test Kuznetsov's action (fold)
  REQUIRE(triton.Rotation() == 1);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply(poker_engine::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kTurn);
  REQUIRE(triton.cycled() == 4);
  REQUIRE(triton.acting_player() == 5);
  REQUIRE(triton.folded(2) == true);
  REQUIRE(triton.players_left() == 2);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 96000);
  REQUIRE(triton.bets(2) == 10500);
  REQUIRE(triton.stack(2) == 486000);

  /* -------------------------------- Turn ---------------------------------- */

  // Test Tony G's action (bet 100000)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.CanBet(100000));
  REQUIRE(triton.Apply(poker_engine::Action::kBet, 100000) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kTurn);
  REQUIRE(triton.cycled() == 5);
  REQUIRE(triton.acting_player() == 6);
  REQUIRE(triton.folded(5) == false);
  REQUIRE(triton.players_left() == 2);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 196000);
  REQUIRE(triton.bets(5) == 140500);
  REQUIRE(triton.stack(5) == 360000);

  uint8_t cards_throw[8][2];
  uint8_t board_throw[1][5];
  REQUIRE_THROWS(triton.AwardPot(triton.same_stack_no_rake_, cards_throw,
                                 board_throw[0]));
  REQUIRE_THROWS(triton.AwardPot(triton.single_run_, cards_throw,
                                 board_throw[0]));
  REQUIRE_THROWS(triton.AwardPot(triton.multi_run_, cards_throw, board_throw,
                                 1));

  // Test Haxton's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply(poker_engine::Action::kFold) == false);
  REQUIRE(triton.in_progress() == false);
  REQUIRE(triton.round() == poker_engine::Round::kTurn);
  REQUIRE(triton.cycled() == 12);
  REQUIRE(triton.acting_player() == 5);
  REQUIRE(triton.folded(6) == true);
  REQUIRE(triton.players_left() == 1);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 196000);
  REQUIRE(triton.bets(6) == 40500);
  REQUIRE(triton.stack(6) == 460000);

  REQUIRE_THROWS(triton.Apply(poker_engine::Action::kFold));
  REQUIRE_THROWS(triton.NewHand());

  uint8_t cards0[8][2] = {{/*     Loeliger     */}, {/*      Cates       */},
                          {Card("Qh"), Card("Js")}, {Card("Td"), Card("5s")},
                          {Card("Kc"), Card("8h")}, {Card("Jc"), Card("7h")},
                          {Card("Kh"), Card("Jh")}, {Card("7s"), Card("2d")}};
  uint8_t board0[1][5] = {{Card("8c"), Card("6c"), Card("Ks"), Card("5c"),
                           Card("4d")}};
  triton.AwardPot(triton.single_run_, cards0, board0[0]);

  REQUIRE_THROWS(triton.Apply(poker_engine::Action::kFold));
  REQUIRE_THROWS(triton.AwardPot(triton.same_stack_no_rake_, cards0,
                                 board0[0]));
  REQUIRE_THROWS(triton.AwardPot(triton.single_run_, cards0, board0[0]));
  REQUIRE_THROWS(triton.AwardPot(triton.multi_run_, cards0, board0, 1));

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

  triton.NewHand();

  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 0);
  REQUIRE(triton.acting_player() == 4);
  REQUIRE(triton.players_left() == 8);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 10000);

  // Test Paul Phua's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply(poker_engine::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 1);
  REQUIRE(triton.acting_player() == 5);
  REQUIRE(triton.folded(4) == true);
  REQUIRE(triton.players_left() == 7);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 10000);
  REQUIRE(triton.bets(4) == 500);
  REQUIRE(triton.stack(4) == 500000);

  // Test Tony G's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply(poker_engine::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 2);
  REQUIRE(triton.acting_player() == 6);
  REQUIRE(triton.folded(5) == true);
  REQUIRE(triton.players_left() == 6);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 10000);
  REQUIRE(triton.bets(5) == 500);
  REQUIRE(triton.stack(5) == 556000);

  // Test Haxton's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply(poker_engine::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 3);
  REQUIRE(triton.acting_player() == 7);
  REQUIRE(triton.folded(6) == true);
  REQUIRE(triton.players_left() == 5);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 10000);
  REQUIRE(triton.bets(6) == 500);
  REQUIRE(triton.stack(6) == 460000);

  // Test Badziakouski's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply(poker_engine::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 4);
  REQUIRE(triton.acting_player() == 0);
  REQUIRE(triton.folded(7) == true);
  REQUIRE(triton.players_left() == 4);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 10000);
  REQUIRE(triton.bets(7) == 500);
  REQUIRE(triton.stack(7) == 500000);

  // Test Loeliger's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply(poker_engine::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 5);
  REQUIRE(triton.acting_player() == 1);
  REQUIRE(triton.folded(0) == true);
  REQUIRE(triton.players_left() == 3);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 10000);
  REQUIRE(triton.bets(0) == 500);
  REQUIRE(triton.stack(0) == 500000);

  // Test Cates's action (raise to 12000)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.CanBet(12000) == true);
  REQUIRE(triton.Apply(poker_engine::Action::kBet, 12000) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 6);
  REQUIRE(triton.acting_player() == 2);
  REQUIRE(triton.folded(1) == false);
  REQUIRE(triton.players_left() == 3);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 22000);
  REQUIRE(triton.bets(1) == 12500);
  REQUIRE(triton.stack(1) == 486000);

  // Test Kuznetsov's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply(poker_engine::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 7);
  REQUIRE(triton.acting_player() == 3);
  REQUIRE(triton.folded(2) == true);
  REQUIRE(triton.players_left() == 2);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 22000);
  REQUIRE(triton.bets(2) == 2500);
  REQUIRE(triton.stack(2) == 484000);

  REQUIRE_THROWS(triton.AwardPot(triton.single_run_, cards_throw,
                                 board_throw[0]));
  REQUIRE_THROWS(triton.AwardPot(triton.multi_run_, cards_throw, board_throw,
                                 1));
  REQUIRE_THROWS(triton.AwardPot(triton.same_stack_no_rake_, cards_throw,
                                 board_throw[0]));

  // Test Tan Xuan's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply(poker_engine::Action::kFold) == false);
  REQUIRE(triton.in_progress() == false);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 13);
  REQUIRE(triton.acting_player() == 1);
  REQUIRE(triton.folded(3) == true);
  REQUIRE(triton.players_left() == 1);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 22000);
  REQUIRE(triton.bets(3) == 4500);
  REQUIRE(triton.stack(3) == 492000);

  REQUIRE_THROWS(triton.Apply(poker_engine::Action::kFold));
  REQUIRE_THROWS(triton.NewHand());

  uint8_t cards1[8][2] = {{Card("Qd"), Card("6c")}, {Card("Ah"), Card("8d")},
                          {/*     Kuznetsov    */}, {Card("8c"), Card("6h")},
                          {Card("Kc"), Card("4s")}, {Card("7c"), Card("6s")},
                          {Card("7s"), Card("3s")}, {Card("Tc"), Card("8h")}};
  uint8_t board1[1][5] = {{/*Flop1*/ /*Flop2*/ /*Flop3*/ /*Turn*/ /*River*/}};
  triton.AwardPot(triton.single_run_, cards1, board1[0]);

  REQUIRE_THROWS(triton.Apply(poker_engine::Action::kFold));
  REQUIRE_THROWS(triton.AwardPot(triton.single_run_, cards1, board1[0]));
  REQUIRE_THROWS(triton.AwardPot(triton.multi_run_, cards1, board1, 1));
  REQUIRE_THROWS(triton.AwardPot(triton.same_stack_no_rake_, cards1,
                                 board1[0]));

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

  triton.NewHand();

  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 0);
  REQUIRE(triton.acting_player() == 5);
  REQUIRE(triton.players_left() == 8);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 10000);

  // Test Tony G's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply(poker_engine::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 1);
  REQUIRE(triton.acting_player() == 6);
  REQUIRE(triton.folded(5) == true);
  REQUIRE(triton.players_left() == 7);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 10000);
  REQUIRE(triton.bets(5) == 500);
  REQUIRE(triton.stack(5) == 556000);

  // Test Haxton's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply(poker_engine::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 2);
  REQUIRE(triton.acting_player() == 7);
  REQUIRE(triton.folded(6) == true);
  REQUIRE(triton.players_left() == 6);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 10000);
  REQUIRE(triton.bets(6) == 500);
  REQUIRE(triton.stack(6) == 460000);

  // Test Badziakouski's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply(poker_engine::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 3);
  REQUIRE(triton.acting_player() == 0);
  REQUIRE(triton.folded(7) == true);
  REQUIRE(triton.players_left() == 5);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 10000);
  REQUIRE(triton.bets(7) == 500);
  REQUIRE(triton.stack(7) == 500000);

  // Test Loeliger's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply(poker_engine::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 4);
  REQUIRE(triton.acting_player() == 1);
  REQUIRE(triton.folded(0) == true);
  REQUIRE(triton.players_left() == 4);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 10000);
  REQUIRE(triton.bets(0) == 500);
  REQUIRE(triton.stack(0) == 500000);

  // Test Cates's action (raise to 12000)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.CanBet(120000) == true);
  REQUIRE(triton.Apply(poker_engine::Action::kBet, 12000) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 5);
  REQUIRE(triton.acting_player() == 2);
  REQUIRE(triton.folded(1) == false);
  REQUIRE(triton.players_left() == 4);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 22000);
  REQUIRE(triton.bets(1) == 12500);
  REQUIRE(triton.stack(1) == 496000);

  // Test Kuznetsov's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply(poker_engine::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 6);
  REQUIRE(triton.acting_player() == 3);
  REQUIRE(triton.folded(2) == true);
  REQUIRE(triton.players_left() == 3);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 22000);
  REQUIRE(triton.bets(2) == 500);
  REQUIRE(triton.stack(2) == 484000);

  // Test Tan Xuan's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply(poker_engine::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kPreFlop);
  REQUIRE(triton.cycled() == 7);
  REQUIRE(triton.acting_player() == 4);
  REQUIRE(triton.folded(3) == true);
  REQUIRE(triton.players_left() == 2);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 22000);
  REQUIRE(triton.bets(3) == 2500);
  REQUIRE(triton.stack(3) == 490000);

  // Test Paul Phua's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply(poker_engine::Action::kCheckCall) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kFlop);
  REQUIRE(triton.cycled() == 1);
  REQUIRE(triton.acting_player() == 4);
  REQUIRE(triton.folded(4) == false);
  REQUIRE(triton.players_left() == 2);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 30000);
  REQUIRE(triton.bets(4) == 12500);
  REQUIRE(triton.stack(4) == 484000);

  /* -------------------------------- Flop ---------------------------------- */

  // Test Paul Phua's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply(poker_engine::Action::kCheckCall) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kFlop);
  REQUIRE(triton.cycled() == 6);
  REQUIRE(triton.acting_player() == 1);
  REQUIRE(triton.folded(4) == false);
  REQUIRE(triton.players_left() == 2);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 30000);
  REQUIRE(triton.bets(4) == 12500);
  REQUIRE(triton.stack(4) == 484000);

  // Test Cates's action (bet 10000)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.CanBet(10000) == true);
  REQUIRE(triton.Apply(poker_engine::Action::kBet, 10000) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == poker_engine::Round::kFlop);
  REQUIRE(triton.cycled() == 9);
  REQUIRE(triton.acting_player() == 4);
  REQUIRE(triton.folded(1) == false);
  REQUIRE(triton.players_left() == 2);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 40000);
  REQUIRE(triton.bets(1) == 22500);
  REQUIRE(triton.stack(1) == 486000);

  REQUIRE_THROWS(triton.AwardPot(triton.single_run_));
  REQUIRE_THROWS(triton.AwardPot(triton.same_stack_no_rake_, cards_throw,
                                 board_throw[0]));
  REQUIRE_THROWS(triton.AwardPot(triton.multi_run_, cards_throw, board_throw,
                                 1));

  // Test Paul Phua's action (fold)
  REQUIRE(triton.Rotation() == 1);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.Apply(poker_engine::Action::kFold) == false);
  REQUIRE(triton.in_progress() == false);
  REQUIRE(triton.round() == poker_engine::Round::kFlop);
  REQUIRE(triton.cycled() == 14);
  REQUIRE(triton.acting_player() == 1);
  REQUIRE(triton.folded(4) == true);
  REQUIRE(triton.players_left() == 1);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 40000);

  // uint8_t cards2[8][2] =
  //     {{Card("Kc"), Card("Th")}, {Card("Ah"), Card("9d")},
  //      {Card("As"), Card("5d")}, {Card("4h"), Card("2d")},
  //      {Card("Qd"), Card("8h")}, {Card("Tc"), Card("4s")},
  //      {Card("5s"), Card("4d")}, {Card("9c"), Card("3d")}};
  // uint8_t board2[5] = {Card("Kd"), Card("3s"), Card("7d") /*Turn*/
  //                      /*River*/};
  REQUIRE_THROWS(triton.Apply(poker_engine::Action::kAllIn));
  REQUIRE_THROWS(triton.NewHand());
  triton.AwardPot(triton.single_run_);
  REQUIRE(triton.pot() == 0);
  REQUIRE(triton.bets(0) == 0);
  REQUIRE(triton.stack(0) == 500000);
  REQUIRE(triton.bets(1) == 0);
  REQUIRE(triton.stack(1) == 526000);
  REQUIRE(triton.bets(2) == 0);
  REQUIRE(triton.stack(2) == 484000);
  REQUIRE(triton.bets(3) == 0);
  REQUIRE(triton.stack(3) == 490000);
  REQUIRE(triton.bets(4) == 0);
  REQUIRE(triton.stack(4) == 484000);
  REQUIRE(triton.bets(5) == 0);
  REQUIRE(triton.stack(5) == 556000);
  REQUIRE(triton.bets(6) == 0);
  REQUIRE(triton.stack(6) == 460000);
  REQUIRE(triton.bets(7) == 0);
  REQUIRE(triton.stack(7) == 500000);
  REQUIRE_THROWS(triton.Apply(poker_engine::Action::kFold));
  REQUIRE_THROWS(triton.AwardPot(triton.single_run_));
  REQUIRE_THROWS(triton.AwardPot(triton.same_stack_no_rake_, cards_throw,
                                 board_throw[0]));
  REQUIRE_THROWS(triton.AwardPot(triton.multi_run_, cards_throw, board_throw,
                                 1));
}  // TEST_CASE "Triton cash game first 3 hands"

TEST_CASE("heads up big blind ante big blind first", "[poker_engine][node]") {
  auto Card = deck::SKCardFromStr;
  poker_engine::Node<2, utils::Fraction> heads_up(0, 4, 2, 2, true, true, 100);
  REQUIRE_THROWS(heads_up.NewHand());

  // Verify that the constructor worked correctly
  REQUIRE(heads_up.big_blind() == 4);
  REQUIRE(heads_up.small_blind() == 2);
  REQUIRE(heads_up.ante() == 2);
  REQUIRE(heads_up.big_blind_ante() == true);
  REQUIRE(heads_up.blind_before_ante() == true);
  REQUIRE(heads_up.button() == 0);
  REQUIRE(heads_up.in_progress() == true);
  REQUIRE(heads_up.round() == poker_engine::Round::kPreFlop);
  REQUIRE(heads_up.cycled() == 0);
  REQUIRE(heads_up.acting_player() == 0);
  REQUIRE(heads_up.folded(0) == false);
  REQUIRE(heads_up.players_left() == 2);
  REQUIRE(heads_up.players_all_in() == 0);
  REQUIRE(heads_up.pot() == 10);
  REQUIRE(heads_up.bets(0) == 4);
  REQUIRE(heads_up.stack(0) == 98);

  // Verify PlayerIndex works
  REQUIRE(heads_up.PlayerIndex(0) == 0);
  REQUIRE(heads_up.PlayerIndex(1) == 0);
  REQUIRE(heads_up.PlayerIndex(2) == 1);
  REQUIRE(heads_up.PlayerIndex(3) == 0);
  REQUIRE(heads_up.PlayerIndex(4) == 1);
  REQUIRE(heads_up.PlayerIndex(5) == 0);
  REQUIRE(heads_up.PlayerIndex(6) == 1);
  REQUIRE(heads_up.PlayerIndex(7) == 0);
  REQUIRE(heads_up.PlayerIndex(8) == 1);

  // Test John's action (all in)
  REQUIRE(heads_up.Rotation() == 0);
  REQUIRE(heads_up.CanCheckCall() == true);
  REQUIRE(heads_up.CanBet(5) == false);
  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kBet, 5));
  REQUIRE(heads_up.CanBet(6) == true);
  REQUIRE(heads_up.CanBet(97) == true);
  REQUIRE(heads_up.CanBet(98) == false);
  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kBet, 98));
  REQUIRE(heads_up.Apply(poker_engine::Action::kAllIn) == true);
  REQUIRE(heads_up.in_progress() == true);
  REQUIRE(heads_up.round() == poker_engine::Round::kPreFlop);
  REQUIRE(heads_up.cycled() == 1);
  REQUIRE(heads_up.acting_player() == 1);
  REQUIRE(heads_up.folded(0) == false);
  REQUIRE(heads_up.players_left() == 2);
  REQUIRE(heads_up.players_all_in() == 1);
  REQUIRE(heads_up.pot() == 108);
  REQUIRE(heads_up.bets(0) == 102);
  REQUIRE(heads_up.stack(0) == 0);
  REQUIRE_THROWS(heads_up.NewHand());

  uint8_t cards_throw[2][2];
  uint8_t board_throw[1][5];
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.single_run_, cards_throw,
                                   board_throw[0]));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.multi_run_, cards_throw,
                                   board_throw, 1));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.same_stack_no_rake_, cards_throw,
                                   board_throw[0]));

  // Test Mary's action (all in)
  REQUIRE(heads_up.Rotation() == 0);
  REQUIRE(heads_up.CanCheckCall() == false);
  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kCheckCall));
  REQUIRE(heads_up.CanBet(3) == false);
  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kBet, 3));
  REQUIRE(heads_up.CanBet(4) == false);
  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kBet, 4));
  REQUIRE(heads_up.CanBet(91) == false);
  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kBet, 91));
  REQUIRE(heads_up.CanBet(92) == false);
  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kBet, 92));
  REQUIRE(heads_up.Apply(poker_engine::Action::kAllIn) == false);
  REQUIRE(heads_up.in_progress() == false);
  REQUIRE(heads_up.round() == poker_engine::Round::kRiver);
  REQUIRE(heads_up.cycled() == 2);
  REQUIRE(heads_up.acting_player() == 0);
  REQUIRE(heads_up.folded(1) == false);
  REQUIRE(heads_up.players_left() == 2);
  REQUIRE(heads_up.players_all_in() == 2);

  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kFold));
  REQUIRE_THROWS(heads_up.NewHand());

  uint8_t cards0[2][2] = {{Card("7c"), Card("2h")}, {Card("Ah"), Card("As")}};
  uint8_t board0[1][5] = {{Card("9c"), Card("6c"), Card("Ks"), Card("5c"),
                           Card("4d")}};
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.single_run_));
  heads_up.AwardPot(heads_up.single_run_, cards0, board0[0]);

  // Check AwardPot results
  REQUIRE(heads_up.pot() == 0);
  REQUIRE(heads_up.bets(0) == 0);
  REQUIRE(heads_up.bets(1) == 0);
  REQUIRE(heads_up.stack(0) == 4);
  REQUIRE(heads_up.stack(1) == 196);

  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kFold));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.single_run_, cards0, board0[0]));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.multi_run_, cards0, board0, 1));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.same_stack_no_rake_, cards0,
                                   board0[0]));

  /* ------------------------------------------------------------------------ */
  /* ------------------------------- Hand 2 --------------------------------- */
  /* ------------------------------------------------------------------------ */

  heads_up.NewHand();

  REQUIRE(heads_up.in_progress() == true);
  REQUIRE(heads_up.round() == poker_engine::Round::kPreFlop);
  REQUIRE(heads_up.cycled() == 0);
  REQUIRE(heads_up.acting_player() == 1);
  REQUIRE(heads_up.players_left() == 2);
  REQUIRE(heads_up.players_all_in() == 1);
  REQUIRE(heads_up.pot() == 6);

  REQUIRE_THROWS(heads_up.AwardPot(heads_up.single_run_, cards_throw,
                                   board_throw[0]));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.same_stack_no_rake_, cards_throw,
                                   board_throw[0]));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.multi_run_, cards_throw,
                                   board_throw, 1));

  // Test Mary's action (call)
  REQUIRE(heads_up.Rotation() == 0);
  REQUIRE(heads_up.CanCheckCall() == true);
  REQUIRE(heads_up.Apply(poker_engine::Action::kCheckCall) == false);
  REQUIRE(heads_up.in_progress() == false);
  REQUIRE(heads_up.round() == poker_engine::Round::kRiver);
  REQUIRE(heads_up.cycled() == 2);
  REQUIRE(heads_up.acting_player() == 1);
  REQUIRE(heads_up.folded(1) == false);
  REQUIRE(heads_up.players_left() == 2);
  REQUIRE(heads_up.players_all_in() == 1);

  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kFold));
  REQUIRE_THROWS(heads_up.NewHand());

  uint8_t cards1[8][2] = {{Card("5s"), Card("4s")}, {Card("Kh"), Card("Th")}};
  uint8_t board1[1][5] = {{Card("7h"), Card("Ah"), Card("3c"), Card("7s"),
                           Card("6c")}};
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.single_run_));
  heads_up.AwardPot(heads_up.single_run_, cards1, board1[0]);

  // Check AwardPot results
  REQUIRE(heads_up.pot() == 0);
  REQUIRE(heads_up.bets(0) == 0);
  REQUIRE(heads_up.bets(1) == 0);
  REQUIRE(heads_up.stack(0) == 8);
  REQUIRE(heads_up.stack(1) == 192);

  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kFold));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.single_run_, cards1, board1[0]));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.same_stack_no_rake_, cards1,
                                   board1[0]));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.multi_run_, cards1, board1, 1));

  /* ------------------------------------------------------------------------ */
  /* ------------------------------- Hand 3 --------------------------------- */
  /* ------------------------------------------------------------------------ */

  heads_up.NewHand();

  REQUIRE(heads_up.in_progress() == true);
  REQUIRE(heads_up.round() == poker_engine::Round::kPreFlop);
  REQUIRE(heads_up.cycled() == 0);
  REQUIRE(heads_up.acting_player() == 0);
  REQUIRE(heads_up.players_left() == 2);
  REQUIRE(heads_up.players_all_in() == 0);
  REQUIRE(heads_up.pot() == 10);

  // Test John's action (all in)
  REQUIRE(heads_up.Rotation() == 0);
  REQUIRE(heads_up.CanCheckCall() == true);
  REQUIRE(heads_up.CanBet(5) == false);
  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kBet, 5));
  REQUIRE(heads_up.CanBet(6) == false);
  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kBet, 6));
  REQUIRE(heads_up.Apply(poker_engine::Action::kAllIn) == true);
  REQUIRE(heads_up.in_progress() == true);
  REQUIRE(heads_up.round() == poker_engine::Round::kPreFlop);
  REQUIRE(heads_up.cycled() == 1);
  REQUIRE(heads_up.acting_player() == 1);
  REQUIRE(heads_up.folded(0) == false);
  REQUIRE(heads_up.players_left() == 2);
  REQUIRE(heads_up.players_all_in() == 1);
  REQUIRE(heads_up.pot() == 16);
  REQUIRE(heads_up.bets(0) == 10);
  REQUIRE(heads_up.stack(0) == 0);

  REQUIRE_THROWS(heads_up.AwardPot(heads_up.single_run_, cards_throw,
                                   board_throw[0]));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.same_stack_no_rake_, cards_throw,
                                   board_throw[0]));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.multi_run_, cards_throw,
                                   board_throw, 1));

  // Test Mary's action (call)
  REQUIRE(heads_up.Rotation() == 0);
  REQUIRE(heads_up.CanCheckCall() == true);
  REQUIRE(heads_up.CanBet(4) == false);
  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kBet, 4));
  REQUIRE(heads_up.CanBet(6) == false);
  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kBet, 6));
  REQUIRE(heads_up.CanBet(7) == false);
  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kBet, 7));
  REQUIRE(heads_up.CanBet(8) == true);
  REQUIRE(heads_up.Apply(poker_engine::Action::kCheckCall) == false);
  REQUIRE(heads_up.in_progress() == false);
  REQUIRE(heads_up.round() == poker_engine::Round::kRiver);
  REQUIRE(heads_up.cycled() == 2);
  REQUIRE(heads_up.acting_player() == 0);
  REQUIRE(heads_up.folded(1) == false);
  REQUIRE(heads_up.players_left() == 2);
  REQUIRE(heads_up.players_all_in() == 1);

  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kFold));
  REQUIRE_THROWS(heads_up.NewHand());

  uint8_t cards2[8][2] = {{Card("8h"), Card("8c")}, {Card("5d"), Card("5c")}};
  uint8_t board2[1][5] = {{Card("9d"), Card("6h"), Card("7c"), Card("Td"),
                           Card("Kh")}};
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.single_run_));
  heads_up.AwardPot(heads_up.single_run_, cards2, board2[0]);

  // Check AwardPot results
  REQUIRE(heads_up.pot() == 0);
  REQUIRE(heads_up.bets(0) == 0);
  REQUIRE(heads_up.bets(1) == 0);
  REQUIRE(heads_up.stack(0) == 20);
  REQUIRE(heads_up.stack(1) == 180);

  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kFold));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.single_run_, cards2, board2[0]));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.same_stack_no_rake_, cards2,
                                   board2[0]));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.multi_run_, cards2, board2, 1));
}  // TEST_CASE "heads up big blind ante big blind first"

TEST_CASE("heads up big blind ante ante first", "[poker_engine][node]") {
  auto Card = deck::SKCardFromStr;
  poker_engine::Node<2, double> heads_up(0, 4, 2, 2, true, false, 100, 0);
  REQUIRE_THROWS(heads_up.NewHand());

  // Verify that the constructor worked correctly
  REQUIRE(heads_up.big_blind() == 4);
  REQUIRE(heads_up.small_blind() == 2);
  REQUIRE(heads_up.ante() == 2);
  REQUIRE(heads_up.big_blind_ante() == true);
  REQUIRE(heads_up.blind_before_ante() == false);
  REQUIRE(heads_up.button() == 0);
  REQUIRE(heads_up.in_progress() == true);
  REQUIRE(heads_up.round() == poker_engine::Round::kPreFlop);
  REQUIRE(heads_up.cycled() == 0);
  REQUIRE(heads_up.acting_player() == 0);
  REQUIRE(heads_up.folded(0) == false);
  REQUIRE(heads_up.players_left() == 2);
  REQUIRE(heads_up.players_all_in() == 0);
  REQUIRE(heads_up.pot() == 10);
  REQUIRE(heads_up.bets(0) == 4);
  REQUIRE(heads_up.stack(0) == 98);

  // Test John's action (all in)
  REQUIRE(heads_up.Rotation() == 0);
  REQUIRE(heads_up.CanCheckCall() == true);
  REQUIRE(heads_up.CanBet(5) == false);
  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kBet, 5));
  REQUIRE(heads_up.CanBet(6) == true);
  REQUIRE(heads_up.CanBet(97) == true);
  REQUIRE(heads_up.CanBet(98) == false);
  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kBet, 98));
  REQUIRE(heads_up.Apply(poker_engine::Action::kAllIn) == true);
  REQUIRE(heads_up.in_progress() == true);
  REQUIRE(heads_up.round() == poker_engine::Round::kPreFlop);
  REQUIRE(heads_up.cycled() == 1);
  REQUIRE(heads_up.acting_player() == 1);
  REQUIRE(heads_up.folded(0) == false);
  REQUIRE(heads_up.players_left() == 2);
  REQUIRE(heads_up.players_all_in() == 1);
  REQUIRE(heads_up.pot() == 108);
  REQUIRE(heads_up.bets(0) == 102);
  REQUIRE(heads_up.stack(0) == 0);
  REQUIRE_THROWS(heads_up.NewHand());

  uint8_t cards_throw[2][2];
  uint8_t board_throw[1][5];
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.single_run_, cards_throw,
                                   board_throw[0]));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.same_stack_no_rake_, cards_throw,
                                   board_throw[0]));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.multi_run_, cards_throw,
                                   board_throw, 8));

  // Test Mary's action (all in)
  REQUIRE(heads_up.Rotation() == 0);
  REQUIRE(heads_up.CanCheckCall() == false);
  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kCheckCall));
  REQUIRE(heads_up.CanBet(3) == false);
  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kBet, 3));
  REQUIRE(heads_up.CanBet(4) == false);
  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kBet, 4));
  REQUIRE(heads_up.CanBet(91) == false);
  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kBet, 91));
  REQUIRE(heads_up.CanBet(92) == false);
  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kBet, 92));
  REQUIRE(heads_up.Apply(poker_engine::Action::kAllIn) == false);
  REQUIRE(heads_up.in_progress() == false);
  REQUIRE(heads_up.round() == poker_engine::Round::kRiver);
  REQUIRE(heads_up.cycled() == 2);
  REQUIRE(heads_up.acting_player() == 0);
  REQUIRE(heads_up.folded(1) == false);
  REQUIRE(heads_up.players_left() == 2);
  REQUIRE(heads_up.players_all_in() == 2);

  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kFold));
  REQUIRE_THROWS(heads_up.NewHand());

  uint8_t cards0[2][2] = {{Card("6d"), Card("6c")}, {Card("Qc"), Card("Jd")}};
  uint8_t board0[1][5] = {{Card("7h"), Card("4s"), Card("Js"), Card("Th"),
                           Card("Ad")}};
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.single_run_));
  heads_up.AwardPot(heads_up.single_run_, cards0, board0[0]);

  // Check AwardPot results
  REQUIRE(heads_up.pot() == 0);
  REQUIRE(heads_up.bets(0) == 0);
  REQUIRE(heads_up.bets(1) == 0);
  REQUIRE(heads_up.stack(0) == 4);
  REQUIRE(heads_up.stack(1) == 196);

  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kFold));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.single_run_, cards0, board0[0]));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.same_stack_no_rake_, cards0,
                                   board0[0]));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.multi_run_, cards0, board0, 8));

  /* ------------------------------------------------------------------------ */
  /* ------------------------------- Hand 2 --------------------------------- */
  /* ------------------------------------------------------------------------ */

  heads_up.NewHand();

  REQUIRE(heads_up.in_progress() == true);
  REQUIRE(heads_up.round() == poker_engine::Round::kPreFlop);
  REQUIRE(heads_up.cycled() == 0);
  REQUIRE(heads_up.acting_player() == 1);
  REQUIRE(heads_up.players_left() == 2);
  REQUIRE(heads_up.players_all_in() == 1);
  REQUIRE(heads_up.pot() == 6);

  REQUIRE_THROWS(heads_up.AwardPot(heads_up.single_run_, cards_throw,
                                   board_throw[0]));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.same_stack_no_rake_, cards_throw,
                                   board_throw[0]));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.multi_run_, cards_throw,
                                   board_throw, 8));

  // Test Mary's action (call)
  REQUIRE(heads_up.Rotation() == 0);
  REQUIRE(heads_up.CanCheckCall() == true);
  REQUIRE(heads_up.Apply(poker_engine::Action::kCheckCall) == false);
  REQUIRE(heads_up.in_progress() == false);
  REQUIRE(heads_up.round() == poker_engine::Round::kRiver);
  REQUIRE(heads_up.cycled() == 2);
  REQUIRE(heads_up.acting_player() == 1);
  REQUIRE(heads_up.folded(1) == false);
  REQUIRE(heads_up.players_left() == 2);
  REQUIRE(heads_up.players_all_in() == 1);

  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kFold));
  REQUIRE_THROWS(heads_up.NewHand());

  uint8_t cards1[8][2] = {{Card("Ah"), Card("Kd")}, {Card("Qc"), Card("Qh")}};
  uint8_t board1[1][5] = {{Card("3h"), Card("6c"), Card("9c"), Card("9d"),
                           Card("Ad")}};
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.single_run_));
  heads_up.AwardPot(heads_up.single_run_, cards1, board1[0]);

  // Check AwardPot results
  REQUIRE(heads_up.pot() == 0);
  REQUIRE(heads_up.bets(0) == 0);
  REQUIRE(heads_up.bets(1) == 0);
  REQUIRE(heads_up.stack(0) == 4);
  REQUIRE(heads_up.stack(1) == 196);

  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kFold));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.single_run_, cards1, board1[0]));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.same_stack_no_rake_, cards1,
                                   board1[0]));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.multi_run_, cards1, board1, 8));

  /* ------------------------------------------------------------------------ */
  /* ------------------------------- Hand 3 --------------------------------- */
  /* ------------------------------------------------------------------------ */

  heads_up.NewHand();

  REQUIRE(heads_up.in_progress() == true);
  REQUIRE(heads_up.round() == poker_engine::Round::kPreFlop);
  REQUIRE(heads_up.cycled() == 0);
  REQUIRE(heads_up.acting_player() == 0);
  REQUIRE(heads_up.players_left() == 2);
  REQUIRE(heads_up.players_all_in() == 0);
  REQUIRE(heads_up.pot() == 10);

  // Test John's action (all in)
  REQUIRE(heads_up.Rotation() == 0);
  REQUIRE(heads_up.CanCheckCall() == false);
  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kCheckCall));
  REQUIRE(heads_up.CanBet(0) == false);
  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kBet, 0));
  REQUIRE(heads_up.CanBet(1) == false);
  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kBet, 1));
  REQUIRE(heads_up.CanBet(2) == false);
  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kBet, 2));
  REQUIRE(heads_up.CanBet(3) == false);
  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kBet, 3));
  REQUIRE(heads_up.Apply(poker_engine::Action::kAllIn) == true);
  REQUIRE(heads_up.in_progress() == true);
  REQUIRE(heads_up.round() == poker_engine::Round::kPreFlop);
  REQUIRE(heads_up.cycled() == 1);
  REQUIRE(heads_up.acting_player() == 1);
  REQUIRE(heads_up.folded(0) == false);
  REQUIRE(heads_up.players_left() == 2);
  REQUIRE(heads_up.players_all_in() == 1);
  REQUIRE(heads_up.pot() == 12);
  REQUIRE(heads_up.bets(0) == 6);
  REQUIRE(heads_up.stack(0) == 0);

  REQUIRE_THROWS(heads_up.AwardPot(heads_up.single_run_, cards_throw,
                                   board_throw[0]));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.multi_run_, cards_throw,
                                   board_throw, 1));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.same_stack_no_rake_, cards_throw,
                                   board_throw[0]));

  // Test Mary's action (call)
  REQUIRE(heads_up.Rotation() == 0);
  REQUIRE(heads_up.CanCheckCall() == true);
  REQUIRE(heads_up.CanBet(3) == false);
  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kBet, 3));
  REQUIRE(heads_up.CanBet(4) == true);
  REQUIRE(heads_up.CanBet(187) == true);
  REQUIRE(heads_up.CanBet(188) == false);
  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kBet, 188));
  REQUIRE(heads_up.Apply(poker_engine::Action::kCheckCall) == false);
  REQUIRE(heads_up.in_progress() == false);
  REQUIRE(heads_up.round() == poker_engine::Round::kRiver);
  REQUIRE(heads_up.cycled() == 2);
  REQUIRE(heads_up.acting_player() == 0);
  REQUIRE(heads_up.folded(1) == false);
  REQUIRE(heads_up.players_left() == 2);
  REQUIRE(heads_up.players_all_in() == 1);

  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kFold));
  REQUIRE_THROWS(heads_up.NewHand());

  uint8_t cards2[8][2] = {{Card("Qc"), Card("Qh")}, {Card("8s"), Card("8d")}};
  uint8_t board2[1][5] = {{Card("3d"), Card("Ac"), Card("Kh"), Card("3h"),
                           Card("2h")}};
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.single_run_));
  heads_up.AwardPot(heads_up.single_run_, cards2, board2[0]);

  // Check AwardPot results
  REQUIRE(heads_up.pot() == 0);
  REQUIRE(heads_up.bets(0) == 0);
  REQUIRE(heads_up.bets(1) == 0);
  REQUIRE(heads_up.stack(0) == 12);
  REQUIRE(heads_up.stack(1) == 188);

  REQUIRE_THROWS(heads_up.Apply(poker_engine::Action::kFold));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.single_run_, cards2, board2[0]));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.multi_run_, cards2, board2, 1));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.same_stack_no_rake_, cards2,
                                   board2[0]));
}  // TEST_CASE "heads up big blind ante ante first"

TEST_CASE("straddle", "[poker_engine][node]") {
  auto Card = deck::SKCardFromStr;
  poker_engine::Node<6, utils::Fraction> game(0, 100, 50, 0, true, false, 10000,
                                              3, utils::Fraction{}, 5, true);
  REQUIRE_THROWS(game.NewHand());

  // Verify that the constructor worked correctly
  REQUIRE(game.button() == 0);
  REQUIRE(game.big_blind() == 100);
  REQUIRE(game.small_blind() == 50);
  REQUIRE(game.ante() == 0);
  REQUIRE(game.big_blind_ante() == true);
  REQUIRE(game.blind_before_ante() == false);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == poker_engine::Round::kPreFlop);
  REQUIRE(game.cycled() == 0);
  REQUIRE(game.acting_player() == 0);
  REQUIRE(game.folded(0) == false);
  REQUIRE(game.players_left() == 6);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 1550);
  REQUIRE(game.bets(0) == 0);
  REQUIRE(game.stack(0) == 10000);

  uint8_t card_throws[8][2];
  uint8_t board_throws[1][5];
  REQUIRE_THROWS(game.AwardPot(game.single_run_, card_throws, board_throws[0]));
  REQUIRE_THROWS(game.AwardPot(game.multi_run_, card_throws, board_throws, 1));
  REQUIRE_THROWS(game.AwardPot(game.same_stack_no_rake_, card_throws,
                               board_throws[0]));

  // Player 0 action (fold)
  REQUIRE(game.Rotation() == 0);
  REQUIRE(game.CanCheckCall() == true);
  REQUIRE(game.CanBet(1599) == false);
  REQUIRE_THROWS(game.Apply(poker_engine::Action::kBet, 1599));
  REQUIRE(game.CanBet(1600) == true);
  REQUIRE(game.CanBet(9999) == true);
  REQUIRE(game.CanBet(10000) == false);
  REQUIRE_THROWS(game.Apply(poker_engine::Action::kBet, 10000));
  REQUIRE(game.Apply(poker_engine::Action::kFold) == true);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == poker_engine::Round::kPreFlop);
  REQUIRE(game.cycled() == 1);
  REQUIRE(game.acting_player() == 1);
  REQUIRE(game.folded(0) == true);
  REQUIRE(game.players_left() == 5);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 1550);
  REQUIRE(game.bets(0) == 0);
  REQUIRE(game.stack(0) == 10000);
  REQUIRE_THROWS(game.NewHand());

  // Player 1 action (fold)
  REQUIRE(game.Rotation() == 0);
  REQUIRE(game.CanCheckCall() == true);
  REQUIRE(game.CanBet(800) == false);
  REQUIRE_THROWS(game.Apply(poker_engine::Action::kBet, 800));
  REQUIRE(game.CanBet(1549) == false);
  REQUIRE_THROWS(game.Apply(poker_engine::Action::kBet, 1549));
  REQUIRE(game.CanBet(1550) == true);
  REQUIRE(game.CanBet(9949) == true);
  REQUIRE(game.CanBet(9950) == false);
  REQUIRE_THROWS(game.Apply(poker_engine::Action::kBet, 9950));
  REQUIRE(game.Apply(poker_engine::Action::kFold) == true);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == poker_engine::Round::kPreFlop);
  REQUIRE(game.cycled() == 2);
  REQUIRE(game.acting_player() == 2);
  REQUIRE(game.folded(1) == true);
  REQUIRE(game.players_left() == 4);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 1550);
  REQUIRE(game.bets(1) == 50);
  REQUIRE(game.stack(1) == 9950);

  // Player 2 action (fold)
  REQUIRE(game.Rotation() == 0);
  REQUIRE(game.CanCheckCall() == true);
  REQUIRE(game.CanBet(800) == false);
  REQUIRE_THROWS(game.Apply(poker_engine::Action::kBet, 800));
  REQUIRE(game.CanBet(1499) == false);
  REQUIRE_THROWS(game.Apply(poker_engine::Action::kBet, 1499));
  REQUIRE(game.CanBet(1500) == true);
  REQUIRE(game.CanBet(9899) == true);
  REQUIRE(game.CanBet(9900) == false);
  REQUIRE_THROWS(game.Apply(poker_engine::Action::kBet, 9900));
  REQUIRE(game.Apply(poker_engine::Action::kFold) == true);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == poker_engine::Round::kPreFlop);
  REQUIRE(game.cycled() == 3);
  REQUIRE(game.acting_player() == 3);
  REQUIRE(game.folded(2) == true);
  REQUIRE(game.players_left() == 3);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 1550);
  REQUIRE(game.bets(2) == 100);
  REQUIRE(game.stack(2) == 9900);

  // Player 3 action (call)
  REQUIRE(game.Rotation() == 0);
  REQUIRE(game.CanCheckCall() == true);
  REQUIRE(game.CanBet(600) == false);
  REQUIRE_THROWS(game.Apply(poker_engine::Action::kBet, 600));
  REQUIRE(game.Apply(poker_engine::Action::kCheckCall) == true);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == poker_engine::Round::kPreFlop);
  REQUIRE(game.cycled() == 4);
  REQUIRE(game.acting_player() == 4);
  REQUIRE(game.folded(3) == false);
  REQUIRE(game.players_left() == 3);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 2150);
  REQUIRE(game.bets(3) == 800);
  REQUIRE(game.stack(3) == 9200);

  // Player 4 action (call)
  REQUIRE(game.Rotation() == 0);
  REQUIRE(game.CanCheckCall() == true);
  REQUIRE(game.CanBet(400) == false);
  REQUIRE_THROWS(game.Apply(poker_engine::Action::kBet, 400));
  REQUIRE(game.Apply(poker_engine::Action::kCheckCall) == true);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == poker_engine::Round::kPreFlop);
  REQUIRE(game.cycled() == 5);
  REQUIRE(game.acting_player() == 5);
  REQUIRE(game.folded(4) == false);
  REQUIRE(game.players_left() == 3);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 2550);
  REQUIRE(game.bets(4) == 800);
  REQUIRE(game.stack(4) == 9200);

  // Player 5 action (check)
  REQUIRE(game.Rotation() == 0);
  REQUIRE(game.CanCheckCall() == true);
  REQUIRE(game.Apply(poker_engine::Action::kCheckCall) == true);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == poker_engine::Round::kFlop);
  REQUIRE(game.cycled() == 2);
  REQUIRE(game.acting_player() == 3);
  REQUIRE(game.folded(5) == false);
  REQUIRE(game.players_left() == 3);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 2550);
  REQUIRE(game.bets(5) == 800);
  REQUIRE(game.stack(5) == 9200);

  /* -------------------------------- Flop ---------------------------------- */

  // Player 3 action (check)
  REQUIRE(game.Rotation() == 0);
  REQUIRE(game.CanCheckCall() == true);
  REQUIRE(game.CanBet(99) == false);
  REQUIRE_THROWS(game.Apply(poker_engine::Action::kBet, 99));
  REQUIRE(game.CanBet(100) == true);
  REQUIRE(game.Apply(poker_engine::Action::kCheckCall) == true);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == poker_engine::Round::kFlop);
  REQUIRE(game.cycled() == 3);
  REQUIRE(game.acting_player() == 4);
  REQUIRE(game.folded(3) == false);
  REQUIRE(game.players_left() == 3);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 2550);
  REQUIRE(game.bets(3) == 800);
  REQUIRE(game.stack(3) == 9200);

  // Player 4 action (raise 800)
  REQUIRE(game.Rotation() == 0);
  REQUIRE(game.CanCheckCall() == true);
  REQUIRE(game.CanBet(99) == false);
  REQUIRE_THROWS(game.Apply(poker_engine::Action::kBet, 99));
  REQUIRE(game.CanBet(100) == true);
  REQUIRE(game.CanBet(800) == true);
  REQUIRE(game.CanBet(9200) == false);
  REQUIRE_THROWS(game.Apply(poker_engine::Action::kBet, 9200));
  REQUIRE(game.Apply(poker_engine::Action::kBet, 800) == true);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == poker_engine::Round::kFlop);
  REQUIRE(game.cycled() == 4);
  REQUIRE(game.acting_player() == 5);
  REQUIRE(game.folded(4) == false);
  REQUIRE(game.players_left() == 3);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 3350);
  REQUIRE(game.bets(4) == 1600);
  REQUIRE(game.stack(4) == 8400);

  // Player 5 action (fold)
  REQUIRE(game.Rotation() == 0);
  REQUIRE(game.CanCheckCall() == true);
  REQUIRE(game.Apply(poker_engine::Action::kFold) == true);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == poker_engine::Round::kFlop);
  REQUIRE(game.cycled() == 8);
  REQUIRE(game.acting_player() == 3);
  REQUIRE(game.folded(5) == true);
  REQUIRE(game.players_left() == 2);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 3350);
  REQUIRE(game.bets(5) == 800);
  REQUIRE(game.stack(5) == 9200);

  // Player 3 action (call)
  REQUIRE(game.Rotation() == 1);
  REQUIRE(game.CanCheckCall() == true);
  REQUIRE(game.Apply(poker_engine::Action::kCheckCall) == true);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == poker_engine::Round::kTurn);
  REQUIRE(game.cycled() == 2);
  REQUIRE(game.acting_player() == 3);
  REQUIRE(game.folded(3) == false);
  REQUIRE(game.players_left() == 2);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 4150);
  REQUIRE(game.bets(3) == 1600);
  REQUIRE(game.stack(3) == 8400);

  /* -------------------------------- Turn ---------------------------------- */

  // Player 3 action (check)
  REQUIRE(game.Rotation() == 0);
  REQUIRE(game.CanCheckCall() == true);
  REQUIRE(game.Apply(poker_engine::Action::kCheckCall) == true);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == poker_engine::Round::kTurn);
  REQUIRE(game.cycled() == 3);
  REQUIRE(game.acting_player() == 4);
  REQUIRE(game.folded(3) == false);
  REQUIRE(game.players_left() == 2);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 4150);
  REQUIRE(game.bets(3) == 1600);
  REQUIRE(game.stack(3) == 8400);

  // Player 4 action (check)
  REQUIRE(game.Rotation() == 0);
  REQUIRE(game.CanCheckCall() == true);
  REQUIRE(game.Apply(poker_engine::Action::kCheckCall) == true);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == poker_engine::Round::kRiver);
  REQUIRE(game.cycled() == 2);
  REQUIRE(game.acting_player() == 3);
  REQUIRE(game.folded(4) == false);
  REQUIRE(game.players_left() == 2);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 4150);
  REQUIRE(game.bets(4) == 1600);
  REQUIRE(game.stack(4) == 8400);

  /* -------------------------------- River --------------------------------- */

  // Player 3 action (check)
  REQUIRE(game.Rotation() == 0);
  REQUIRE(game.CanCheckCall() == true);
  REQUIRE(game.Apply(poker_engine::Action::kCheckCall) == true);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == poker_engine::Round::kRiver);
  REQUIRE(game.cycled() == 3);
  REQUIRE(game.acting_player() == 4);
  REQUIRE(game.folded(3) == false);
  REQUIRE(game.players_left() == 2);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 4150);
  REQUIRE(game.bets(3) == 1600);
  REQUIRE(game.stack(3) == 8400);

  // Player 4 action (raise 1600)
  REQUIRE(game.Rotation() == 0);
  REQUIRE(game.CanCheckCall() == true);
  REQUIRE(game.CanBet(99) == false);
  REQUIRE_THROWS(game.Apply(poker_engine::Action::kBet, 99));
  REQUIRE(game.CanBet(100) == true);
  REQUIRE(game.CanBet(1600) == true);
  REQUIRE(game.CanBet(8399) == true);
  REQUIRE(game.CanBet(8400) == false);
  REQUIRE_THROWS(game.Apply(poker_engine::Action::kBet, 8400));
  REQUIRE(game.Apply(poker_engine::Action::kBet, 1600) == true);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == poker_engine::Round::kRiver);
  REQUIRE(game.cycled() == 8);
  REQUIRE(game.acting_player() == 3);
  REQUIRE(game.folded(4) == false);
  REQUIRE(game.players_left() == 2);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 5750);
  REQUIRE(game.bets(4) == 3200);
  REQUIRE(game.stack(4) == 6800);

  // Player 3 action (raise all in)
  REQUIRE(game.Rotation() == 1);
  REQUIRE(game.CanCheckCall() == true);
  REQUIRE(game.Apply(poker_engine::Action::kAllIn) == true);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == poker_engine::Round::kRiver);
  REQUIRE(game.cycled() == 9);
  REQUIRE(game.acting_player() == 4);
  REQUIRE(game.folded(3) == false);
  REQUIRE(game.players_left() == 2);
  REQUIRE(game.players_all_in() == 1);
  REQUIRE(game.pot() == 14150);
  REQUIRE(game.bets(3) == 10000);
  REQUIRE(game.stack(3) == 0);

  // Player 4 action (call all in)
  REQUIRE(game.Rotation() == 1);
  REQUIRE(game.CanCheckCall() == false);
  REQUIRE_THROWS(game.Apply(poker_engine::Action::kCheckCall));
  REQUIRE(game.Apply(poker_engine::Action::kAllIn) == false);
  REQUIRE(game.in_progress() == false);
  REQUIRE(game.round() == poker_engine::Round::kRiver);
  REQUIRE(game.cycled() == 14);
  REQUIRE(game.acting_player() == 3);
  REQUIRE(game.folded(4) == false);
  REQUIRE(game.players_left() == 2);
  REQUIRE(game.players_all_in() == 2);
  REQUIRE(game.pot() == 20950);
  REQUIRE(game.bets(4) == 10000);
  REQUIRE(game.stack(4) == 0);

  /* ------------------------------ Award Pot ------------------------------- */

  REQUIRE_THROWS(game.Apply(poker_engine::Action::kFold));
  REQUIRE_THROWS(game.NewHand());

  uint8_t cards0[6][2] = {{Card("Jc"), Card("9d")}, {0, 0}, {0, 0}, {0, 0},
                          {Card("Jh"), Card("Kd")}, {0, 0}};
  uint8_t board0[1][5] = {{Card("Qd"), Card("4s"), Card("Kc"), Card("6d"),
                           Card("Tc")}};
  REQUIRE_THROWS(game.AwardPot(game.multi_run_));
  game.AwardPot(game.multi_run_, cards0, board0, 1);

  // Check AwardPot results
  REQUIRE(game.pot() == 0);
  for (uint8_t i = 0; i < 6; ++i) {
    REQUIRE(game.bets(i) == 0);
  }
  REQUIRE(game.stack(0) == 10000);
  REQUIRE(game.stack(1) == 9950);
  REQUIRE(game.stack(2) == 9900);
  REQUIRE(game.stack(3) == 0);
  REQUIRE(game.stack(4) == 20950);
  REQUIRE(game.stack(5) == 9200);

  REQUIRE_THROWS(game.Apply(poker_engine::Action::kFold));
  REQUIRE_THROWS(game.AwardPot(game.single_run_, cards0, board0[0]));
  REQUIRE_THROWS(game.AwardPot(game.multi_run_, cards0, board0, 1));
  REQUIRE_THROWS(game.AwardPot(game.same_stack_no_rake_, cards0,
                               board0[0]));
}  // TEST_CASE "straddle"

TEST_CASE("straddle with all in", "[poker_engine][node]") {
  poker_engine::Node<6, double> game(0, 100, 50, 0, true, false, 400, 3);
  REQUIRE_THROWS(game.NewHand());

  uint8_t card_throws[8][2];
  uint8_t board_throws[1][5];
  REQUIRE_THROWS(game.AwardPot(game.single_run_, card_throws, board_throws[0]));
  REQUIRE_THROWS(game.AwardPot(game.multi_run_, card_throws, board_throws, 1));
  REQUIRE_THROWS(game.AwardPot(game.same_stack_no_rake_, card_throws,
                               board_throws[0]));

  REQUIRE(game.button() == 0);
  REQUIRE(game.big_blind() == 100);
  REQUIRE(game.small_blind() == 50);
  REQUIRE(game.ante() == 0);
  REQUIRE(game.big_blind_ante() == true);
  REQUIRE(game.blind_before_ante() == false);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == poker_engine::Round::kPreFlop);
  REQUIRE(game.cycled() == 0);
  REQUIRE(game.acting_player() == 5);
  for (uint8_t i = 0; i < 6; ++i) {
    REQUIRE(game.folded(i) == false);
  }
  REQUIRE(game.players_left() == 6);
  REQUIRE(game.players_all_in() == 1);
  REQUIRE(game.pot() == 750);
  REQUIRE(game.bets(0) == 0);
  REQUIRE(game.bets(1) == 50);
  REQUIRE(game.bets(2) == 100);
  REQUIRE(game.bets(3) == 200);
  REQUIRE(game.bets(4) == 400);
  REQUIRE(game.bets(5) == 0);
  REQUIRE(game.stack(0) == 400);
  REQUIRE(game.stack(1) == 350);
  REQUIRE(game.stack(2) == 300);
  REQUIRE(game.stack(3) == 200);
  REQUIRE(game.stack(4) == 0);
  REQUIRE(game.stack(5) == 400);
}  // TEST_CASE "straddle with all in"

TEST_CASE("straddle with bb ante", "[poker_engine][node]") {
  poker_engine::Node<6, double> game(0, 100, 50, 100, true, false, 10000, 1);
  REQUIRE_THROWS(game.NewHand());

  uint8_t card_throws[8][2];
  uint8_t board_throws[1][5];
  REQUIRE_THROWS(game.AwardPot(game.single_run_, card_throws, board_throws[0]));
  REQUIRE_THROWS(game.AwardPot(game.multi_run_, card_throws, board_throws, 1));
  REQUIRE_THROWS(game.AwardPot(game.same_stack_no_rake_, card_throws,
                               board_throws[0]));

  REQUIRE(game.button() == 0);
  REQUIRE(game.big_blind() == 100);
  REQUIRE(game.small_blind() == 50);
  REQUIRE(game.ante() == 100);
  REQUIRE(game.big_blind_ante() == true);
  REQUIRE(game.blind_before_ante() == false);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == poker_engine::Round::kPreFlop);
  REQUIRE(game.cycled() == 0);
  REQUIRE(game.acting_player() == 4);
  for (uint8_t i = 0; i < 6; ++i) {
    REQUIRE(game.folded(i) == false);
  }
  REQUIRE(game.players_left() == 6);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 950);
  REQUIRE(game.bets(0) == 100);
  REQUIRE(game.bets(1) == 150);
  REQUIRE(game.bets(2) == 200);
  REQUIRE(game.bets(3) == 300);
  REQUIRE(game.bets(4) == 100);
  REQUIRE(game.bets(5) == 100);
  REQUIRE(game.stack(0) == 10000);
  REQUIRE(game.stack(1) == 9950);
  REQUIRE(game.stack(2) == 9300);
  REQUIRE(game.stack(3) == 9800);
  REQUIRE(game.stack(4) == 10000);
  REQUIRE(game.stack(5) == 10000);
  REQUIRE(game.CanBet(200) == false);
  REQUIRE(game.CanBet(399) == false);
  REQUIRE(game.CanBet(400) == true);
}  // TEST_CASE "straddle with bb ante"

TEST_CASE("straddle with ante", "[poker_engine][node]") {
  poker_engine::Node<6, double> game(0, 100, 50, 100, false, false, 10000, 1);
  REQUIRE_THROWS(game.NewHand());

  uint8_t card_throws[8][2];
  uint8_t board_throws[1][5];
  REQUIRE_THROWS(game.AwardPot(game.single_run_, card_throws, board_throws[0]));
  REQUIRE_THROWS(game.AwardPot(game.multi_run_, card_throws, board_throws, 1));
  REQUIRE_THROWS(game.AwardPot(game.same_stack_no_rake_, card_throws,
                               board_throws[0]));

  REQUIRE(game.button() == 0);
  REQUIRE(game.big_blind() == 100);
  REQUIRE(game.small_blind() == 50);
  REQUIRE(game.ante() == 100);
  REQUIRE(game.big_blind_ante() == false);
  REQUIRE(game.blind_before_ante() == false);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == poker_engine::Round::kPreFlop);
  REQUIRE(game.cycled() == 0);
  REQUIRE(game.acting_player() == 4);
  for (uint8_t i = 0; i < 6; ++i) {
    REQUIRE(game.folded(i) == false);
  }
  REQUIRE(game.players_left() == 6);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 950);
  REQUIRE(game.bets(0) == 100);
  REQUIRE(game.bets(1) == 150);
  REQUIRE(game.bets(2) == 200);
  REQUIRE(game.bets(3) == 300);
  REQUIRE(game.bets(4) == 100);
  REQUIRE(game.bets(5) == 100);
  REQUIRE(game.stack(0) == 9900);
  REQUIRE(game.stack(1) == 9850);
  REQUIRE(game.stack(2) == 9800);
  REQUIRE(game.stack(3) == 9700);
  REQUIRE(game.stack(4) == 9900);
  REQUIRE(game.stack(5) == 9900);
  REQUIRE(game.CanBet(200) == false);
  REQUIRE(game.CanBet(399) == false);
  REQUIRE(game.CanBet(400) == true);
}  // TEST_CASE "straddle with ante"

TEST_CASE("awardpot same stack no rake double", "[poker_engine][node]") {
  auto Card = deck::SKCardFromStr;
  poker_engine::Node<3, double> game(0, 100, 50, 0, false, false, 10000);
  game.Apply(poker_engine::Action::kAllIn);
  game.Apply(poker_engine::Action::kAllIn);
  game.Apply(poker_engine::Action::kAllIn);
  REQUIRE_THROWS(game.AwardPot(game.same_stack_no_rake_));
  uint8_t board[5] = {Card("As"), Card("Ks"), Card("Qs"), Card("Js"),
                      Card("Ts")};
  uint8_t hands[3][2] = {{Card("2s"), Card("2h")}, {Card("3s"), Card("3h")},
                         {Card("4s"), Card("4h")}};
  game.AwardPot(game.same_stack_no_rake_, hands, board);
  REQUIRE(game.stack(0) == 10000);
  REQUIRE(game.stack(1) == 10000);
  REQUIRE(game.stack(2) == 10000);
  for (uint8_t i = 0; i < 3; ++i) {
    REQUIRE(game.bets(i) == 0);
  }
  REQUIRE(game.pot() == 0);
}  // TEST_CASE "awardpot same stack no rake double"

TEST_CASE("awardpot same stack no rake fraction", "[poker_engine][node]") {
  auto Card = deck::SKCardFromStr;
  poker_engine::Node<3, utils::Fraction> game(0, 100, 50, 0, false, false,
                                              10000);
  game.Apply(poker_engine::Action::kAllIn);
  game.Apply(poker_engine::Action::kAllIn);
  game.Apply(poker_engine::Action::kAllIn);
  REQUIRE_THROWS(game.AwardPot(game.same_stack_no_rake_));
  uint8_t board[5] = {Card("2d"), Card("2c"), Card("Qs"), Card("Js"),
                      Card("Ts")};
  uint8_t hands[3][2] = {{Card("2s"), Card("2h")}, {Card("3s"), Card("3h")},
                         {0, 0}};
  game.AwardPot(game.same_stack_no_rake_, hands, board);
  REQUIRE(game.stack(0) == 30000);
  REQUIRE(game.stack(1) == 0);
  REQUIRE(game.stack(2) == 0);
  for (uint8_t i = 0; i < 3; ++i) {
    REQUIRE(game.bets(i) == 0);
  }
  REQUIRE(game.pot() == 0);
}  // TEST_CASE "awardpot same stack no rake fraction"

TEST_CASE("awardpot single run double", "[poker_engine][node]") {
  auto Card = deck::SKCardFromStr;
  poker_engine::Node<3, double> game(0, 100, 50, 100, true, false, 10000);
  game.Apply(poker_engine::Action::kAllIn);
  REQUIRE(game.CanCheckCall() == false);
  game.Apply(poker_engine::Action::kAllIn);
  REQUIRE(game.CanCheckCall() == false);
  game.Apply(poker_engine::Action::kAllIn);
  REQUIRE_THROWS(game.AwardPot(game.single_run_));
  uint8_t board[5] = {Card("8c"), Card("9c"), Card("Ts"), Card("Js"),
                      Card("Qs")};
  uint8_t hands[3][2] = {{Card("2s"), Card("2h")}, {Card("3s"), Card("3h")},
                         {Card("Ah"), Card("Kh")}};
  game.AwardPot(game.single_run_, hands, board);
  REQUIRE(game.stack(0) == 300);
  REQUIRE(game.stack(1) == 300);
  REQUIRE(game.stack(2) == 29400);
  for (uint8_t i = 0; i < 3; ++i) {
    REQUIRE(game.bets(i) == 0);
  }
  REQUIRE(game.pot() == 0);
}  // TEST_CASE "awardpot single run double"

TEST_CASE("awardpot single run fraction", "[poker_engine][node]") {
  auto Card = deck::SKCardFromStr;
  poker_engine::Node<4, utils::Fraction> game(0, 100, 50, 100, true, false,
                                              10000);
  game.Apply(poker_engine::Action::kAllIn);
  REQUIRE(game.CanCheckCall() == false);
  game.Apply(poker_engine::Action::kAllIn);
  REQUIRE(game.CanCheckCall() == false);
  game.Apply(poker_engine::Action::kAllIn);
  REQUIRE(game.CanCheckCall() == false);
  game.Apply(poker_engine::Action::kAllIn);
  REQUIRE_THROWS(game.AwardPot(game.single_run_));
  uint8_t board[5] = {Card("8c"), Card("9c"), Card("Ts"), Card("Js"),
                      Card("Qs")};
  uint8_t hands[4][2] = {{Card("Ad"), Card("Kd")}, {Card("Ac"), Card("Kc")},
                         {Card("Ah"), Card("Kh")}, {Card("2s"), Card("2h")}};
  game.AwardPot(game.single_run_, hands, board);
  REQUIRE((game.stack(0) == 13533 || game.stack(0) == 13534));
  REQUIRE((game.stack(1) == 13533 || game.stack(1) == 13534));
  REQUIRE((game.stack(2) == 12933 || game.stack(2) == 12934));
  REQUIRE(game.stack(3) == 0);
  REQUIRE(game.stack(0) + game.stack(1) + game.stack(2) + game.stack(3) ==
          40000);
  for (uint8_t i = 0; i < 4; ++i) {
    REQUIRE(game.bets(i) == 0);
  }
  REQUIRE(game.pot() == 0);
}  // TEST_CASE "awardpot single run double"

TEMPLATE_TEST_CASE("awardpot multi run", "[poker_engine][node]", double,
    utils::Fraction) {
  auto Card = deck::SKCardFromStr;
  poker_engine::Node<5, TestType> game(0, 2, 1, 0, true, false, 11);
  // Preflop fold to big blind
  game.Apply(poker_engine::Action::kFold);
  game.Apply(poker_engine::Action::kFold);
  game.Apply(poker_engine::Action::kFold);
  game.Apply(poker_engine::Action::kFold);
  // Award Pot
  game.AwardPot(game.multi_run_);
  REQUIRE(game.stack(0) == 11);
  REQUIRE(game.stack(1) == 10);
  REQUIRE(game.stack(2) == 12);
  REQUIRE(game.stack(3) == 11);
  REQUIRE(game.stack(4) == 11);
  for (uint8_t i = 0; i < 5; ++i) {
    REQUIRE(game.bets(i) == 0);
  }
  REQUIRE(game.pot() == 0);

  // Hand 2
  game.NewHand();
  // Preflop
  game.Apply(poker_engine::Action::kFold);
  game.Apply(poker_engine::Action::kFold);
  game.Apply(poker_engine::Action::kFold);
  game.Apply(poker_engine::Action::kCheckCall);
  game.Apply(poker_engine::Action::kCheckCall);
  // Flop
  game.Apply(poker_engine::Action::kCheckCall);
  game.Apply(poker_engine::Action::kCheckCall);
  // Turn
  game.Apply(poker_engine::Action::kCheckCall);
  game.Apply(poker_engine::Action::kCheckCall);
  // River
  game.Apply(poker_engine::Action::kCheckCall);
  game.Apply(poker_engine::Action::kCheckCall);
  // Award Pot
  REQUIRE_THROWS(game.AwardPot(game.multi_run_));
  uint8_t board_0[1][5] = {{Card("8c"), Card("9c"), Card("Ts"), Card("Js"),
                            Card("Qs")}};
  uint8_t hands_0[5][2] = {{}, {}, {0, 0}, {Card("2s"), Card("2h")}, {}};
  game.AwardPot(game.multi_run_, hands_0, board_0);
  REQUIRE(game.stack(0) == 11);
  REQUIRE(game.stack(1) == 10);
  REQUIRE(game.stack(2) == 10);
  REQUIRE(game.stack(3) == 13);
  REQUIRE(game.stack(4) == 11);
  for (uint8_t i = 0; i < 5; ++i) {
    REQUIRE(game.bets(i) == 0);
  }
  REQUIRE(game.pot() == 0);

  // Hand 3
  game.NewHand();
  // Preflop everyone all in
  game.Apply(poker_engine::Action::kAllIn);
  game.Apply(poker_engine::Action::kAllIn);
  game.Apply(poker_engine::Action::kAllIn);
  game.Apply(poker_engine::Action::kAllIn);
  game.Apply(poker_engine::Action::kAllIn);
  // Award Pot
  REQUIRE_THROWS(game.AwardPot(game.multi_run_));
  uint8_t board_1[3][5] = {{Card("8c"), Card("9c"), Card("Tc"), Card("Jc"),
                            Card("2d")}, {Card("2c"), Card("3c"), Card("4c"),
                            Card("5d"), Card("6d")}, {Card("8s"), Card("9s"),
                            Card("Ts"), Card("Qs"), Card("2h")}};
  uint8_t hands_1[5][2] = {{Card("6h"), Card("7h")}, {Card("Ac"), Card("Kc")},
                           {Card("As"), Card("Ks")}, {Card("5s"), Card("7s")},
                           {Card("Qd"), Card("8d")}};
  game.AwardPot(game.multi_run_, hands_1, board_1, 3);
  REQUIRE((game.stack(0) == 1 || game.stack(0) == 0));
  REQUIRE(game.stack(1) == 33);
  REQUIRE(game.stack(2) == 17);
  REQUIRE((game.stack(3) == 3 || game.stack(3) == 4));
  REQUIRE(game.stack(4) == 1);
  uint32_t stack_sum = 0;
  for (uint8_t i = 0; i < 5; ++i) {
    stack_sum += game.stack(i);
  }
  REQUIRE(stack_sum == 55);
  for (uint8_t i = 0; i < 5; ++i) {
    REQUIRE(game.bets(i) == 0);
  }
  REQUIRE(game.pot() == 0);
}  // TEMPLATE_TEST_CASE "awardpot multi run"

TEMPLATE_TEST_CASE("awardpot multi run rake no flop no drop",
                   "[poker_engine][node]", double, utils::Fraction) {
  auto Card = deck::SKCardFromStr;
  TestType rake{1};
  rake /= 20;
  poker_engine::Node<5, TestType> game(0, 2, 1, 0, true, false, 11, 0, rake, 0,
                                       true);
  // Preflop fold to big blind
  game.Apply(poker_engine::Action::kFold);
  game.Apply(poker_engine::Action::kFold);
  game.Apply(poker_engine::Action::kFold);
  game.Apply(poker_engine::Action::kFold);
  // Award Pot
  game.AwardPot(game.multi_run_);
  REQUIRE(game.stack(0) == 11);
  REQUIRE(game.stack(1) == 10);
  REQUIRE(game.stack(2) == 12);
  REQUIRE(game.stack(3) == 11);
  REQUIRE(game.stack(4) == 11);
  for (uint8_t i = 0; i < 5; ++i) {
    REQUIRE(game.bets(i) == 0);
  }
  REQUIRE(game.pot() == 0);

  // Hand 2
  game.NewHand();
  // Preflop
  game.Apply(poker_engine::Action::kFold);
  game.Apply(poker_engine::Action::kFold);
  game.Apply(poker_engine::Action::kFold);
  game.Apply(poker_engine::Action::kCheckCall);
  game.Apply(poker_engine::Action::kCheckCall);
  // Flop
  game.Apply(poker_engine::Action::kCheckCall);
  game.Apply(poker_engine::Action::kCheckCall);
  // Turn
  game.Apply(poker_engine::Action::kCheckCall);
  game.Apply(poker_engine::Action::kCheckCall);
  // River
  game.Apply(poker_engine::Action::kCheckCall);
  game.Apply(poker_engine::Action::kCheckCall);
  // Award Pot
  REQUIRE_THROWS(game.AwardPot(game.multi_run_));
  uint8_t board_0[1][5] = {{Card("8c"), Card("9c"), Card("Ts"), Card("Js"),
                            Card("Qs")}};
  uint8_t hands_0[5][2] = {{}, {}, {0, 0}, {Card("2s"), Card("2h")}, {}};
  game.AwardPot(game.multi_run_, hands_0, board_0);
  REQUIRE(game.stack(0) == 11);
  REQUIRE(game.stack(1) == 10);
  REQUIRE(game.stack(2) == 10);
  REQUIRE(game.stack(3) == 13);
  REQUIRE(game.stack(4) == 11);
  for (uint8_t i = 0; i < 5; ++i) {
    REQUIRE(game.bets(i) == 0);
  }
  REQUIRE(game.pot() == 0);

  // Hand 3
  game.NewHand();
  // Preflop everyone all in
  game.Apply(poker_engine::Action::kAllIn);
  game.Apply(poker_engine::Action::kAllIn);
  game.Apply(poker_engine::Action::kAllIn);
  game.Apply(poker_engine::Action::kAllIn);
  game.Apply(poker_engine::Action::kAllIn);
  // Award Pot
  REQUIRE_THROWS(game.AwardPot(game.multi_run_));
  uint8_t board_1[3][5] = {{Card("8c"), Card("9c"), Card("Tc"), Card("Jc"),
                            Card("2d")}, {Card("2c"), Card("3c"), Card("4c"),
                            Card("5d"), Card("6d")}, {Card("8s"), Card("9s"),
                            Card("Ts"), Card("Qs"), Card("2h")}};
  uint8_t hands_1[5][2] = {{Card("6h"), Card("7h")}, {Card("Ac"), Card("Kc")},
                           {Card("As"), Card("Ks")}, {Card("5s"), Card("7s")},
                           {Card("Qd"), Card("8d")}};
  game.AwardPot(game.multi_run_, hands_1, board_1, 3);
  REQUIRE(game.stack(0) == 0);
  REQUIRE(game.stack(1) == 32);
  REQUIRE(game.stack(2) == 16);
  REQUIRE(game.stack(3) == 3);
  REQUIRE(game.stack(4) == 1);
  uint32_t stack_sum = 0;
  for (uint8_t i = 0; i < 5; ++i) {
    stack_sum += game.stack(i);
  }
  REQUIRE(stack_sum == 52);
  for (uint8_t i = 0; i < 5; ++i) {
    REQUIRE(game.bets(i) == 0);
  }
  REQUIRE(game.pot() == 0);
}  // TEMPLATE_TEST_CASE "awardpot multi run rake no flop no drop"

TEMPLATE_TEST_CASE("awardpot single run rake rake cap", "[poker_engine][node]",
                   double, utils::Fraction) {
  auto Card = deck::SKCardFromStr;
  TestType rake{1};
  rake /= 20;
  poker_engine::Node<4, TestType> game(0, 100, 50, 100, true, false, 10000, 0,
                                       rake, 100, false);
  game.Apply(poker_engine::Action::kAllIn);
  REQUIRE(game.CanCheckCall() == false);
  game.Apply(poker_engine::Action::kAllIn);
  REQUIRE(game.CanCheckCall() == false);
  game.Apply(poker_engine::Action::kAllIn);
  REQUIRE(game.CanCheckCall() == false);
  game.Apply(poker_engine::Action::kAllIn);
  REQUIRE_THROWS(game.AwardPot(game.single_run_));
  uint8_t board[5] = {Card("8c"), Card("9c"), Card("Ts"), Card("Js"),
                      Card("Qs")};
  uint8_t hands[4][2] = {{Card("Ad"), Card("Kd")}, {Card("Ac"), Card("Kc")},
                         {Card("Ah"), Card("Kh")}, {Card("2s"), Card("2h")}};
  game.AwardPot(game.single_run_, hands, board);
  REQUIRE((game.stack(0) == 13499 || game.stack(0) == 13500));
  REQUIRE((game.stack(1) == 13499 || game.stack(1) == 13500));
  REQUIRE(game.stack(2) == 12901);
  REQUIRE(game.stack(3) == 0);
  REQUIRE(game.stack(0) + game.stack(1) + game.stack(2) + game.stack(3) ==
          39900);
  for (uint8_t i = 0; i < 4; ++i) {
    REQUIRE(game.bets(i) == 0);
  }
  REQUIRE(game.pot() == 0);
}  // TEMPLATE_TEST_CASE "awardpot single run rake rake cap"

TEMPLATE_TEST_CASE("bb ante with change", "[poker_engine][node]",
                   double, utils::Fraction) {
  poker_engine::Node<3, TestType> game(0, 100, 50, 100, true, false, 250);
  REQUIRE(game.players_all_in() == 1);
  REQUIRE(game.bets(0) == 83);
  REQUIRE(game.bets(1) == 133);
  REQUIRE(game.bets(2) == 84);
  REQUIRE(game.stack(0) == 250);
  REQUIRE(game.stack(1) == 200);
  REQUIRE(game.stack(2) == 0);
  REQUIRE(game.pot() == 300);

  // Preflop

  // Player 0 calls
  REQUIRE(game.CanCheckCall());
  game.Apply(poker_engine::Action::kCheckCall);
  REQUIRE(game.bets(0) == 183);
  REQUIRE(game.bets(1) == 133);
  REQUIRE(game.bets(2) == 84);
  REQUIRE(game.stack(0) == 150);
  REQUIRE(game.stack(1) == 200);
  REQUIRE(game.stack(2) == 0);
  REQUIRE(game.pot() == 400);

  // Player 1 calls
  REQUIRE(game.CanCheckCall());
  game.Apply(poker_engine::Action::kCheckCall);
  REQUIRE(game.bets(0) == 183);
  REQUIRE(game.bets(1) == 183);
  REQUIRE(game.bets(2) == 84);
  REQUIRE(game.stack(0) == 150);
  REQUIRE(game.stack(1) == 150);
  REQUIRE(game.stack(2) == 0);
  REQUIRE(game.pot() == 450);

  REQUIRE(game.round() == poker_engine::Round::kFlop);
}  // TEMPLATE_TEST_CASE "bb ante with change"
