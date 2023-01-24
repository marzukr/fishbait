#include "catch2/catch.hpp"
#include "poker/card_utils.h"
#include "poker/definitions.h"
#include "poker/node.h"
#include "utils/array.h"
#include "utils/fraction.h"

TEST_CASE("Triton cash game first 3 hands", "[poker][node]") {
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
  auto Card = fishbait::ISOCardFromStr;
  fishbait::Node<8, double> triton(
      fishbait::FilledArray<fishbait::Chips, 8>(500000), 0, 4000, 2000, 500,
      true, true);
  REQUIRE_THROWS(triton.NewHand());

  // Verify that the constructor worked correctly
  REQUIRE(triton.big_blind() == 4000);
  REQUIRE(triton.small_blind() == 2000);
  REQUIRE(triton.ante() == 500);
  REQUIRE(triton.big_blind_ante() == true);
  REQUIRE(triton.blind_before_ante() == true);
  REQUIRE(triton.button() == 0);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == fishbait::Round::kPreFlop);
  REQUIRE(triton.acting_player() == triton.kChancePlayer);
  REQUIRE(triton == triton);
  REQUIRE(triton.players() == 8);
  fishbait::Node triton_cp = triton;
  REQUIRE(triton_cp == triton);
  REQUIRE(triton_cp.big_blind() == 4000);
  REQUIRE(triton_cp.small_blind() == 2000);
  REQUIRE(triton_cp.ante() == 500);
  REQUIRE(triton_cp.big_blind_ante() == true);
  REQUIRE(triton_cp.blind_before_ante() == true);
  REQUIRE(triton_cp.button() == 0);
  REQUIRE(triton_cp.in_progress() == true);
  REQUIRE(triton_cp.round() == fishbait::Round::kPreFlop);
  REQUIRE(triton_cp.acting_player() == triton.kChancePlayer);
  REQUIRE(triton_cp.players() == 8);

  triton.Deal();
  triton.ProceedPlay();
  REQUIRE(triton.cycled() == 0);
  REQUIRE(triton.acting_player() == 3);
  REQUIRE(triton.folded(3) == false);
  REQUIRE(triton.players_left() == 8);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 10000);
  REQUIRE(triton.bets(3) == 500);
  REQUIRE(triton.stack(3) == 500000);
  REQUIRE(triton_cp != triton);

  /* ------------------------------- Preflop -------------------------------- */

  // Test Tan Xuan's action (fold)
  REQUIRE(triton.PlayerIndex(3) == 3);
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.CanBet(4000) == false);
  REQUIRE(triton.CanBet(7999) == false);
  REQUIRE(triton.CanBet(8000) == true);
  REQUIRE(triton.CanFold());
  REQUIRE(triton.Apply(fishbait::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == fishbait::Round::kPreFlop);
  REQUIRE(triton.cycled() == 1);
  REQUIRE(triton.acting_player() == 4);
  REQUIRE(triton.folded(3) == true);
  REQUIRE(triton.players_left() == 7);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 10000);
  REQUIRE(triton.bets(3) == 500);
  REQUIRE(triton.stack(3) == 500000);
  REQUIRE_THROWS(triton.NewHand());
  REQUIRE_THROWS(triton.ProceedPlay());
  REQUIRE_THROWS(triton.Deal());

  // Test Paul Phua's action (fold)
  REQUIRE(triton.PlayerIndex(4) == 4);
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.CanFold());
  REQUIRE(triton.Apply(fishbait::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == fishbait::Round::kPreFlop);
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
  REQUIRE(triton.CanFold());
  REQUIRE(triton.Apply(fishbait::Action::kBet, 10000) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == fishbait::Round::kPreFlop);
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
  REQUIRE(triton.CanFold());
  REQUIRE(triton.NeededToCall() == 10000);
  REQUIRE(triton.Apply(fishbait::Action::kCheckCall) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == fishbait::Round::kPreFlop);
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
  REQUIRE(triton.CanFold());
  REQUIRE(triton.Apply(fishbait::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == fishbait::Round::kPreFlop);
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
  REQUIRE(triton.CanFold());
  REQUIRE(triton.Apply(fishbait::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == fishbait::Round::kPreFlop);
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
  REQUIRE(triton.CanFold());
  REQUIRE(triton.Apply(fishbait::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == fishbait::Round::kPreFlop);
  REQUIRE(triton.cycled() == 7);
  REQUIRE(triton.acting_player() == 2);
  REQUIRE(triton.folded(1) == true);
  REQUIRE(triton.players_left() == 3);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 30000);
  REQUIRE(triton.bets(1) == 2500);
  REQUIRE(triton.stack(1) == 498000);

  // Test Kuznetsov's action (call)
  REQUIRE(triton.PlayerIndex(2) == 2);
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.CanFold());
  REQUIRE(triton.NeededToCall() == 6000);
  REQUIRE(triton.Apply(fishbait::Action::kCheckCall) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == fishbait::Round::kFlop);

  /* -------------------------------- Flop ---------------------------------- */

  REQUIRE(triton.round() == fishbait::Round::kFlop);
  REQUIRE_THROWS(triton.Apply(fishbait::Action::kCheckCall));
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.acting_player() == triton.kChancePlayer);
  triton.Deal();
  triton.ProceedPlay();
  REQUIRE(triton.cycled() == 1);
  REQUIRE(triton.acting_player() == 2);
  REQUIRE(triton.folded(2) == false);
  REQUIRE(triton.players_left() == 3);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 36000);
  REQUIRE(triton.bets(2) == 10500);
  REQUIRE(triton.stack(2) == 486000);

  // Test Kuznetsov's action (check)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.CanFold() == false);
  REQUIRE(triton.NeededToCall() == 0);
  REQUIRE(triton.Apply(fishbait::Action::kCheckCall) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == fishbait::Round::kFlop);
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
  REQUIRE(triton.CanFold() == false);
  REQUIRE(triton.Apply(fishbait::Action::kBet, 30000) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == fishbait::Round::kFlop);
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
  REQUIRE(triton.CanFold());
  REQUIRE(triton.NeededToCall() == 30000);
  REQUIRE(triton.Apply(fishbait::Action::kCheckCall) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == fishbait::Round::kFlop);
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
  REQUIRE(triton.CanFold());
  REQUIRE(triton.Apply(fishbait::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == fishbait::Round::kTurn);

  /* -------------------------------- Turn ---------------------------------- */

  REQUIRE(triton.round() == fishbait::Round::kTurn);
  REQUIRE_THROWS(triton.Apply(fishbait::Action::kCheckCall));
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.acting_player() == triton.kChancePlayer);
  triton.Deal();
  triton.ProceedPlay();
  REQUIRE(triton.cycled() == 4);
  REQUIRE(triton.acting_player() == 5);
  REQUIRE(triton.folded(2) == true);
  REQUIRE(triton.players_left() == 2);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 96000);
  REQUIRE(triton.bets(2) == 10500);
  REQUIRE(triton.stack(2) == 486000);

  // Test Tony G's action (bet 100000)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.CanBet(100000));
  REQUIRE(triton.CanFold() == false);
  REQUIRE(triton.Apply(fishbait::Action::kBet, 100000) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == fishbait::Round::kTurn);
  REQUIRE(triton.cycled() == 5);
  REQUIRE(triton.acting_player() == 6);
  REQUIRE(triton.folded(5) == false);
  REQUIRE(triton.players_left() == 2);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 196000);
  REQUIRE(triton.bets(5) == 140500);
  REQUIRE(triton.stack(5) == 360000);

  fishbait::MultiBoardArray<fishbait::ISO_Card, 1> board_throw;
  REQUIRE_THROWS(triton.AwardPot(triton.same_stack_no_rake_));
  REQUIRE_THROWS(triton.AwardPot(triton.single_run_));
  REQUIRE_THROWS(triton.AwardPot(triton.multi_run_, board_throw));

  // Test Haxton's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.CanFold());
  REQUIRE(triton.Apply(fishbait::Action::kFold) == false);
  REQUIRE(triton.in_progress() == false);
  REQUIRE(triton.round() == fishbait::Round::kTurn);
  REQUIRE(triton.cycled() == 12);
  REQUIRE(triton.acting_player() == 5);
  REQUIRE(triton.folded(6) == true);
  REQUIRE(triton.players_left() == 1);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 196000);
  REQUIRE(triton.bets(6) == 40500);
  REQUIRE(triton.stack(6) == 460000);

  REQUIRE_THROWS(triton.Apply(fishbait::Action::kFold));
  REQUIRE_THROWS(triton.NewHand());
  REQUIRE_THROWS(triton.ProceedPlay());
  REQUIRE_THROWS(triton.Deal());

  // fishbait::HandArray<fishbait::ISO_Card, 8> cards0 = {{
  //     {/*     Loeliger     */}, {/*      Cates       */},
  //     {Card("Qh"), Card("Js")}, {Card("Td"), Card("5s")},
  //     {Card("Kc"), Card("8h")}, {Card("Jc"), Card("7h")},
  //     {Card("Kh"), Card("Jh")}, {Card("7s"), Card("2d")}
  // }};
  fishbait::MultiBoardArray<fishbait::ISO_Card, 1> board0 = {{{Card("8c"),
      Card("6c"), Card("Ks"), Card("5c"), Card("4d")}}};
  // triton.SetHands(cards0);
  // triton.SetBoard(board0[0]);
  // REQUIRE(triton.PlayerCards(2) == std::array{Card("Qh"), Card("Js"),
  //     Card("8c"), Card("6c"), Card("Ks"), Card("5c"), Card("4d")});
  triton.AwardPot(triton.single_run_);

  REQUIRE_THROWS(triton.Apply(fishbait::Action::kFold));
  REQUIRE_THROWS(triton.ProceedPlay());
  REQUIRE_THROWS(triton.Deal());
  REQUIRE_THROWS(triton.AwardPot(triton.same_stack_no_rake_));
  REQUIRE_THROWS(triton.AwardPot(triton.single_run_));
  REQUIRE_THROWS(triton.AwardPot(triton.multi_run_, board0));

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
  REQUIRE(triton.round() == fishbait::Round::kPreFlop);
  REQUIRE(triton.acting_player() == triton.kChancePlayer);
  triton.Deal();
  triton.ProceedPlay();

  REQUIRE(triton.cycled() == 0);
  REQUIRE(triton.acting_player() == 4);
  REQUIRE(triton.players_left() == 8);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 10000);
  REQUIRE(triton.button() == 1);

  // Test Paul Phua's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.CanFold());
  REQUIRE(triton.Apply(fishbait::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == fishbait::Round::kPreFlop);
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
  REQUIRE(triton.CanFold());
  REQUIRE(triton.Apply(fishbait::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == fishbait::Round::kPreFlop);
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
  REQUIRE(triton.CanFold());
  REQUIRE(triton.Apply(fishbait::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == fishbait::Round::kPreFlop);
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
  REQUIRE(triton.CanFold());
  REQUIRE(triton.Apply(fishbait::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == fishbait::Round::kPreFlop);
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
  REQUIRE(triton.CanFold());
  REQUIRE(triton.Apply(fishbait::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == fishbait::Round::kPreFlop);
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
  REQUIRE(triton.CanFold());
  REQUIRE(triton.Apply(fishbait::Action::kBet, 12000) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == fishbait::Round::kPreFlop);
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
  REQUIRE(triton.CanFold());
  REQUIRE(triton.Apply(fishbait::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == fishbait::Round::kPreFlop);
  REQUIRE(triton.cycled() == 7);
  REQUIRE(triton.acting_player() == 3);
  REQUIRE(triton.folded(2) == true);
  REQUIRE(triton.players_left() == 2);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 22000);
  REQUIRE(triton.bets(2) == 2500);
  REQUIRE(triton.stack(2) == 484000);

  REQUIRE_THROWS(triton.AwardPot(triton.single_run_));
  REQUIRE_THROWS(triton.AwardPot(triton.multi_run_, board_throw));
  REQUIRE_THROWS(triton.AwardPot(triton.same_stack_no_rake_));

  // Test Tan Xuan's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.CanFold());
  REQUIRE(triton.Apply(fishbait::Action::kFold) == false);
  REQUIRE(triton.in_progress() == false);
  REQUIRE(triton.round() == fishbait::Round::kPreFlop);
  REQUIRE(triton.cycled() == 13);
  REQUIRE(triton.acting_player() == 1);
  REQUIRE(triton.folded(3) == true);
  REQUIRE(triton.players_left() == 1);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 22000);
  REQUIRE(triton.bets(3) == 4500);
  REQUIRE(triton.stack(3) == 492000);

  REQUIRE_THROWS(triton.Apply(fishbait::Action::kFold));
  REQUIRE_THROWS(triton.NewHand());
  REQUIRE_THROWS(triton.ProceedPlay());
  REQUIRE_THROWS(triton.Deal());

  fishbait::HandArray<fishbait::ISO_Card, 8> cards1 = {{
      {Card("Qd"), Card("6c")}, {Card("Ah"), Card("8d")},
      {/*     Kuznetsov    */}, {Card("8c"), Card("6h")},
      {Card("Kc"), Card("4s")}, {Card("7c"), Card("6s")},
      {Card("7s"), Card("3s")}, {Card("Tc"), Card("8h")}
  }};
  fishbait::MultiBoardArray<fishbait::ISO_Card, 1> board1 = {{/*Flop1*/
      /*Flop2*/ /*Flop3*/ /*Turn*/ /*River*/}};
  triton.SetHands(cards1);
  triton.SetBoard(board1[0]);
  std::array player_cards_0 = triton.PlayerCards(0);
  REQUIRE(player_cards_0[0] == Card("Qd"));
  REQUIRE(player_cards_0[1] == Card("6c"));
  triton.AwardPot(triton.single_run_);

  REQUIRE_THROWS(triton.Apply(fishbait::Action::kFold));
  REQUIRE_THROWS(triton.Deal());
  REQUIRE_THROWS(triton.ProceedPlay());
  REQUIRE_THROWS(triton.AwardPot(triton.single_run_));
  REQUIRE_THROWS(triton.AwardPot(triton.multi_run_, board1));
  REQUIRE_THROWS(triton.AwardPot(triton.same_stack_no_rake_));

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
  REQUIRE(triton.round() == fishbait::Round::kPreFlop);
  REQUIRE(triton.acting_player() == triton.kChancePlayer);
  REQUIRE_THROWS(triton.Apply(fishbait::Action::kFold));
  triton.ResetDeck();
  triton.ProceedPlay();
  REQUIRE_THROWS(triton.Deal());

  REQUIRE(triton.cycled() == 0);
  REQUIRE(triton.acting_player() == 5);
  REQUIRE(triton.players_left() == 8);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 10000);
  REQUIRE(triton.button() == 2);

  // Test Tony G's action (fold)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.CanFold());
  REQUIRE(triton.Apply(fishbait::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == fishbait::Round::kPreFlop);
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
  REQUIRE(triton.CanFold());
  REQUIRE(triton.Apply(fishbait::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == fishbait::Round::kPreFlop);
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
  REQUIRE(triton.CanFold());
  REQUIRE(triton.Apply(fishbait::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == fishbait::Round::kPreFlop);
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
  REQUIRE(triton.CanFold());
  REQUIRE(triton.Apply(fishbait::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == fishbait::Round::kPreFlop);
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
  REQUIRE(triton.CanFold());
  REQUIRE(triton.Apply(fishbait::Action::kBet, 12000) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == fishbait::Round::kPreFlop);
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
  REQUIRE(triton.CanFold());
  REQUIRE(triton.Apply(fishbait::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == fishbait::Round::kPreFlop);
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
  REQUIRE(triton.CanFold());
  REQUIRE(triton.Apply(fishbait::Action::kFold) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == fishbait::Round::kPreFlop);
  REQUIRE(triton.cycled() == 7);
  REQUIRE(triton.acting_player() == 4);
  REQUIRE(triton.folded(3) == true);
  REQUIRE(triton.players_left() == 2);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 22000);
  REQUIRE(triton.bets(3) == 2500);
  REQUIRE(triton.stack(3) == 490000);

  // Test Paul Phua's action (call)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.CanFold());
  REQUIRE(triton.NeededToCall() == 8000);
  REQUIRE(triton.Apply(fishbait::Action::kCheckCall) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == fishbait::Round::kFlop);

  /* -------------------------------- Flop ---------------------------------- */

  REQUIRE(triton.round() == fishbait::Round::kFlop);
  REQUIRE_THROWS(triton.Apply(fishbait::Action::kCheckCall));
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.acting_player() == triton.kChancePlayer);
  REQUIRE_THROWS(triton.Deal());
  triton.ResetDeck();
  REQUIRE_THROWS(triton.Deal());
  triton.ProceedPlay();
  REQUIRE(triton.cycled() == 1);
  REQUIRE(triton.acting_player() == 4);
  REQUIRE(triton.folded(4) == false);
  REQUIRE(triton.players_left() == 2);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 30000);
  REQUIRE(triton.bets(4) == 12500);
  REQUIRE(triton.stack(4) == 484000);

  // Test Paul Phua's action (check)
  REQUIRE(triton.Rotation() == 0);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.CanFold() == false);
  REQUIRE(triton.NeededToCall() == 0);
  REQUIRE(triton.Apply(fishbait::Action::kCheckCall) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == fishbait::Round::kFlop);
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
  REQUIRE(triton.CanFold() == false);
  REQUIRE(triton.Apply(fishbait::Action::kBet, 10000) == true);
  REQUIRE(triton.in_progress() == true);
  REQUIRE(triton.round() == fishbait::Round::kFlop);
  REQUIRE(triton.cycled() == 9);
  REQUIRE(triton.acting_player() == 4);
  REQUIRE(triton.folded(1) == false);
  REQUIRE(triton.players_left() == 2);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 40000);
  REQUIRE(triton.bets(1) == 22500);
  REQUIRE(triton.stack(1) == 486000);

  REQUIRE_THROWS(triton.AwardPot(triton.single_run_));
  REQUIRE_THROWS(triton.AwardPot(triton.same_stack_no_rake_));
  REQUIRE_THROWS(triton.AwardPot(triton.multi_run_, board_throw));

  // Test Paul Phua's action (fold)
  REQUIRE(triton.Rotation() == 1);
  REQUIRE(triton.CanCheckCall() == true);
  REQUIRE(triton.CanFold());
  REQUIRE(triton.Apply(fishbait::Action::kFold) == false);
  REQUIRE(triton.in_progress() == false);
  REQUIRE(triton.round() == fishbait::Round::kFlop);
  REQUIRE(triton.cycled() == 14);
  REQUIRE(triton.acting_player() == 1);
  REQUIRE(triton.folded(4) == true);
  REQUIRE(triton.players_left() == 1);
  REQUIRE(triton.players_all_in() == 0);
  REQUIRE(triton.pot() == 40000);

  // fishbait::SK_Card cards2[8][2] =
  //     {{Card("Kc"), Card("Th")}, {Card("Ah"), Card("9d")},
  //      {Card("As"), Card("5d")}, {Card("4h"), Card("2d")},
  //      {Card("Qd"), Card("8h")}, {Card("Tc"), Card("4s")},
  //      {Card("5s"), Card("4d")}, {Card("9c"), Card("3d")}};
  // fishbait::SK_Card board2[5] = {Card("Kd"), Card("3s"), Card("7d") /*Turn*/
  //                                /*River*/};
  REQUIRE_THROWS(triton.Apply(fishbait::Action::kAllIn));
  REQUIRE_THROWS(triton.NewHand());
  REQUIRE_THROWS(triton.ProceedPlay());
  REQUIRE_THROWS(triton.Deal());
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
  REQUIRE_THROWS(triton.Apply(fishbait::Action::kFold));
  REQUIRE_THROWS(triton.AwardPot(triton.single_run_));
  REQUIRE_THROWS(triton.AwardPot(triton.same_stack_no_rake_));
  REQUIRE_THROWS(triton.AwardPot(triton.multi_run_, board_throw));
  triton.ResetDeck();
  triton.NewHand();
  REQUIRE_NOTHROW(triton.Deal());
  REQUIRE_NOTHROW(triton.ProceedPlay());
}  // TEST_CASE "Triton cash game first 3 hands"

TEST_CASE("heads up big blind ante big blind first", "[poker][node]") {
  auto Card = fishbait::ISOCardFromStr;
  fishbait::Node<2, fishbait::Fraction> heads_up({100, 100}, 0, 4, 2, 2, true,
                                                 true);
  REQUIRE_THROWS(heads_up.NewHand());

  // Verify that the constructor worked correctly
  REQUIRE(heads_up.big_blind() == 4);
  REQUIRE(heads_up.small_blind() == 2);
  REQUIRE(heads_up.ante() == 2);
  REQUIRE(heads_up.big_blind_ante() == true);
  REQUIRE(heads_up.blind_before_ante() == true);
  REQUIRE(heads_up.button() == 0);
  REQUIRE(heads_up.in_progress() == true);
  REQUIRE(heads_up.round() == fishbait::Round::kPreFlop);
  REQUIRE(heads_up.acting_player() == heads_up.kChancePlayer);
  REQUIRE(heads_up == heads_up);
  REQUIRE(heads_up.players() == 2);
  fishbait::Node heads_up_copy(heads_up);
  REQUIRE(heads_up == heads_up_copy);
  heads_up.Deal();
  heads_up.ProceedPlay();

  REQUIRE(heads_up.cycled() == 0);
  REQUIRE(heads_up.acting_player() == 0);
  REQUIRE(heads_up.folded(0) == false);
  REQUIRE(heads_up.players_left() == 2);
  REQUIRE(heads_up.players_all_in() == 0);
  REQUIRE(heads_up.pot() == 10);
  REQUIRE(heads_up.bets(0) == 4);
  REQUIRE(heads_up.stack(0) == 98);
  REQUIRE(heads_up != heads_up_copy);

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

  /* ------------------------------ Preflop --------------------------------- */

  // Test John's action (all in)
  REQUIRE(heads_up.Rotation() == 0);
  REQUIRE(heads_up.CanCheckCall() == true);
  REQUIRE(heads_up.CanBet(5) == false);
  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kBet, 5));
  REQUIRE(heads_up.CanBet(6) == true);
  REQUIRE(heads_up.CanBet(97) == true);
  REQUIRE(heads_up.CanBet(98) == false);
  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kBet, 98));
  REQUIRE(heads_up.CanFold());
  REQUIRE(heads_up.Apply(fishbait::Action::kAllIn) == true);
  REQUIRE(heads_up.in_progress() == true);
  REQUIRE(heads_up.round() == fishbait::Round::kPreFlop);
  REQUIRE(heads_up.cycled() == 1);
  REQUIRE(heads_up.acting_player() == 1);
  REQUIRE(heads_up.folded(0) == false);
  REQUIRE(heads_up.players_left() == 2);
  REQUIRE(heads_up.players_all_in() == 1);
  REQUIRE(heads_up.pot() == 108);
  REQUIRE(heads_up.bets(0) == 102);
  REQUIRE(heads_up.stack(0) == 0);
  REQUIRE_THROWS(heads_up.NewHand());
  REQUIRE_THROWS(heads_up.ProceedPlay());
  REQUIRE_THROWS(heads_up.Deal());

  fishbait::MultiBoardArray<fishbait::ISO_Card, 1> board_throw;
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.single_run_));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.multi_run_, board_throw));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.same_stack_no_rake_));

  // Test Mary's action (all in)
  REQUIRE(heads_up.Rotation() == 0);
  REQUIRE(heads_up.CanCheckCall() == false);
  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kCheckCall));
  REQUIRE(heads_up.CanBet(3) == false);
  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kBet, 3));
  REQUIRE(heads_up.CanBet(4) == false);
  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kBet, 4));
  REQUIRE(heads_up.CanBet(91) == false);
  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kBet, 91));
  REQUIRE(heads_up.CanBet(92) == false);
  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kBet, 92));
  REQUIRE(heads_up.CanFold());
  REQUIRE(heads_up.Apply(fishbait::Action::kAllIn) == true);
  REQUIRE(heads_up.in_progress() == true);
  REQUIRE(heads_up.round() == fishbait::Round::kFlop);
  REQUIRE(heads_up.folded(1) == false);
  REQUIRE(heads_up.players_left() == 2);
  REQUIRE(heads_up.players_all_in() == 2);

  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kFold));
  REQUIRE_THROWS(heads_up.NewHand());

  /* -------------------------------- Flop ---------------------------------- */
  heads_up.Deal();
  heads_up.ProceedPlay();
  /* -------------------------------- Turn ---------------------------------- */
  heads_up.Deal();
  heads_up.ProceedPlay();
  /* -------------------------------- River --------------------------------- */
  heads_up.Deal();
  heads_up.ProceedPlay();

  // fishbait::HandArray<fishbait::ISO_Card, 2> cards0 = {{{Card("7c"),
  //     Card("2h")}, {Card("Ah"), Card("As")}}};
  fishbait::MultiBoardArray<fishbait::ISO_Card, 1> board0 = {{{Card("9c"),
      Card("6c"), Card("Ks"), Card("5c"), Card("4d")}}};
  heads_up.SetBoard(board0[0]);
  heads_up.SetHand(0, {Card("7c"), Card("2h")});
  heads_up.SetHand(1, {Card("Ah"), Card("As")});
  REQUIRE(heads_up.PlayerCards(1) == std::array{Card("Ah"), Card("As"),
      Card("9c"), Card("6c"), Card("Ks"), Card("5c"), Card("4d")});
  heads_up.AwardPot(heads_up.single_run_);

  // Check AwardPot results
  REQUIRE(heads_up.pot() == 0);
  REQUIRE(heads_up.bets(0) == 0);
  REQUIRE(heads_up.bets(1) == 0);
  REQUIRE(heads_up.stack(0) == 4);
  REQUIRE(heads_up.stack(1) == 196);

  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kFold));
  REQUIRE_THROWS(heads_up.Deal());
  REQUIRE_THROWS(heads_up.ProceedPlay());
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.single_run_));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.multi_run_, board0));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.same_stack_no_rake_));

  /* ------------------------------------------------------------------------ */
  /* ------------------------------- Hand 2 --------------------------------- */
  /* ------------------------------------------------------------------------ */

  heads_up.NewHand();

  REQUIRE(heads_up.in_progress() == true);
  REQUIRE(heads_up.round() == fishbait::Round::kPreFlop);
  REQUIRE(heads_up.acting_player() == heads_up.kChancePlayer);
  REQUIRE_THROWS(heads_up.Deal());
  heads_up.ResetDeck();
  heads_up.Deal();
  heads_up.ProceedPlay();

  REQUIRE(heads_up.cycled() == 0);
  REQUIRE(heads_up.acting_player() == 1);
  REQUIRE(heads_up.players_left() == 2);
  REQUIRE(heads_up.players_all_in() == 1);
  REQUIRE(heads_up.pot() == 6);
  REQUIRE(heads_up.button() == 1);

  REQUIRE_THROWS(heads_up.AwardPot(heads_up.single_run_));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.same_stack_no_rake_));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.multi_run_, board_throw));

  /* ------------------------------ Preflop --------------------------------- */

  // Test Mary's action (call)
  REQUIRE(heads_up.Rotation() == 0);
  REQUIRE(heads_up.CanCheckCall() == true);
  REQUIRE(heads_up.CanFold());
  REQUIRE(heads_up.Apply(fishbait::Action::kCheckCall) == true);
  REQUIRE(heads_up.in_progress() == true);
  REQUIRE(heads_up.round() == fishbait::Round::kFlop);
  REQUIRE(heads_up.folded(1) == false);
  REQUIRE(heads_up.players_left() == 2);
  REQUIRE(heads_up.players_all_in() == 1);

  /* -------------------------------- Flop ---------------------------------- */
  heads_up.Deal();
  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kFold));
  REQUIRE_THROWS(heads_up.NewHand());
  heads_up.ProceedPlay();
  /* -------------------------------- Turn ---------------------------------- */
  heads_up.Deal();
  heads_up.ProceedPlay();
  /* -------------------------------- River --------------------------------- */
  heads_up.Deal();
  heads_up.ProceedPlay();

  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kFold));
  REQUIRE_THROWS(heads_up.NewHand());
  REQUIRE_THROWS(heads_up.Deal());
  REQUIRE_THROWS(heads_up.ProceedPlay());

  fishbait::HandArray<fishbait::ISO_Card, 2> cards1 = {{{Card("5s"),
      Card("4s")}, {Card("Kh"), Card("Th")}}};
  fishbait::MultiBoardArray<fishbait::ISO_Card, 1> board1 = {{{Card("7h"),
      Card("Ah"), Card("3c"), Card("7s"), Card("6c")}}};
  heads_up.SetBoard(board1[0]);
  heads_up.SetHands(cards1);
  REQUIRE(heads_up.PlayerCards(1) == std::array{Card("Kh"), Card("Th"),
      Card("7h"), Card("Ah"), Card("3c"), Card("7s"), Card("6c")});
  heads_up.AwardPot(heads_up.single_run_);

  // Check AwardPot results
  REQUIRE(heads_up.pot() == 0);
  REQUIRE(heads_up.bets(0) == 0);
  REQUIRE(heads_up.bets(1) == 0);
  REQUIRE(heads_up.stack(0) == 8);
  REQUIRE(heads_up.stack(1) == 192);

  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kFold));
  REQUIRE_THROWS(heads_up.Deal());
  REQUIRE_THROWS(heads_up.ProceedPlay());
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.single_run_));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.same_stack_no_rake_));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.multi_run_, board1));

  /* ------------------------------------------------------------------------ */
  /* ------------------------------- Hand 3 --------------------------------- */
  /* ------------------------------------------------------------------------ */

  heads_up.NewHand();

  REQUIRE(heads_up.in_progress() == true);
  REQUIRE(heads_up.round() == fishbait::Round::kPreFlop);
  REQUIRE(heads_up.acting_player() == heads_up.kChancePlayer);
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.single_run_));
  REQUIRE_THROWS(heads_up.NewHand());
  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kFold));
  REQUIRE_THROWS(heads_up.Deal());
  heads_up.ProceedPlay();
  heads_up.ResetDeck();
  REQUIRE_THROWS(heads_up.Deal());

  REQUIRE(heads_up.cycled() == 0);
  REQUIRE(heads_up.acting_player() == 0);
  REQUIRE(heads_up.players_left() == 2);
  REQUIRE(heads_up.players_all_in() == 0);
  REQUIRE(heads_up.pot() == 10);
  REQUIRE(heads_up.button() == 0);

  // Test John's action (all in)
  REQUIRE(heads_up.Rotation() == 0);
  REQUIRE(heads_up.CanCheckCall() == true);
  REQUIRE(heads_up.CanBet(5) == false);
  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kBet, 5));
  REQUIRE(heads_up.CanBet(6) == false);
  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kBet, 6));
  REQUIRE(heads_up.CanFold());
  REQUIRE(heads_up.Apply(fishbait::Action::kAllIn) == true);
  REQUIRE(heads_up.in_progress() == true);
  REQUIRE(heads_up.round() == fishbait::Round::kPreFlop);
  REQUIRE(heads_up.cycled() == 1);
  REQUIRE(heads_up.acting_player() == 1);
  REQUIRE(heads_up.folded(0) == false);
  REQUIRE(heads_up.players_left() == 2);
  REQUIRE(heads_up.players_all_in() == 1);
  REQUIRE(heads_up.pot() == 16);
  REQUIRE(heads_up.bets(0) == 10);
  REQUIRE(heads_up.stack(0) == 0);

  REQUIRE_THROWS(heads_up.AwardPot(heads_up.single_run_));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.same_stack_no_rake_));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.multi_run_, board_throw));

  // Test Mary's action (call)
  REQUIRE(heads_up.Rotation() == 0);
  REQUIRE(heads_up.CanCheckCall() == true);
  REQUIRE(heads_up.CanBet(4) == false);
  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kBet, 4));
  REQUIRE(heads_up.CanBet(6) == false);
  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kBet, 6));
  REQUIRE(heads_up.CanBet(7) == false);
  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kBet, 7));
  REQUIRE(heads_up.CanBet(8) == true);
  REQUIRE(heads_up.CanFold());
  REQUIRE(heads_up.Apply(fishbait::Action::kCheckCall) == true);
  REQUIRE(heads_up.in_progress() == true);
  REQUIRE(heads_up.round() == fishbait::Round::kFlop);
  REQUIRE(heads_up.folded(1) == false);
  REQUIRE(heads_up.players_left() == 2);
  REQUIRE(heads_up.players_all_in() == 1);

  /* -------------------------------- Flop ---------------------------------- */
  REQUIRE_THROWS(heads_up.Deal());
  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kFold));
  REQUIRE_THROWS(heads_up.NewHand());
  heads_up.ProceedPlay();
  /* -------------------------------- Turn ---------------------------------- */
  REQUIRE_THROWS(heads_up.Deal());
  heads_up.ProceedPlay();
  /* -------------------------------- River --------------------------------- */
  REQUIRE_THROWS(heads_up.Deal());
  heads_up.ProceedPlay();

  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kFold));
  REQUIRE_THROWS(heads_up.NewHand());

  fishbait::HandArray<fishbait::ISO_Card, 2> cards2 = {{{Card("8h"),
      Card("8c")}, {Card("5d"), Card("5c")}}};
  fishbait::MultiBoardArray<fishbait::ISO_Card, 1> board2 = {{{Card("9d"),
      Card("6h"), Card("7c"), Card("Td"), Card("Kh")}}};
  heads_up.SetBoard(board2[0]);
  heads_up.SetHands(cards2);
  REQUIRE(heads_up.PlayerCards(1) == std::array{Card("5d"), Card("5c"),
      Card("9d"), Card("6h"), Card("7c"), Card("Td"), Card("Kh")});
  heads_up.AwardPot(heads_up.single_run_);

  // Check AwardPot results
  REQUIRE(heads_up.pot() == 0);
  REQUIRE(heads_up.bets(0) == 0);
  REQUIRE(heads_up.bets(1) == 0);
  REQUIRE(heads_up.stack(0) == 20);
  REQUIRE(heads_up.stack(1) == 180);

  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kFold));
  REQUIRE_THROWS(heads_up.ProceedPlay());
  REQUIRE_THROWS(heads_up.Deal());
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.single_run_));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.same_stack_no_rake_));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.multi_run_, board2));
}  // TEST_CASE "heads up big blind ante big blind first"

