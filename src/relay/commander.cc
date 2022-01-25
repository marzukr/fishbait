// Copyright 2021 Marzuk Rashid

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <utility>

#include "blueprint/hyperparameters.h"
#include "clustering/cluster_table.h"
#include "poker/definitions.h"
#include "poker/node.h"
#include "relay/commander.h"
#include "relay/scribe.h"
#include "utils/cereal.h"

namespace fishbait {

extern "C" {

template class Commander<hparam::kPlayers, hparam::kActions, ClusterTable>;
using CommanderT = Commander<hparam::kPlayers, hparam::kActions, ClusterTable>;
template struct NodeSnapshot<hparam::kPlayers>;
using NodeSnapshotT = NodeSnapshot<hparam::kPlayers>;

/*
  @brief Creates a new Commander from the given Average strategy file.

  @param location The location of the Average strategy file to use.

  @return A pointer to the created Commander object on the heap.
*/
CommanderT* CommanderNew(char* location) {
  std::filesystem::path avg_loc{location};
  return new CommanderT{CommanderT::ScribeT{avg_loc}};
}

/* @brief Deallocate the given Commander object. */
void CommanderDelete(CommanderT* c) {
  delete c;
}

/*
  @brief Resets the game with the given parameters.

  @param stacks An array of the stack size of each player.
  @param button Seat number of the button.
  @param big_blind Size of the big blind in number of chips.
  @param small_blind Size of the small blind in number of chips.
  @param fishbait_seat Seat number of fishbait.
*/
void CommanderReset(CommanderT* c, Chips stacks[3], PlayerId button,
                    Chips big_blind, Chips small_blind,
                    PlayerId fishbait_seat) {
  std::array<Chips, hparam::kPlayers> stack_arr;
  std::copy_n(stacks, hparam::kPlayers, stack_arr.begin());
  Node<hparam::kPlayers> start_state{stack_arr, button, big_blind, small_blind};
  c->Reset(start_state, fishbait_seat);
}

/* @brief Deals the given player the given hand. */
void CommanderSetHand(CommanderT* c, PlayerId player,
                      ISO_Card hand[kHandCards]) {
  Hand<ISO_Card> player_hand;
  std::copy_n(hand, player_hand.size(), player_hand.begin());
  c->SetHand(player, player_hand);
}

/* @brief Proceeds play to the next round. */
void CommanderProceedPlay(CommanderT* c) {
  c->ProceedPlay();
}

/* @brief Get the current state of the game. */
NodeSnapshotT CommanderState(CommanderT* c) {
  return c->State().Snapshot();
}

/* @brief Returns fishbait's player id. */
PlayerId CommanderFishbaitSeat(CommanderT* c) {
  return c->fishbait_seat();
}

struct ActionStruct {
  Action action;
  Chips size;
  bool could_check;
};

ActionStruct CommanderQuery(CommanderT* c) {
  bool can_check = c->State().NeededToCall() == 0;
  std::pair fish_act = c->Query();
  return { fish_act.first, fish_act.second, can_check };
}

void CommanderApply(CommanderT* c, Action play, Chips size) {
  c->Apply(play, size);
}

void CommanderSetBoard(CommanderT* c, ISO_Card board[kBoardCards]) {
  BoardArray<ISO_Card> board_arr;
  std::copy_n(board, kBoardCards, board_arr.begin());
  c->SetBoard(board_arr);
}

void CommanderAwardPot(CommanderT* c) {
  c->AwardPot();
}

void CommanderNewHand(CommanderT* c) {
  c->NewHand();
}

}  // extern "C"

}  // namespace fishbait
