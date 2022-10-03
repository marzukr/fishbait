// Copyright 2021 Marzuk Rashid

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
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

std::unique_ptr<std::string> error_message = nullptr;
void ClearError() {
  error_message = nullptr;
}
const char* CheckError() {
  if (!error_message) return nullptr;
  return error_message.get()->c_str();
}
void HandleError(const std::exception& e) {
  error_message = std::make_unique<std::string>(e.what());
}

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
void CommanderReset(
  CommanderT* c, Chips stacks[3], PlayerId button, Chips big_blind,
  Chips small_blind, PlayerId fishbait_seat
) {
  try {
    std::array<Chips, hparam::kPlayers> stack_arr;
    std::copy_n(stacks, hparam::kPlayers, stack_arr.begin());
    Node<hparam::kPlayers> start_state{
      stack_arr, button, big_blind, small_blind
    };
    c->Reset(start_state, fishbait_seat);
  } catch (const std::exception& e) {
    HandleError(e);
  }
}

/* @brief Deals the given player the given hand. */
void CommanderSetHand(
  CommanderT* c, PlayerId player, ISO_Card hand[kHandCards]
) {
  try {
    Hand<ISO_Card> player_hand;
    std::copy_n(hand, player_hand.size(), player_hand.begin());
    c->SetHand(player, player_hand);
  } catch (const std::exception& e) {
    HandleError(e);
  }
}

/* @brief Proceeds play to the next round. */
void CommanderProceedPlay(CommanderT* c) {
  try {
    c->ProceedPlay();
  } catch (const std::exception& e) {
    HandleError(e);
  }
}

/* @brief Get the current state of the game. */
NodeSnapshotT CommanderState(CommanderT* c) {
  try {
    return c->State().Snapshot();
  } catch (const std::exception& e) {
    HandleError(e);
  }
  return {};
}

/* @brief Returns fishbait's player id. */
PlayerId CommanderFishbaitSeat(CommanderT* c) {
  try {
    return c->fishbait_seat();
  } catch (const std::exception& e) {
    HandleError(e);
  }
  return 0;
}

struct ActionStruct {
  Action action;
  Chips size;
  bool could_check;
};

ActionStruct CommanderQuery(CommanderT* c) {
  try {
    bool can_check = c->State().NeededToCall() == 0;
    std::pair fish_act = c->Query();
    return { fish_act.first, fish_act.second, can_check };
  } catch (const std::exception& e) {
    HandleError(e);
  }
  return {};
}

void CommanderApply(CommanderT* c, Action play, Chips size) {
  try {
    c->Apply(play, size);
  } catch (const std::exception& e) {
    HandleError(e);
  }
}

void CommanderSetBoard(CommanderT* c, ISO_Card board[kBoardCards]) {
  try {
    BoardArray<ISO_Card> board_arr;
    std::copy_n(board, kBoardCards, board_arr.begin());
    c->SetBoard(board_arr);
  } catch (const std::exception& e) {
    HandleError(e);
  }
}

void CommanderAwardPot(CommanderT* c) {
  try {
    c->AwardPot();
  } catch (const std::exception& e) {
    HandleError(e);
  }
}

void CommanderNewHand(CommanderT* c) {
  try {
    c->NewHand();
  } catch (const std::exception& e) {
    HandleError(e);
  }
}

}  // extern "C"

}  // namespace fishbait