TEST_CASE("heads up big blind ante ante first", "[poker][node]") {
  auto Card = fishbait::ISOCardFromStr;
  fishbait::Node<2, double> heads_up({100, 100}, 0, 4, 2, 2, true, false);
  REQUIRE_THROWS(heads_up.NewHand());

  // Verify that the constructor worked correctly
  REQUIRE(heads_up.big_blind() == 4);
  REQUIRE(heads_up.small_blind() == 2);
  REQUIRE(heads_up.ante() == 2);
  REQUIRE(heads_up.big_blind_ante() == true);
  REQUIRE(heads_up.blind_before_ante() == false);
  REQUIRE(heads_up.button() == 0);
  REQUIRE(heads_up.in_progress() == true);
  REQUIRE(heads_up.round() == fishbait::Round::kPreFlop);
  REQUIRE(heads_up.acting_player() == heads_up.kChancePlayer);
  heads_up.Deal();
  heads_up.ProceedPlay();

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
  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kBet, 5));
  REQUIRE(heads_up.CanBet(6) == true);
  REQUIRE(heads_up.CanBet(97) == true);
  REQUIRE(heads_up.CanBet(98) == false);
  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kBet, 98));
  REQUIRE(heads_up.CanFold());
  REQUIRE(heads_up.Apply(fishbait::Action::kAllIn) == true);
  REQUIRE(heads_up.in_progress() == true);
  REQUIRE(heads_up.round() == fishbait::Round::kPreFlop);
  REQUIRE(heads_up.cycled() == 1);
  REQUIRE(heads_up.acting_player() == 1);
  REQUIRE(heads_up.folded(0) == false);
  REQUIRE(heads_up.players_left() == 2);
  REQUIRE(heads_up.players_all_in() == 1);
  REQUIRE(heads_up.pot() == 108);
  REQUIRE(heads_up.bets(0) == 102);
  REQUIRE(heads_up.stack(0) == 0);
  REQUIRE_THROWS(heads_up.NewHand());

  fishbait::MultiBoardArray<fishbait::ISO_Card, 8> board_throw;
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.single_run_));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.same_stack_no_rake_));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.multi_run_, board_throw));

  // Test Mary's action (all in)
  REQUIRE(heads_up.Rotation() == 0);
  REQUIRE(heads_up.CanCheckCall() == false);
  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kCheckCall));
  REQUIRE(heads_up.CanBet(3) == false);
  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kBet, 3));
  REQUIRE(heads_up.CanBet(4) == false);
  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kBet, 4));
  REQUIRE(heads_up.CanBet(91) == false);
  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kBet, 91));
  REQUIRE(heads_up.CanBet(92) == false);
  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kBet, 92));
  REQUIRE(heads_up.CanFold());
  REQUIRE(heads_up.Apply(fishbait::Action::kAllIn) == true);
  REQUIRE(heads_up.in_progress() == true);
  REQUIRE(heads_up.round() == fishbait::Round::kFlop);
  REQUIRE(heads_up.acting_player() == heads_up.kChancePlayer);
  REQUIRE(heads_up.folded(1) == false);
  REQUIRE(heads_up.players_left() == 2);
  REQUIRE(heads_up.players_all_in() == 2);

  /* -------------------------------- Flop ---------------------------------- */
  heads_up.Deal();
  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kFold));
  REQUIRE_THROWS(heads_up.NewHand());
  heads_up.ProceedPlay();
  /* -------------------------------- Turn ---------------------------------- */
  heads_up.Deal();
  heads_up.ProceedPlay();
  /* -------------------------------- River --------------------------------- */
  heads_up.Deal();
  heads_up.ProceedPlay();

  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kFold));
  REQUIRE_THROWS(heads_up.NewHand());

  fishbait::HandArray<fishbait::ISO_Card, 2> cards0 = {{{Card("6d"),
      Card("6c")}, {Card("Qc"), Card("Jd")}}};
  fishbait::MultiBoardArray<fishbait::ISO_Card, 1> board0 = {{{Card("7h"),
      Card("4s"), Card("Js"), Card("Th"), Card("Ad")}}};
  heads_up.SetBoard(board0[0]);
  heads_up.SetHands(cards0);
  REQUIRE(heads_up.PlayerCards(1) == std::array{Card("Qc"), Card("Jd"),
      Card("7h"), Card("4s"), Card("Js"), Card("Th"), Card("Ad")});
  heads_up.AwardPot(heads_up.single_run_);

  // Check AwardPot results
  REQUIRE(heads_up.pot() == 0);
  REQUIRE(heads_up.bets(0) == 0);
  REQUIRE(heads_up.bets(1) == 0);
  REQUIRE(heads_up.stack(0) == 4);
  REQUIRE(heads_up.stack(1) == 196);

  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kFold));
  REQUIRE_THROWS(heads_up.ProceedPlay());
  REQUIRE_THROWS(heads_up.Deal());
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.single_run_));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.same_stack_no_rake_));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.multi_run_, board0));

  /* ------------------------------------------------------------------------ */
  /* ------------------------------- Hand 2 --------------------------------- */
  /* ------------------------------------------------------------------------ */

  heads_up.NewHand();

  REQUIRE(heads_up.in_progress() == true);
  REQUIRE(heads_up.round() == fishbait::Round::kPreFlop);
  REQUIRE(heads_up.acting_player() == heads_up.kChancePlayer);
  heads_up.ResetDeck();
  heads_up.Deal();
  heads_up.ProceedPlay();

  REQUIRE(heads_up.cycled() == 0);
  REQUIRE(heads_up.acting_player() == 1);
  REQUIRE(heads_up.players_left() == 2);
  REQUIRE(heads_up.players_all_in() == 1);
  REQUIRE(heads_up.pot() == 6);
  REQUIRE(heads_up.button() == 1);

  REQUIRE_THROWS(heads_up.AwardPot(heads_up.single_run_));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.same_stack_no_rake_));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.multi_run_, board_throw));

  // Test Mary's action (call)
  REQUIRE(heads_up.Rotation() == 0);
  REQUIRE(heads_up.CanCheckCall() == true);
  REQUIRE(heads_up.CanFold());
  REQUIRE(heads_up.Apply(fishbait::Action::kCheckCall) == true);
  REQUIRE(heads_up.in_progress() == true);
  REQUIRE(heads_up.round() == fishbait::Round::kFlop);
  REQUIRE(heads_up.folded(1) == false);
  REQUIRE(heads_up.players_left() == 2);
  REQUIRE(heads_up.players_all_in() == 1);

  /* -------------------------------- Flop ---------------------------------- */
  heads_up.Deal();
  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kFold));
  REQUIRE_THROWS(heads_up.NewHand());
  heads_up.ProceedPlay();
  /* -------------------------------- Turn ---------------------------------- */
  heads_up.Deal();
  heads_up.ProceedPlay();
  /* -------------------------------- River --------------------------------- */
  heads_up.Deal();
  heads_up.ProceedPlay();

  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kFold));
  REQUIRE_THROWS(heads_up.NewHand());

  fishbait::HandArray<fishbait::ISO_Card, 2> cards1 = {{{Card("Ah"),
      Card("Kd")}, {Card("Qc"), Card("Qh")}}};
  fishbait::MultiBoardArray<fishbait::ISO_Card, 1> board1 = {{{Card("3h"),
      Card("6c"), Card("9c"), Card("9d"), Card("Ad")}}};
  heads_up.SetBoard(board1[0]);
  heads_up.SetHands(cards1);
  REQUIRE(heads_up.PlayerCards(0) == std::array{Card("Ah"), Card("Kd"),
      Card("3h"), Card("6c"), Card("9c"), Card("9d"), Card("Ad")});
  heads_up.AwardPot(heads_up.single_run_);

  // Check AwardPot results
  REQUIRE(heads_up.pot() == 0);
  REQUIRE(heads_up.bets(0) == 0);
  REQUIRE(heads_up.bets(1) == 0);
  REQUIRE(heads_up.stack(0) == 4);
  REQUIRE(heads_up.stack(1) == 196);

  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kFold));
  REQUIRE_THROWS(heads_up.ProceedPlay());
  REQUIRE_THROWS(heads_up.Deal());
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.single_run_));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.same_stack_no_rake_));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.multi_run_, board1));
  heads_up.ResetDeck();

  /* ------------------------------------------------------------------------ */
  /* ------------------------------- Hand 3 --------------------------------- */
  /* ------------------------------------------------------------------------ */

  heads_up.NewHand();

  REQUIRE(heads_up.in_progress() == true);
  REQUIRE(heads_up.round() == fishbait::Round::kPreFlop);
  REQUIRE(heads_up.acting_player() == heads_up.kChancePlayer);
  heads_up.Deal();
  heads_up.ProceedPlay();

  REQUIRE(heads_up.cycled() == 0);
  REQUIRE(heads_up.acting_player() == 0);
  REQUIRE(heads_up.players_left() == 2);
  REQUIRE(heads_up.players_all_in() == 0);
  REQUIRE(heads_up.pot() == 10);
  REQUIRE(heads_up.button() == 0);

  // Test John's action (all in)
  REQUIRE(heads_up.Rotation() == 0);
  REQUIRE(heads_up.CanCheckCall() == false);
  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kCheckCall));
  REQUIRE(heads_up.CanBet(0) == false);
  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kBet, 0));
  REQUIRE(heads_up.CanBet(1) == false);
  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kBet, 1));
  REQUIRE(heads_up.CanBet(2) == false);
  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kBet, 2));
  REQUIRE(heads_up.CanBet(3) == false);
  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kBet, 3));
  REQUIRE(heads_up.CanFold());
  REQUIRE(heads_up.Apply(fishbait::Action::kAllIn) == true);
  REQUIRE(heads_up.in_progress() == true);

  /* Mary can't fold or raise since she already has enough chips in the pot to
     continue and John is all in, so play skips to the next chance node. */

  REQUIRE(heads_up.round() == fishbait::Round::kFlop);
  REQUIRE(heads_up.acting_player() == heads_up.kChancePlayer);
  REQUIRE(heads_up.folded(0) == false);
  REQUIRE(heads_up.players_left() == 2);
  REQUIRE(heads_up.players_all_in() == 1);
  REQUIRE(heads_up.pot() == 12);
  REQUIRE(heads_up.bets(0) == 6);
  REQUIRE(heads_up.stack(0) == 0);

  REQUIRE_THROWS(heads_up.AwardPot(heads_up.single_run_));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.multi_run_, board_throw));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.same_stack_no_rake_));

  /* -------------------------------- Flop ---------------------------------- */
  heads_up.Deal();
  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kFold));
  REQUIRE_THROWS(heads_up.NewHand());
  heads_up.ProceedPlay();
  /* -------------------------------- Turn ---------------------------------- */
  heads_up.Deal();
  heads_up.ProceedPlay();
  /* -------------------------------- River --------------------------------- */
  heads_up.Deal();
  heads_up.ProceedPlay();

  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kFold));
  REQUIRE_THROWS(heads_up.NewHand());

  fishbait::HandArray<fishbait::ISO_Card, 2> cards2 = {{{Card("Qc"),
      Card("Qh")}, {Card("8s"), Card("8d")}}};
  fishbait::MultiBoardArray<fishbait::ISO_Card, 1> board2 = {{{Card("3d"),
      Card("Ac"), Card("Kh"), Card("3h"), Card("2h")}}};
  heads_up.SetBoard(board2[0]);
  heads_up.SetHands(cards2);
  REQUIRE(heads_up.PlayerCards(1) == std::array{Card("8s"), Card("8d"),
      Card("3d"), Card("Ac"), Card("Kh"), Card("3h"), Card("2h")});
  heads_up.AwardPot(heads_up.single_run_);

  // Check AwardPot results
  REQUIRE(heads_up.pot() == 0);
  REQUIRE(heads_up.bets(0) == 0);
  REQUIRE(heads_up.bets(1) == 0);
  REQUIRE(heads_up.stack(0) == 12);
  REQUIRE(heads_up.stack(1) == 188);

  REQUIRE_THROWS(heads_up.Apply(fishbait::Action::kFold));
  REQUIRE_THROWS(heads_up.ProceedPlay());
  REQUIRE_THROWS(heads_up.Deal());
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.single_run_));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.multi_run_, board2));
  REQUIRE_THROWS(heads_up.AwardPot(heads_up.same_stack_no_rake_));
}  // TEST_CASE "heads up big blind ante ante first"

TEST_CASE("straddle", "[poker][node]") {
  auto Card = fishbait::ISOCardFromStr;
  fishbait::Node<6, fishbait::Fraction> game(fishbait::StackArray<6>(10000), 0,
                                             100, 50, 0, true, false,
                                             fishbait::Fraction{}, 5, true);
  REQUIRE_THROWS(game.NewHand());

  // Verify that the constructor worked correctly
  REQUIRE(game.button() == 0);
  REQUIRE(game.big_blind() == 100);
  REQUIRE(game.small_blind() == 50);
  REQUIRE(game.ante() == 0);
  REQUIRE(game.big_blind_ante() == true);
  REQUIRE(game.blind_before_ante() == false);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == fishbait::Round::kPreFlop);
  REQUIRE(game.acting_player() == game.kChancePlayer);
  game.PostStraddles(3);
  REQUIRE_THROWS(game.PostStraddles(3));
  game.Deal();
  REQUIRE_THROWS(game.PostStraddles(3));
  game.ProceedPlay();

  REQUIRE(game.cycled() == 0);
  REQUIRE(game.acting_player() == 0);
  REQUIRE(game.folded(0) == false);
  REQUIRE(game.players_left() == 6);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 1550);
  REQUIRE(game.bets(0) == 0);
  REQUIRE(game.stack(0) == 10000);

  fishbait::MultiBoardArray<fishbait::ISO_Card, 1> board_throws;
  REQUIRE_THROWS(game.AwardPot(game.single_run_));
  REQUIRE_THROWS(game.AwardPot(game.multi_run_, board_throws));
  REQUIRE_THROWS(game.AwardPot(game.same_stack_no_rake_));

  // Player 0 action (fold)
  REQUIRE(game.Rotation() == 0);
  REQUIRE(game.CanCheckCall() == true);
  REQUIRE(game.CanBet(1599) == false);
  REQUIRE_THROWS(game.Apply(fishbait::Action::kBet, 1599));
  REQUIRE(game.CanBet(1600) == true);
  REQUIRE(game.CanBet(9999) == true);
  REQUIRE(game.CanBet(10000) == false);
  REQUIRE_THROWS(game.Apply(fishbait::Action::kBet, 10000));
  REQUIRE(game.CanFold());
  REQUIRE(game.Apply(fishbait::Action::kFold) == true);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == fishbait::Round::kPreFlop);
  REQUIRE(game.cycled() == 1);
  REQUIRE(game.acting_player() == 1);
  REQUIRE(game.folded(0) == true);
  REQUIRE(game.players_left() == 5);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 1550);
  REQUIRE(game.bets(0) == 0);
  REQUIRE(game.stack(0) == 10000);
  REQUIRE_THROWS(game.NewHand());
  REQUIRE_THROWS(game.PostStraddles(3));

  // Player 1 action (fold)
  REQUIRE(game.Rotation() == 0);
  REQUIRE(game.CanCheckCall() == true);
  REQUIRE(game.CanBet(800) == false);
  REQUIRE_THROWS(game.Apply(fishbait::Action::kBet, 800));
  REQUIRE(game.CanBet(1549) == false);
  REQUIRE_THROWS(game.Apply(fishbait::Action::kBet, 1549));
  REQUIRE(game.CanBet(1550) == true);
  REQUIRE(game.CanBet(9949) == true);
  REQUIRE(game.CanBet(9950) == false);
  REQUIRE_THROWS(game.Apply(fishbait::Action::kBet, 9950));
  REQUIRE(game.CanFold());
  REQUIRE(game.Apply(fishbait::Action::kFold) == true);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == fishbait::Round::kPreFlop);
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
  REQUIRE_THROWS(game.Apply(fishbait::Action::kBet, 800));
  REQUIRE(game.CanBet(1499) == false);
  REQUIRE_THROWS(game.Apply(fishbait::Action::kBet, 1499));
  REQUIRE(game.CanBet(1500) == true);
  REQUIRE(game.CanBet(9899) == true);
  REQUIRE(game.CanBet(9900) == false);
  REQUIRE_THROWS(game.Apply(fishbait::Action::kBet, 9900));
  REQUIRE(game.CanFold());
  REQUIRE(game.Apply(fishbait::Action::kFold) == true);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == fishbait::Round::kPreFlop);
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
  REQUIRE_THROWS(game.Apply(fishbait::Action::kBet, 600));
  REQUIRE(game.CanFold());
  REQUIRE(game.Apply(fishbait::Action::kCheckCall) == true);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == fishbait::Round::kPreFlop);
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
  REQUIRE_THROWS(game.Apply(fishbait::Action::kBet, 400));
  REQUIRE(game.CanFold());
  REQUIRE(game.Apply(fishbait::Action::kCheckCall) == true);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == fishbait::Round::kPreFlop);
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
  REQUIRE(game.CanFold() == false);
  REQUIRE_THROWS(game.Apply(fishbait::Action::kFold));
  REQUIRE(game.Apply(fishbait::Action::kCheckCall) == true);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == fishbait::Round::kFlop);

  /* -------------------------------- Flop ---------------------------------- */

  REQUIRE(game.acting_player() == game.kChancePlayer);
  REQUIRE_THROWS(game.PostStraddles(3));
  game.Deal();
  game.ProceedPlay();
  REQUIRE(game.cycled() == 2);
  REQUIRE(game.acting_player() == 3);
  REQUIRE(game.folded(5) == false);
  REQUIRE(game.players_left() == 3);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 2550);
  REQUIRE(game.bets(5) == 800);
  REQUIRE(game.stack(5) == 9200);

  // Player 3 action (check)
  REQUIRE(game.Rotation() == 0);
  REQUIRE(game.CanCheckCall() == true);
  REQUIRE(game.CanBet(99) == false);
  REQUIRE_THROWS(game.Apply(fishbait::Action::kBet, 99));
  REQUIRE(game.CanBet(100) == true);
  REQUIRE(game.CanFold() == false);
  REQUIRE_THROWS(game.Apply(fishbait::Action::kFold));
  REQUIRE(game.Apply(fishbait::Action::kCheckCall) == true);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == fishbait::Round::kFlop);
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
  REQUIRE_THROWS(game.Apply(fishbait::Action::kBet, 99));
  REQUIRE(game.CanBet(100) == true);
  REQUIRE(game.CanBet(800) == true);
  REQUIRE(game.CanBet(9200) == false);
  REQUIRE_THROWS(game.Apply(fishbait::Action::kBet, 9200));
  REQUIRE(game.CanFold() == false);
  REQUIRE_THROWS(game.Apply(fishbait::Action::kFold));
  REQUIRE(game.Apply(fishbait::Action::kBet, 800) == true);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == fishbait::Round::kFlop);
  REQUIRE(game.cycled() == 4);
  REQUIRE(game.acting_player() == 5);
  REQUIRE(game.folded(4) == false);
  REQUIRE(game.players_left() == 3);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 3350);
  REQUIRE(game.bets(4) == 1600);
  REQUIRE(game.stack(4) == 8400);

  // Player 5 action (fold)
  fishbait::Node game_cp = game;
  REQUIRE(game_cp == game);
  REQUIRE(game.Rotation() == 0);
  REQUIRE(game.CanCheckCall() == true);
  REQUIRE(game.CanFold());
  REQUIRE(game.Apply(fishbait::Action::kFold) == true);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == fishbait::Round::kFlop);
  REQUIRE(game.cycled() == 8);
  REQUIRE(game.acting_player() == 3);
  REQUIRE(game.folded(5) == true);
  REQUIRE(game.players_left() == 2);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 3350);
  REQUIRE(game.bets(5) == 800);
  REQUIRE(game.stack(5) == 9200);
  REQUIRE(game_cp != game);

  // Player 3 action (call)
  REQUIRE(game.Rotation() == 1);
  REQUIRE(game.CanCheckCall() == true);
  REQUIRE(game.CanFold());
  REQUIRE(game.Apply(fishbait::Action::kCheckCall) == true);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == fishbait::Round::kTurn);

  /* -------------------------------- Turn ---------------------------------- */

  REQUIRE(game.acting_player() == game.kChancePlayer);
  REQUIRE_THROWS(game.PostStraddles(3));
  game.Deal();
  game.ProceedPlay();
  REQUIRE(game.cycled() == 2);
  REQUIRE(game.acting_player() == 3);
  REQUIRE(game.folded(3) == false);
  REQUIRE(game.players_left() == 2);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 4150);
  REQUIRE(game.bets(3) == 1600);
  REQUIRE(game.stack(3) == 8400);

  // Player 3 action (check)
  REQUIRE(game.Rotation() == 0);
  REQUIRE(game.CanCheckCall() == true);
  REQUIRE(game.CanFold() == false);
  REQUIRE_THROWS(game.Apply(fishbait::Action::kFold));
  REQUIRE(game.Apply(fishbait::Action::kCheckCall) == true);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == fishbait::Round::kTurn);
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
  REQUIRE(game.CanFold() == false);
  REQUIRE_THROWS(game.Apply(fishbait::Action::kFold));
  REQUIRE(game.Apply(fishbait::Action::kCheckCall) == true);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == fishbait::Round::kRiver);

  /* -------------------------------- River --------------------------------- */

  REQUIRE(game.acting_player() == game.kChancePlayer);
  REQUIRE_THROWS(game.PostStraddles(3));
  game.Deal();
  game.ProceedPlay();
  REQUIRE(game.cycled() == 2);
  REQUIRE(game.acting_player() == 3);
  REQUIRE(game.folded(4) == false);
  REQUIRE(game.players_left() == 2);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 4150);
  REQUIRE(game.bets(4) == 1600);
  REQUIRE(game.stack(4) == 8400);

  // Player 3 action (check)
  REQUIRE(game.Rotation() == 0);
  REQUIRE(game.CanCheckCall() == true);
  REQUIRE(game.CanFold() == false);
  REQUIRE_THROWS(game.Apply(fishbait::Action::kFold));
  REQUIRE(game.Apply(fishbait::Action::kCheckCall) == true);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == fishbait::Round::kRiver);
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
  REQUIRE_THROWS(game.Apply(fishbait::Action::kBet, 99));
  REQUIRE(game.CanBet(100) == true);
  REQUIRE(game.CanBet(1600) == true);
  REQUIRE(game.CanBet(8399) == true);
  REQUIRE(game.CanBet(8400) == false);
  REQUIRE_THROWS(game.Apply(fishbait::Action::kBet, 8400));
  REQUIRE(game.CanFold() == false);
  REQUIRE_THROWS(game.Apply(fishbait::Action::kFold));
  REQUIRE(game.Apply(fishbait::Action::kBet, 1600) == true);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == fishbait::Round::kRiver);
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
  REQUIRE(game.CanFold());
  REQUIRE(game.Apply(fishbait::Action::kAllIn) == true);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == fishbait::Round::kRiver);
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
  REQUIRE_THROWS(game.Apply(fishbait::Action::kCheckCall));
  REQUIRE(game.CanFold());
  REQUIRE(game.Apply(fishbait::Action::kAllIn) == false);
  REQUIRE(game.in_progress() == false);
  REQUIRE(game.round() == fishbait::Round::kRiver);
  REQUIRE(game.cycled() == 14);
  REQUIRE(game.acting_player() == 3);
  REQUIRE(game.folded(4) == false);
  REQUIRE(game.players_left() == 2);
  REQUIRE(game.players_all_in() == 2);
  REQUIRE(game.pot() == 20950);
  REQUIRE(game.bets(4) == 10000);
  REQUIRE(game.stack(4) == 0);

  /* ------------------------------ Award Pot ------------------------------- */

  REQUIRE_THROWS(game.Apply(fishbait::Action::kFold));
  REQUIRE_THROWS(game.NewHand());

  fishbait::HandArray<fishbait::ISO_Card, 6> cards0 = {{{Card("Jc"),
      Card("9d")}, {0, 0}, {0, 0}, {0, 0}, {Card("Jh"), Card("Kd")}, {0, 0}}};
  fishbait::MultiBoardArray<fishbait::ISO_Card, 1> board0 = {{{Card("Qd"),
      Card("4s"), Card("Kc"), Card("6d"), Card("Tc")}}};
  game.SetBoard(board0[0]);
  game.SetHands(cards0);
  REQUIRE(game.PlayerCards(4) == std::array{Card("Jh"), Card("Kd"), Card("Qd"),
      Card("4s"), Card("Kc"), Card("6d"), Card("Tc")});
  game.AwardPot(game.multi_run_, board0);

  // Check AwardPot results
  REQUIRE(game.pot() == 0);
  for (fishbait::PlayerId i = 0; i < 6; ++i) {
    REQUIRE(game.bets(i) == 0);
  }
  REQUIRE(game.stack(0) == 10000);
  REQUIRE(game.stack(1) == 9950);
  REQUIRE(game.stack(2) == 9900);
  REQUIRE(game.stack(3) == 0);
  REQUIRE(game.stack(4) == 20950);
  REQUIRE(game.stack(5) == 9200);

  REQUIRE_THROWS(game.Apply(fishbait::Action::kFold));
  REQUIRE_THROWS(game.AwardPot(game.single_run_));
  REQUIRE_THROWS(game.AwardPot(game.multi_run_, board0));
  REQUIRE_THROWS(game.AwardPot(game.same_stack_no_rake_));
  REQUIRE_THROWS(game.PostStraddles(3));
}  // TEST_CASE "straddle"

TEST_CASE("straddle with all in", "[poker][node]") {
  fishbait::Node<6, double> game(fishbait::StackArray<6>(400), 0, 100, 50, 0,
                                 true, false);
  REQUIRE_THROWS(game.NewHand());

  fishbait::MultiBoardArray<fishbait::ISO_Card, 1> board_throws;
  REQUIRE_THROWS(game.AwardPot(game.single_run_));
  REQUIRE_THROWS(game.AwardPot(game.multi_run_, board_throws));
  REQUIRE_THROWS(game.AwardPot(game.same_stack_no_rake_));

  REQUIRE(game.button() == 0);
  REQUIRE(game.big_blind() == 100);
  REQUIRE(game.small_blind() == 50);
  REQUIRE(game.ante() == 0);
  REQUIRE(game.big_blind_ante() == true);
  REQUIRE(game.blind_before_ante() == false);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == fishbait::Round::kPreFlop);
  REQUIRE(game.acting_player() == game.kChancePlayer);
  game.PostStraddles(3);
  game.ProceedPlay();
  REQUIRE_THROWS(game.PostStraddles(3));

  REQUIRE(game.cycled() == 0);
  REQUIRE(game.acting_player() == 5);
  for (fishbait::PlayerId i = 0; i < 6; ++i) {
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
  REQUIRE_THROWS(game.Deal());
  REQUIRE_THROWS(game.PostStraddles(3));
}  // TEST_CASE "straddle with all in"

TEST_CASE("straddle with bb ante", "[poker][node]") {
  fishbait::Node<6, double> game(fishbait::StackArray<6>(10000), 0, 100, 50,
                                 100, true, false);
  REQUIRE_THROWS(game.NewHand());

  fishbait::MultiBoardArray<fishbait::ISO_Card, 1> board_throws;
  game.SetBoard(board_throws[0]);
  REQUIRE_THROWS(game.AwardPot(game.single_run_));
  REQUIRE_THROWS(game.AwardPot(game.multi_run_, board_throws));
  REQUIRE_THROWS(game.AwardPot(game.same_stack_no_rake_));

  REQUIRE(game.button() == 0);
  REQUIRE(game.big_blind() == 100);
  REQUIRE(game.small_blind() == 50);
  REQUIRE(game.ante() == 100);
  REQUIRE(game.big_blind_ante() == true);
  REQUIRE(game.blind_before_ante() == false);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == fishbait::Round::kPreFlop);
  REQUIRE(game.acting_player() == game.kChancePlayer);
  REQUIRE(game.players() == 6);
  REQUIRE_THROWS(game.Deal());
  game.ResetDeck();
  game.Deal();
  REQUIRE_THROWS(game.PostStraddles(1));
  game.ResetDeck();
  game.PostStraddles(1);
  game.Deal();
  game.ProceedPlay();

  REQUIRE(game.cycled() == 0);
  REQUIRE(game.acting_player() == 4);
  for (fishbait::PlayerId i = 0; i < 6; ++i) {
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
  REQUIRE_THROWS(game.Deal());
  REQUIRE_THROWS(game.PostStraddles(1));
}  // TEST_CASE "straddle with bb ante"

TEST_CASE("straddle with ante", "[poker][node]") {
  fishbait::Node<6, double> game(fishbait::StackArray<6>(10000), 0, 100, 50,
                                 100, false, false);
  REQUIRE_THROWS(game.NewHand());

  fishbait::MultiBoardArray<fishbait::ISO_Card, 1> board_throws;
  game.SetBoard(board_throws[0]);
  REQUIRE_THROWS(game.AwardPot(game.single_run_));
  REQUIRE_THROWS(game.AwardPot(game.multi_run_, board_throws));
  REQUIRE_THROWS(game.AwardPot(game.same_stack_no_rake_));

  REQUIRE(game.button() == 0);
  REQUIRE(game.big_blind() == 100);
  REQUIRE(game.small_blind() == 50);
  REQUIRE(game.ante() == 100);
  REQUIRE(game.big_blind_ante() == false);
  REQUIRE(game.blind_before_ante() == false);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == fishbait::Round::kPreFlop);
  REQUIRE(game.acting_player() == game.kChancePlayer);
  game.PostStraddles(1);
  REQUIRE_THROWS(game.PostStraddles(1));
  game.ProceedPlay();

  REQUIRE(game.cycled() == 0);
  REQUIRE(game.acting_player() == 4);
  for (fishbait::PlayerId i = 0; i < 6; ++i) {
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

TEST_CASE("awardpot same stack no rake double", "[poker][node]") {
  auto Card = fishbait::ISOCardFromStr;
  fishbait::Node<3, double> game({10000, 10000, 10000}, 0, 100, 50, 0, false,
                                 false);
  INFO("Preflop");
  fishbait::Node game_cp = game;
  REQUIRE(game == game_cp);
  game.ProceedPlay();
  REQUIRE(game != game_cp);
  game.Apply(fishbait::Action::kAllIn);
  game.Apply(fishbait::Action::kAllIn);
  game.Apply(fishbait::Action::kAllIn);
  INFO("Flop");
  REQUIRE_THROWS(game.Deal());
  game.ProceedPlay();
  INFO("Turn");
  game.ProceedPlay();
  INFO("River");
  game.ProceedPlay();
  fishbait::BoardArray<fishbait::ISO_Card> board = {Card("As"), Card("Ks"),
      Card("Qs"), Card("Js"), Card("Ts")};
  fishbait::HandArray<fishbait::ISO_Card, 3> hands = {{{Card("2s"), Card("2h")},
      {Card("3s"), Card("3h")}, {Card("4s"), Card("4h")}}};
  game.SetHands(hands);
  game.SetBoard(board);
  REQUIRE(game.PlayerCards(2) == std::array{Card("4s"), Card("4h"), Card("As"),
      Card("Ks"), Card("Qs"), Card("Js"), Card("Ts")});
  game.AwardPot(game.same_stack_no_rake_);
  REQUIRE(game.stack(0) == 10000);
  REQUIRE(game.stack(1) == 10000);
  REQUIRE(game.stack(2) == 10000);
  for (fishbait::PlayerId i = 0; i < 3; ++i) {
    REQUIRE(game.bets(i) == 0);
  }
  REQUIRE(game.pot() == 0);
}  // TEST_CASE "awardpot same stack no rake double"

TEST_CASE("awardpot same stack no rake fraction", "[poker][node]") {
  auto Card = fishbait::ISOCardFromStr;
  fishbait::Node<3, fishbait::Fraction> game({10000, 10000, 10000}, 0, 100, 50,
                                             0, false, false);
  INFO("Preflop");
  game.ProceedPlay();
  game.Apply(fishbait::Action::kAllIn);
  game.Apply(fishbait::Action::kAllIn);
  game.Apply(fishbait::Action::kAllIn);
  INFO("Flop");
  REQUIRE_THROWS(game.Deal());
  game.ProceedPlay();
  INFO("Turn");
  game.ProceedPlay();
  INFO("River");
  game.ProceedPlay();
  fishbait::BoardArray<fishbait::ISO_Card> board = {Card("2d"), Card("2c"),
      Card("Qs"), Card("Js"), Card("Ts")};
  fishbait::HandArray<fishbait::ISO_Card, 3> hands = {{{Card("2s"), Card("2h")},
      {Card("3s"), Card("3h")}, {0, 0}}};
  game.SetHands(hands);
  game.SetBoard(board);
  REQUIRE(game.PlayerCards(0) == std::array{Card("2s"), Card("2h"), Card("2d"),
      Card("2c"), Card("Qs"), Card("Js"), Card("Ts")});
  game.AwardPot(game.same_stack_no_rake_);
  REQUIRE(game.stack(0) == 30000);
  REQUIRE(game.stack(1) == 0);
  REQUIRE(game.stack(2) == 0);
  for (fishbait::PlayerId i = 0; i < 3; ++i) {
    REQUIRE(game.bets(i) == 0);
  }
  REQUIRE(game.pot() == 0);
}  // TEST_CASE "awardpot same stack no rake fraction"

TEST_CASE("awardpot single run double", "[poker][node]") {
  auto Card = fishbait::ISOCardFromStr;
  fishbait::Node<3, double> game({10000, 10000, 10000}, 0, 100, 50, 100, true,
                                 false);
  REQUIRE_THROWS(game.Apply(fishbait::Action::kAllIn));
  INFO("Preflop");
  game.ProceedPlay();
  game.Apply(fishbait::Action::kAllIn);
  REQUIRE(game.CanCheckCall() == false);
  REQUIRE(game.CanFold());
  game.Apply(fishbait::Action::kAllIn);
  REQUIRE(game.CanFold());
  REQUIRE(game.CanCheckCall() == false);
  game.Apply(fishbait::Action::kAllIn);
  INFO("Flop");
  REQUIRE_THROWS(game.Deal());
  game.ProceedPlay();
  INFO("Turn");
  game.ProceedPlay();
  INFO("River");
  game.ProceedPlay();
  fishbait::BoardArray<fishbait::ISO_Card> board = {Card("8c"), Card("9c"),
      Card("Ts"), Card("Js"), Card("Qs")};
  fishbait::HandArray<fishbait::ISO_Card, 3> hands = {{{Card("2s"), Card("2h")},
      {Card("3s"), Card("3h")}, {Card("Ah"), Card("Kh")}}};
  game.SetHands(hands);
  game.SetBoard(board);
  REQUIRE(game.PlayerCards(1) == std::array{Card("3s"), Card("3h"), Card("8c"),
      Card("9c"), Card("Ts"), Card("Js"), Card("Qs")});
  game.AwardPot(game.single_run_);
  REQUIRE(game.stack(0) == 300);
  REQUIRE(game.stack(1) == 300);
  REQUIRE(game.stack(2) == 29400);
  for (fishbait::PlayerId i = 0; i < 3; ++i) {
    REQUIRE(game.bets(i) == 0);
  }
  REQUIRE(game.pot() == 0);
}  // TEST_CASE "awardpot single run double"

TEST_CASE("awardpot single run fraction", "[poker][node]") {
  auto Card = fishbait::ISOCardFromStr;
  fishbait::Node<4, fishbait::Fraction> game(10000, 0, 100, 50, 100, true,
                                             false);
  REQUIRE_THROWS(game.Apply(fishbait::Action::kAllIn));
  INFO("Preflop");
  game.ProceedPlay();
  game.Apply(fishbait::Action::kAllIn);
  REQUIRE(game.CanFold());
  REQUIRE(game.CanCheckCall() == false);
  game.Apply(fishbait::Action::kAllIn);
  REQUIRE(game.CanFold());
  REQUIRE(game.CanCheckCall() == false);
  game.Apply(fishbait::Action::kAllIn);
  REQUIRE(game.CanFold());
  REQUIRE(game.CanCheckCall() == false);
  game.Apply(fishbait::Action::kAllIn);
  INFO("Flop");
  REQUIRE_THROWS(game.Deal());
  game.ProceedPlay();
  INFO("Turn");
  game.ProceedPlay();
  INFO("River");
  game.ProceedPlay();
  fishbait::BoardArray<fishbait::ISO_Card> board = {Card("8c"), Card("9c"),
      Card("Ts"), Card("Js"), Card("Qs")};
  fishbait::HandArray<fishbait::ISO_Card, 4> hands = {{{Card("Ad"), Card("Kd")},
      {Card("Ac"), Card("Kc")}, {Card("Ah"), Card("Kh")},
      {Card("2s"), Card("2h")}}};
  game.SetBoard(board);
  game.SetHands(hands);
  REQUIRE(game.PlayerCards(3) == std::array{Card("2s"), Card("2h"), Card("8c"),
      Card("9c"), Card("Ts"), Card("Js"), Card("Qs")});
  game.AwardPot(game.single_run_);
  REQUIRE((game.stack(0) == 13533 || game.stack(0) == 13534));
  REQUIRE((game.stack(1) == 13533 || game.stack(1) == 13534));
  REQUIRE((game.stack(2) == 12933 || game.stack(2) == 12934));
  REQUIRE(game.stack(3) == 0);
  REQUIRE(game.stack(0) + game.stack(1) + game.stack(2) + game.stack(3) ==
          40000);
  for (fishbait::PlayerId i = 0; i < 4; ++i) {
    REQUIRE(game.bets(i) == 0);
  }
  REQUIRE(game.pot() == 0);
}  // TEST_CASE "awardpot single run double"

TEMPLATE_TEST_CASE("awardpot multi run", "[poker][node]", double,
                   fishbait::Fraction) {
  auto Card = fishbait::ISOCardFromStr;
  fishbait::Node<5, TestType> game(11, 0, 2, 1, 0, true, false);
  INFO("Preflop");
  game.ProceedPlay();
  // Preflop fold to big blind
  game.Apply(fishbait::Action::kFold);
  game.Apply(fishbait::Action::kFold);
  game.Apply(fishbait::Action::kFold);
  game.Apply(fishbait::Action::kFold);

  INFO("Award Pot");
  fishbait::MultiBoardArray<fishbait::ISO_Card, 1> multi_board;
  game.AwardPot(game.multi_run_, multi_board);
  REQUIRE(game.stack(0) == 11);
  REQUIRE(game.stack(1) == 10);
  REQUIRE(game.stack(2) == 12);
  REQUIRE(game.stack(3) == 11);
  REQUIRE(game.stack(4) == 11);
  for (fishbait::PlayerId i = 0; i < 5; ++i) {
    REQUIRE(game.bets(i) == 0);
  }
  REQUIRE(game.pot() == 0);

  INFO("Hand 2");
  game.NewHand();
  game.ProceedPlay();
  REQUIRE(game.button() == 1);
  INFO("Preflop");
  game.Apply(fishbait::Action::kFold);
  game.Apply(fishbait::Action::kFold);
  game.Apply(fishbait::Action::kFold);
  game.Apply(fishbait::Action::kCheckCall);
  game.Apply(fishbait::Action::kCheckCall);
  INFO("Flop");
  game.ProceedPlay();
  game.Apply(fishbait::Action::kCheckCall);
  game.Apply(fishbait::Action::kCheckCall);
  INFO("Turn");
  game.ProceedPlay();
  game.Apply(fishbait::Action::kCheckCall);
  game.Apply(fishbait::Action::kCheckCall);
  INFO("River");
  game.ProceedPlay();
  game.Apply(fishbait::Action::kCheckCall);
  game.Apply(fishbait::Action::kCheckCall);
  INFO("Award Pot");
  fishbait::MultiBoardArray<fishbait::ISO_Card, 1> board_0 = {{{Card("8c"),
      Card("9c"), Card("Ts"), Card("Js"), Card("Qs")}}};
  fishbait::HandArray<fishbait::ISO_Card, 5> hands_0 = {{{}, {}, {0, 0},
      {Card("2s"), Card("2h")}, {}}};
  game.SetHands(hands_0);
  game.AwardPot(game.multi_run_, board_0);
  REQUIRE(game.stack(0) == 11);
  REQUIRE(game.stack(1) == 10);
  REQUIRE(game.stack(2) == 10);
  REQUIRE(game.stack(3) == 13);
  REQUIRE(game.stack(4) == 11);
  for (fishbait::PlayerId i = 0; i < 5; ++i) {
    REQUIRE(game.bets(i) == 0);
  }
  REQUIRE(game.pot() == 0);

  INFO("Hand 3");
  game.NewHand();
  REQUIRE(game.button() == 2);
  INFO("Preflop");
  game.ProceedPlay();
  game.Apply(fishbait::Action::kAllIn);
  game.Apply(fishbait::Action::kAllIn);
  game.Apply(fishbait::Action::kAllIn);
  game.Apply(fishbait::Action::kAllIn);
  game.Apply(fishbait::Action::kAllIn);
  INFO("Flop");
  REQUIRE_THROWS(game.Deal());
  game.ProceedPlay();
  INFO("Turn");
  game.ProceedPlay();
  INFO("River");
  game.ProceedPlay();
  INFO("Award Pot");
  fishbait::MultiBoardArray<fishbait::ISO_Card, 3> board_1 = {{{Card("8c"),
      Card("9c"), Card("Tc"), Card("Jc"), Card("2d")}, {Card("2c"), Card("3c"),
      Card("4c"), Card("5d"), Card("6d")}, {Card("8s"), Card("9s"), Card("Ts"),
      Card("Qs"), Card("2h")}}};
  fishbait::HandArray<fishbait::ISO_Card, 5> hands_1 = {{{Card("6h"),
      Card("7h")}, {Card("Ac"), Card("Kc")}, {Card("As"), Card("Ks")},
      {Card("5s"), Card("7s")}, {Card("Qd"), Card("8d")}}};
  game.SetHands(hands_1);
  game.AwardPot(game.multi_run_, board_1);
  REQUIRE((game.stack(0) == 1 || game.stack(0) == 0));
  REQUIRE(game.stack(1) == 33);
  REQUIRE(game.stack(2) == 17);
  REQUIRE((game.stack(3) == 3 || game.stack(3) == 4));
  REQUIRE(game.stack(4) == 1);
  fishbait::Chips stack_sum = 0;
  for (fishbait::PlayerId i = 0; i < 5; ++i) {
    stack_sum += game.stack(i);
  }
  REQUIRE(stack_sum == 55);
  for (fishbait::PlayerId i = 0; i < 5; ++i) {
    REQUIRE(game.bets(i) == 0);
  }
  REQUIRE(game.pot() == 0);
}  // TEMPLATE_TEST_CASE "awardpot multi run"

TEMPLATE_TEST_CASE("awardpot multi run rake no flop no drop",
                   "[poker][node]", double, fishbait::Fraction) {
  auto Card = fishbait::ISOCardFromStr;
  TestType rake{1};
  rake /= 20;
  fishbait::Node<5, TestType> game(11, 0, 2, 1, 0, true, false, rake, 0, true);
  // Preflop fold to big blind
  game.ProceedPlay();
  game.Apply(fishbait::Action::kFold);
  game.Apply(fishbait::Action::kFold);
  game.Apply(fishbait::Action::kFold);
  game.Apply(fishbait::Action::kFold);
  // Award Pot
  fishbait::MultiBoardArray<fishbait::ISO_Card, 4> multi_board;
  game.AwardPot(game.multi_run_, multi_board);
  REQUIRE(game.stack(0) == 11);
  REQUIRE(game.stack(1) == 10);
  REQUIRE(game.stack(2) == 12);
  REQUIRE(game.stack(3) == 11);
  REQUIRE(game.stack(4) == 11);
  for (fishbait::PlayerId i = 0; i < 5; ++i) {
    REQUIRE(game.bets(i) == 0);
  }
  REQUIRE(game.pot() == 0);

  // Hand 2
  game.NewHand();
  REQUIRE(game.button() == 1);
  // Preflop
  game.ProceedPlay();
  game.Apply(fishbait::Action::kFold);
  game.Apply(fishbait::Action::kFold);
  game.Apply(fishbait::Action::kFold);
  game.Apply(fishbait::Action::kCheckCall);
  game.Apply(fishbait::Action::kCheckCall);
  // Flop
  game.ProceedPlay();
  game.Apply(fishbait::Action::kCheckCall);
  game.Apply(fishbait::Action::kCheckCall);
  // Turn
  game.ProceedPlay();
  game.Apply(fishbait::Action::kCheckCall);
  game.Apply(fishbait::Action::kCheckCall);
  // River
  game.ProceedPlay();
  game.Apply(fishbait::Action::kCheckCall);
  game.Apply(fishbait::Action::kCheckCall);
  // Award Pot
  fishbait::MultiBoardArray<fishbait::ISO_Card, 1> board_0 = {{{Card("8c"),
      Card("9c"), Card("Ts"), Card("Js"), Card("Qs")}}};
  fishbait::HandArray<fishbait::ISO_Card, 5> hands_0 = {{{}, {}, {0, 0},
      {Card("2s"), Card("2h")}, {}}};
  game.SetHands(hands_0);
  game.AwardPot(game.multi_run_, board_0);
  REQUIRE(game.stack(0) == 11);
  REQUIRE(game.stack(1) == 10);
  REQUIRE(game.stack(2) == 10);
  REQUIRE(game.stack(3) == 13);
  REQUIRE(game.stack(4) == 11);
  for (fishbait::PlayerId i = 0; i < 5; ++i) {
    REQUIRE(game.bets(i) == 0);
  }
  REQUIRE(game.pot() == 0);

  // Hand 3
  game.NewHand();
  REQUIRE(game.button() == 2);
  // Preflop everyone all in
  game.ProceedPlay();
  game.Apply(fishbait::Action::kAllIn);
  game.Apply(fishbait::Action::kAllIn);
  game.Apply(fishbait::Action::kAllIn);
  game.Apply(fishbait::Action::kAllIn);
  game.Apply(fishbait::Action::kAllIn);
  // Flop
  REQUIRE_THROWS(game.Deal());
  game.ProceedPlay();
  // Turn
  game.ProceedPlay();
  // River
  game.ProceedPlay();
  // Award Pot
  fishbait::MultiBoardArray<fishbait::ISO_Card, 3> board_1 = {{{Card("8c"),
      Card("9c"), Card("Tc"), Card("Jc"), Card("2d")}, {Card("2c"), Card("3c"),
      Card("4c"), Card("5d"), Card("6d")}, {Card("8s"), Card("9s"), Card("Ts"),
      Card("Qs"), Card("2h")}}};
  fishbait::HandArray<fishbait::ISO_Card, 5> hands_1 = {{{Card("6h"),
      Card("7h")}, {Card("Ac"), Card("Kc")}, {Card("As"), Card("Ks")},
      {Card("5s"), Card("7s")}, {Card("Qd"), Card("8d")}}};
  game.SetHands(hands_1);
  game.AwardPot(game.multi_run_, board_1);
  REQUIRE(game.stack(0) == 0);
  REQUIRE(game.stack(1) == 32);
  REQUIRE(game.stack(2) == 16);
  REQUIRE(game.stack(3) == 3);
  REQUIRE(game.stack(4) == 1);
  fishbait::Chips stack_sum = 0;
  for (fishbait::PlayerId i = 0; i < 5; ++i) {
    stack_sum += game.stack(i);
  }
  REQUIRE(stack_sum == 52);
  for (fishbait::PlayerId i = 0; i < 5; ++i) {
    REQUIRE(game.bets(i) == 0);
  }
  REQUIRE(game.pot() == 0);
}  // TEMPLATE_TEST_CASE "awardpot multi run rake no flop no drop"

TEMPLATE_TEST_CASE("awardpot single run rake rake cap", "[poker][node]",
                   double, fishbait::Fraction) {
  auto Card = fishbait::ISOCardFromStr;
  TestType rake{1};
  rake /= 20;
  fishbait::Node<4, TestType> game(10000, 0, 100, 50, 100, true, false, rake,
                                   100, false);
  game.ProceedPlay();
  game.Apply(fishbait::Action::kAllIn);
  REQUIRE(game.CanFold());
  REQUIRE(game.CanCheckCall() == false);
  game.Apply(fishbait::Action::kAllIn);
  REQUIRE(game.CanFold());
  REQUIRE(game.CanCheckCall() == false);
  game.Apply(fishbait::Action::kAllIn);
  REQUIRE(game.CanFold());
  REQUIRE(game.CanCheckCall() == false);
  game.Apply(fishbait::Action::kAllIn);
  // Flop
  REQUIRE_THROWS(game.Deal());
  game.ProceedPlay();
  // Turn
  game.ProceedPlay();
  // River
  game.ProceedPlay();
  fishbait::BoardArray<fishbait::ISO_Card> board = {Card("8c"), Card("9c"),
      Card("Ts"), Card("Js"), Card("Qs")};
  fishbait::HandArray<fishbait::ISO_Card, 4> hands = {{{Card("Ad"), Card("Kd")},
      {Card("Ac"), Card("Kc")}, {Card("Ah"), Card("Kh")}, {Card("2s"),
      Card("2h")}}};
  game.SetHands(hands);
  game.SetBoard(board);
  REQUIRE(game.PlayerCards(2) == std::array{Card("Ah"), Card("Kh"), Card("8c"),
      Card("9c"), Card("Ts"), Card("Js"), Card("Qs")});
  game.AwardPot(game.single_run_);
  REQUIRE((game.stack(0) == 13499 || game.stack(0) == 13500));
  REQUIRE((game.stack(1) == 13499 || game.stack(1) == 13500));
  REQUIRE(game.stack(2) == 12901);
  REQUIRE(game.stack(3) == 0);
  REQUIRE(game.stack(0) + game.stack(1) + game.stack(2) + game.stack(3) ==
          39900);
  for (fishbait::PlayerId i = 0; i < 4; ++i) {
    REQUIRE(game.bets(i) == 0);
  }
  REQUIRE(game.pot() == 0);
}  // TEMPLATE_TEST_CASE "awardpot single run rake rake cap"

TEST_CASE("bb ante with change", "[poker][node]") {
  fishbait::Node<3, double> game(250, 0, 100, 50, 100, true, false);
  REQUIRE(game.players_all_in() == 1);
  REQUIRE(game.bets(0) == 83);
  REQUIRE(game.bets(1) == 133);
  REQUIRE(game.bets(2) == 84);
  REQUIRE(game.stack(0) == 250);
  REQUIRE(game.stack(1) == 200);
  REQUIRE(game.stack(2) == 0);
  REQUIRE(game.pot() == 300);

  // Preflop
  game.ProceedPlay();

  // Player 0 calls
  REQUIRE(game.CanCheckCall());
  REQUIRE(game.CanBet(100) == false);
  REQUIRE(game.CanBet(199) == false);
  REQUIRE(game.CanBet(200) == true);
  REQUIRE(game.CanFold());
  game.Apply(fishbait::Action::kCheckCall);
  REQUIRE(game.bets(0) == 183);
  REQUIRE(game.bets(1) == 133);
  REQUIRE(game.bets(2) == 84);
  REQUIRE(game.stack(0) == 150);
  REQUIRE(game.stack(1) == 200);
  REQUIRE(game.stack(2) == 0);
  REQUIRE(game.pot() == 400);

  // Player 1 calls
  REQUIRE(game.CanFold());
  REQUIRE(game.CanCheckCall());
  game.Apply(fishbait::Action::kCheckCall);
  REQUIRE(game.bets(0) == 183);
  REQUIRE(game.bets(1) == 183);
  REQUIRE(game.bets(2) == 84);
  REQUIRE(game.stack(0) == 150);
  REQUIRE(game.stack(1) == 150);
  REQUIRE(game.stack(2) == 0);
  REQUIRE(game.pot() == 450);

  REQUIRE(game.round() == fishbait::Round::kFlop);
}  // TEST_CASE "bb ante with change"

TEST_CASE("ante all players all in", "[poker][node]") {
  fishbait::Node<3, double> game(100, 0, 100, 50, 100, false, false);
  REQUIRE(game.players_all_in() == 3);
  REQUIRE(game.bets(0) == 100);
  REQUIRE(game.bets(1) == 100);
  REQUIRE(game.bets(2) == 100);
  REQUIRE(game.stack(0) == 0);
  REQUIRE(game.stack(1) == 0);
  REQUIRE(game.stack(2) == 0);
  REQUIRE(game.pot() == 300);
  // Preflop
  game.ProceedPlay();
  // Flop
  game.ProceedPlay();
  // Turn
  game.ProceedPlay();
  // River
  game.ProceedPlay();
  REQUIRE(game.round() == fishbait::Round::kRiver);
  REQUIRE(game.in_progress() == false);
}  // TEST_CASE "bb ante with change"

TEST_CASE("regular hand heads up", "[poker][node]") {
  auto Card = fishbait::ISOCardFromStr;
  fishbait::Node<2, double> game(10000, 0, 100, 50, 0, false, false);

  // Verify that the constructor worked correctly
  REQUIRE(game.big_blind() == 100);
  REQUIRE(game.small_blind() == 50);
  REQUIRE(game.ante() == 0);
  REQUIRE(game.big_blind_ante() == false);
  REQUIRE(game.blind_before_ante() == false);
  REQUIRE(game.button() == 0);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == fishbait::Round::kPreFlop);
  REQUIRE(game.acting_player() == game.kChancePlayer);

  game.ProceedPlay();
  REQUIRE(game.cycled() == 0);
  REQUIRE(game.Rotation() == 0);
  REQUIRE(game.acting_player() == 0);
  REQUIRE(game.folded(0) == false);
  REQUIRE(game.players_left() == 2);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 150);
  REQUIRE(game.bets(0) == 50);
  REQUIRE(game.stack(0) == 9950);

  // Preflop

  // Small Blind call
  REQUIRE(game.CanFold());
  game.Apply(fishbait::Action::kCheckCall);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == fishbait::Round::kPreFlop);
  REQUIRE(game.cycled() == 1);
  REQUIRE(game.Rotation() == 0);
  REQUIRE(game.acting_player() == 1);
  REQUIRE(game.folded(0) == false);
  REQUIRE(game.players_left() == 2);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 200);
  REQUIRE(game.bets(0) == 100);
  REQUIRE(game.stack(0) == 9900);

  // Big blind check
  REQUIRE(game.CanFold() == false);
  REQUIRE_THROWS(game.Apply(fishbait::Action::kFold));
  game.Apply(fishbait::Action::kCheckCall);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == fishbait::Round::kFlop);

  // Flop
  game.ProceedPlay();
  REQUIRE(game.cycled() == 0);
  REQUIRE(game.Rotation() == 0);
  REQUIRE(game.acting_player() == 0);
  REQUIRE(game.folded(1) == false);
  REQUIRE(game.players_left() == 2);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 200);
  REQUIRE(game.bets(1) == 100);
  REQUIRE(game.stack(1) == 9900);

  // Small Blind check
  REQUIRE(game.CanFold() == false);
  REQUIRE_THROWS(game.Apply(fishbait::Action::kFold));
  game.Apply(fishbait::Action::kCheckCall);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == fishbait::Round::kFlop);
  REQUIRE(game.cycled() == 1);
  REQUIRE(game.Rotation() == 0);
  REQUIRE(game.acting_player() == 1);
  REQUIRE(game.folded(0) == false);
  REQUIRE(game.players_left() == 2);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 200);
  REQUIRE(game.bets(0) == 100);
  REQUIRE(game.stack(0) == 9900);

  // big blind check
  REQUIRE(game.CanFold() == false);
  REQUIRE_THROWS(game.Apply(fishbait::Action::kFold));
  game.Apply(fishbait::Action::kCheckCall);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == fishbait::Round::kTurn);

  // Turn
  game.ProceedPlay();
  REQUIRE(game.cycled() == 0);
  REQUIRE(game.Rotation() == 0);
  REQUIRE(game.acting_player() == 0);
  REQUIRE(game.folded(1) == false);
  REQUIRE(game.players_left() == 2);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 200);
  REQUIRE(game.bets(1) == 100);
  REQUIRE(game.stack(1) == 9900);

  // Small Blind bet 200
  REQUIRE(game.CanFold() == false);
  REQUIRE_THROWS(game.Apply(fishbait::Action::kFold));
  game.Apply(fishbait::Action::kBet, 200);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == fishbait::Round::kTurn);
  REQUIRE(game.cycled() == 1);
  REQUIRE(game.Rotation() == 0);
  REQUIRE(game.acting_player() == 1);
  REQUIRE(game.folded(0) == false);
  REQUIRE(game.players_left() == 2);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 400);
  REQUIRE(game.bets(0) == 300);
  REQUIRE(game.stack(0) == 9700);

  // big blind call
  REQUIRE(game.CanFold());
  game.Apply(fishbait::Action::kCheckCall);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == fishbait::Round::kRiver);

  // River
  game.ProceedPlay();
  REQUIRE(game.cycled() == 0);
  REQUIRE(game.Rotation() == 0);
  REQUIRE(game.acting_player() == 0);
  REQUIRE(game.folded(1) == false);
  REQUIRE(game.players_left() == 2);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 600);
  REQUIRE(game.bets(1) == 300);
  REQUIRE(game.stack(1) == 9700);

  // Small Blind check
  REQUIRE(game.CanFold() == false);
  REQUIRE_THROWS(game.Apply(fishbait::Action::kFold));
  game.Apply(fishbait::Action::kCheckCall);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == fishbait::Round::kRiver);
  REQUIRE(game.cycled() == 1);
  REQUIRE(game.Rotation() == 0);
  REQUIRE(game.acting_player() == 1);
  REQUIRE(game.folded(0) == false);
  REQUIRE(game.players_left() == 2);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 600);
  REQUIRE(game.bets(0) == 300);
  REQUIRE(game.stack(0) == 9700);

  // big blind bet 300
  REQUIRE(game.CanFold() == false);
  REQUIRE_THROWS(game.Apply(fishbait::Action::kFold));
  game.Apply(fishbait::Action::kBet, 300);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == fishbait::Round::kRiver);
  REQUIRE(game.cycled() == 2);
  REQUIRE(game.Rotation() == 1);
  REQUIRE(game.acting_player() == 0);
  REQUIRE(game.folded(1) == false);
  REQUIRE(game.players_left() == 2);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 900);
  REQUIRE(game.bets(1) == 600);
  REQUIRE(game.stack(1) == 9400);

  // Small Blind raise 1650
  REQUIRE(game.CanFold());
  game.Apply(fishbait::Action::kBet, 1650);
  REQUIRE(game.in_progress() == true);
  REQUIRE(game.round() == fishbait::Round::kRiver);
  REQUIRE(game.cycled() == 3);
  REQUIRE(game.Rotation() == 1);
  REQUIRE(game.acting_player() == 1);
  REQUIRE(game.folded(0) == false);
  REQUIRE(game.players_left() == 2);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 2550);
  REQUIRE(game.bets(0) == 1950);
  REQUIRE(game.stack(0) == 8050);

  // big blind call
  REQUIRE(game.CanFold());
  game.Apply(fishbait::Action::kCheckCall);
  REQUIRE(game.in_progress() == false);
  REQUIRE(game.round() == fishbait::Round::kRiver);
  REQUIRE(game.cycled() == 4);
  REQUIRE(game.Rotation() == 2);
  REQUIRE(game.acting_player() == 0);
  REQUIRE(game.folded(1) == false);
  REQUIRE(game.players_left() == 2);
  REQUIRE(game.players_all_in() == 0);
  REQUIRE(game.pot() == 3900);
  REQUIRE(game.bets(1) == 1950);
  REQUIRE(game.stack(1) == 8050);

  // AwardPot
  fishbait::BoardArray<fishbait::ISO_Card> board = {Card("5c"), Card("Qd"),
      Card("Jd"), Card("Ks"), Card("3s")};
  fishbait::HandArray<fishbait::ISO_Card, 2> hands = {{{Card("Ac"), Card("Qh")},
      {Card("Kh"), Card("Qc")}}};
  game.SetBoard(board);
  game.SetHands(hands);
  REQUIRE(game.PlayerCards(1) == std::array{Card("Kh"), Card("Qc"), Card("5c"),
      Card("Qd"), Card("Jd"), Card("Ks"), Card("3s")});
  game.AwardPot(game.same_stack_no_rake_);
  REQUIRE(game.pot() == 0);
  REQUIRE(game.bets(0) == 0);
  REQUIRE(game.stack(0) == 8050);
  REQUIRE(game.bets(1) == 0);
  REQUIRE(game.stack(1) == 11950);
}  // TEST_CASE "regular hand heads up"

TEST_CASE("rotation in hand with several rotations", "[poker][node]") {
  fishbait::Node<3, double> game(10000, 0, 100, 50, 0, false, false);

  game.ProceedPlay();
  REQUIRE(game.cycled() == 0);
  REQUIRE(game.Rotation() == 0);
  game.Apply(fishbait::Action::kBet, 200);
  REQUIRE(game.cycled() == 1);
  REQUIRE(game.Rotation() == 0);
  game.Apply(fishbait::Action::kBet, 250);
  REQUIRE(game.cycled() == 2);
  REQUIRE(game.Rotation() == 0);
  game.Apply(fishbait::Action::kBet, 300);
  REQUIRE(game.cycled() == 3);
  REQUIRE(game.Rotation() == 1);
  game.Apply(fishbait::Action::kBet, 300);
  REQUIRE(game.cycled() == 4);
  REQUIRE(game.Rotation() == 1);
  game.Apply(fishbait::Action::kBet, 300);
  REQUIRE(game.cycled() == 5);
  REQUIRE(game.Rotation() == 1);
  game.Apply(fishbait::Action::kBet, 300);
  REQUIRE(game.cycled() == 6);
  REQUIRE(game.Rotation() == 2);
  game.Apply(fishbait::Action::kBet, 300);
  REQUIRE(game.cycled() == 7);
  REQUIRE(game.Rotation() == 2);
  game.Apply(fishbait::Action::kBet, 300);
  REQUIRE(game.cycled() == 8);
  REQUIRE(game.Rotation() == 2);
  game.Apply(fishbait::Action::kBet, 300);
  REQUIRE(game.cycled() == 9);
  REQUIRE(game.Rotation() == 3);
  game.Apply(fishbait::Action::kBet, 300);
  REQUIRE(game.cycled() == 10);
  REQUIRE(game.Rotation() == 3);
  game.Apply(fishbait::Action::kBet, 300);
  REQUIRE(game.cycled() == 11);
  REQUIRE(game.Rotation() == 3);
  game.Apply(fishbait::Action::kBet, 300);
  REQUIRE(game.cycled() == 12);
  REQUIRE(game.Rotation() == 4);
  game.Apply(fishbait::Action::kBet, 300);
  REQUIRE(game.cycled() == 13);
  REQUIRE(game.Rotation() == 4);
  game.Apply(fishbait::Action::kBet, 300);
  REQUIRE(game.cycled() == 14);
  REQUIRE(game.Rotation() == 4);
  game.Apply(fishbait::Action::kBet, 300);
  REQUIRE(game.cycled() == 15);
  REQUIRE(game.Rotation() == 5);
  game.Apply(fishbait::Action::kFold);
  game.Apply(fishbait::Action::kCheckCall);
  game.ProceedPlay();
  REQUIRE(game.cycled() == 0);
  REQUIRE(game.Rotation() == 0);
  REQUIRE(game.round() == fishbait::Round::kFlop);
  REQUIRE(game.players() == 3);
}  // TEST_CASE "rotation in hand with several rotations"

TEST_CASE("player goes all in for less than a min raise",
          "[poker][node]") {
  fishbait::Node<3, fishbait::Fraction> game(10000, 0, 100, 50, 0, false,
                                             false);
  INFO("Hand 1");

  INFO("Preflop");
  game.ProceedPlay();
  game.Apply(fishbait::Action::kFold);
  game.Apply(fishbait::Action::kBet, 4950);
  game.Apply(fishbait::Action::kCheckCall);

  INFO("Flop");
  game.ProceedPlay();
  game.Apply(fishbait::Action::kBet, 2000);
  game.Apply(fishbait::Action::kFold);

  INFO("AwardPot");
  game.AwardPot(game.same_stack_no_rake_);
  REQUIRE(game.stack(0) == 10000);
  REQUIRE(game.stack(1) == 15000);
  REQUIRE(game.stack(2) == 5000);

  INFO("Hand 2");
  game.NewHand();

  INFO("Preflop");
  game.ProceedPlay();
  game.Apply(fishbait::Action::kBet, 4900);
  game.Apply(fishbait::Action::kAllIn);
  REQUIRE(game.CanBet(9699) == false);
  REQUIRE_THROWS(game.Apply(fishbait::Action::kBet, 9699));
  REQUIRE(game.CanBet(9700) == true);
  REQUIRE(game.CanFold());
  game.Apply(fishbait::Action::kCheckCall);
  for (fishbait::Chips size = 0; size < game.stack(1); ++size) {
    REQUIRE(game.CanBet(size) == false);
    REQUIRE_THROWS(game.Apply(fishbait::Action::kBet, size));
  }
  REQUIRE(game.CanCheckCall());
  REQUIRE(game.CanFold());
  game.Apply(fishbait::Action::kCheckCall);

  INFO("Check correct results on start of the flop");
  game.ProceedPlay();
  REQUIRE(game.bets(0) == 5000);
  REQUIRE(game.bets(1) == 5000);
  REQUIRE(game.bets(2) == 5000);
  REQUIRE(game.stack(0) == 5000);
  REQUIRE(game.stack(1) == 10000);
  REQUIRE(game.stack(2) == 0);
  REQUIRE(game.players_all_in() == 1);
  REQUIRE(game.players_left() == 3);
  REQUIRE(game.cycled() == 1);
  REQUIRE(game.Rotation() == 0);
  REQUIRE(game.round() == fishbait::Round::kFlop);
  REQUIRE(game.acting_player() == 0);
}  // TEST_CASE "player goes all in for less than a min raise"

TEST_CASE("non zero button test", "[poker][node]") {
  fishbait::Node<4, double> game(10000, 2, 100, 50, 0, false, false);
  game.ProceedPlay();
  REQUIRE(game.button() == 2);
  REQUIRE(game.acting_player() == 1);
  game.Apply(fishbait::Action::kFold);
  game.Apply(fishbait::Action::kFold);
  game.Apply(fishbait::Action::kFold);
  game.AwardPot(game.same_stack_no_rake_);
  game.NewHand();
  REQUIRE(game.button() == 3);
  game.ProceedPlay();
  REQUIRE(game.acting_player() == 2);
}  // TEST_CASE "non zero button test"

TEMPLATE_TEST_CASE("convert bet 1", "[poker][node]", double,
                   fishbait::Fraction) {
  fishbait::Node<2, TestType> game(1000, 0, 75, 25, 0, false, false);

  INFO("Preflop");
  game.ProceedPlay();
  game.Apply(fishbait::Action::kCheckCall);
  game.Apply(fishbait::Action::kCheckCall);

  INFO("Flop");
  game.ProceedPlay();
  game.Apply(fishbait::Action::kBet, 120);
  game.Apply(fishbait::Action::kBet, 240);
  TestType one{1};
  TestType four{4};
  TestType two29{229};
  REQUIRE(game.ProportionToChips(one) == 750);
  REQUIRE((game.ChipsToProportion(750) == one));
  REQUIRE(game.ProportionToChips(one / 2) == 435);
  REQUIRE((game.ChipsToProportion(435) == one / 2));
  REQUIRE(game.ProportionToChips(one / 5) == 246);
  REQUIRE((game.ChipsToProportion(246) == one / 5));
  REQUIRE(game.ProportionToChips(four / 11) == 349);
  REQUIRE((game.ChipsToProportion(349) == two29 / 630));
  game.Apply(fishbait::Action::kBet, 750);
  REQUIRE(game.pot() == 1260);
}  // TEMPLATE_TEST_CASE "convert bet 1"

TEMPLATE_TEST_CASE("convert bet 2", "[poker][node]", double,
                   fishbait::Fraction) {
  fishbait::Node<3, TestType> game(500, 0, 5, 2, 0, false, false);

  INFO("Preflop");
  game.ProceedPlay();
  game.Apply(fishbait::Action::kCheckCall);
  game.Apply(fishbait::Action::kCheckCall);
  game.Apply(fishbait::Action::kCheckCall);

  INFO("Flop");
  game.ProceedPlay();
  game.Apply(fishbait::Action::kBet, 5);
  game.Apply(fishbait::Action::kBet, 15);
  TestType one{1};
  TestType one3{13};
  TestType two{2};
  TestType three{3};
  TestType three3{33};
  TestType three7{37};
  REQUIRE(game.ProportionToChips(one) == 65);
  REQUIRE((game.ChipsToProportion(65) == one));
  REQUIRE(game.ProportionToChips(one / 2) == 40);
  REQUIRE((game.ChipsToProportion(40) == one / 2));
  REQUIRE(game.ProportionToChips(one / 5) == 25);
  REQUIRE((game.ChipsToProportion(25) == one / 5));
  REQUIRE(game.ProportionToChips(two / 3) == 48);
  REQUIRE((game.ChipsToProportion(48) == three3 / 50));
  REQUIRE(game.ProportionToChips(three / 4) == 52);
  REQUIRE((game.ChipsToProportion(52) == three7 / 50));
  REQUIRE(game.ProportionToChips(one / 4) == 28);
  REQUIRE((game.ChipsToProportion(28) == one3 / 50));
  game.Apply(fishbait::Action::kBet, 65);
  REQUIRE(game.pot() == 100);
}  // TEMPLATE_TEST_CASE "convert bet 2"

TEST_CASE("deal cards test", "[poker][node]") {
  for (int trial = 0; trial < 100; ++trial) {
    fishbait::Node<6, double> game(500, 0, 5, 2, 0, false, false);
    game.Deal();  // preflop
    game.ProceedPlay();
    game.Apply(fishbait::Action::kAllIn);
    game.Apply(fishbait::Action::kAllIn);
    game.Apply(fishbait::Action::kFold);
    game.Apply(fishbait::Action::kFold);
    game.Apply(fishbait::Action::kFold);
    game.Apply(fishbait::Action::kFold);
    game.Deal();  // flop
    game.ProceedPlay();
    game.Deal();  // turn
    game.ProceedPlay();
    game.Deal();  // river
    game.ProceedPlay();
    for (fishbait::PlayerId player = 0; player < 6; ++player) {
      std::array player_cards = game.PlayerCards(player);
      for (std::size_t i = 0; i < player_cards.size(); ++i) {
        for (std::size_t j = i + 1; j < player_cards.size(); ++j) {
          REQUIRE(player_cards[i] != player_cards[j]);
        }
      }
      for (fishbait::PlayerId player2 = player + 1; player2 < 6; ++player2) {
        std::array player_cards2 = game.PlayerCards(player2);
        for (std::size_t i = 0; i < fishbait::kHandCards; ++i) {
          for (std::size_t j = 0; j < fishbait::kHandCards; ++j) {
            REQUIRE(player_cards[i] != player_cards2[j]);
          }
        }
      }
    }  // for player
  }  // for trial
}  // TEST_CASE "deal cards test"

TEST_CASE("copy with stack test", "[poker][node]") {
  fishbait::Node<3, double> game({10000, 10000, 10000}, 0, 100, 50, 0, false,
                                 false);
  fishbait::Node game_cp2{game, {20000, 20000, 20000}};
  REQUIRE(game != game_cp2);
  REQUIRE(game_cp2.button() == 0);
  REQUIRE(game_cp2.big_blind() == 100);
  REQUIRE(game_cp2.small_blind() == 50);
  REQUIRE(game_cp2.ante() == 0);
  REQUIRE(game_cp2.big_blind_ante() == false);
  REQUIRE(game_cp2.blind_before_ante() == false);
  REQUIRE(game_cp2.in_progress() == true);
  REQUIRE(game_cp2.round() == fishbait::Round::kPreFlop);
  REQUIRE(game_cp2.acting_player() == game.kChancePlayer);
  game_cp2.ProceedPlay();
  REQUIRE(game_cp2.cycled() == 0);
  REQUIRE(game_cp2.acting_player() == 0);
  for (fishbait::PlayerId i = 0; i < 3; ++i) {
    REQUIRE(game_cp2.folded(i) == false);
  }
  REQUIRE(game_cp2.players_left() == 3);
  REQUIRE(game_cp2.players_all_in() == 0);
  REQUIRE(game_cp2.pot() == 150);
  REQUIRE(game_cp2.bets(0) == 0);
  REQUIRE(game_cp2.bets(1) == 50);
  REQUIRE(game_cp2.bets(2) == 100);
  REQUIRE(game_cp2.stack(0) == 20000);
  REQUIRE(game_cp2.stack(1) == 19950);
  REQUIRE(game_cp2.stack(2) == 19900);
}  // TEST_CASE "copy with stack test"
