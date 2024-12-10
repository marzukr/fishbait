#include <algorithm>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include "clustering/cluster_table.h"
#include "mccfr/definitions.h"
#include "mccfr/hyperparameters.h"
#include "poker/definitions.h"
#include "poker/node.h"
#include "relay/commander.h"
#include "relay/scribe.h"
#include "utils/cereal.h"

namespace fishbait {

extern "C" {

constexpr int kActions = hparam::kActions;

template class Commander<hparam::kPlayers, kActions, ClusterTable>;
using CommanderT = Commander<hparam::kPlayers, kActions, ClusterTable>;
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

typedef void (*CallbackFunc)(const char*, size_t);

/*
  @brief Creates a new Commander from the given Average strategy file.

  @param location The location of the Average strategy file to use.
  @param callback A callback that will receive the binary string representation
    of the new commander.
*/
void CommanderCreate(const char* location, CallbackFunc callback) {
  std::filesystem::path avg_loc{location};
  std::unique_ptr<CommanderT> napoleon = std::make_unique<CommanderT>(
    CommanderT::ScribeT{avg_loc}
  );
  std::string saved = CerealSave(&napoleon);
  callback(saved.data(), saved.length());
}

/*
  @brief Resets the game with the given parameters.

  @param buffer Binary data start of the Commander to use
  @param length Binary data length of the Commander to use

  @param stacks An array of the stack size of each player.
  @param button Seat number of the button.
  @param big_blind Size of the big blind in number of chips.
  @param small_blind Size of the small blind in number of chips.
  @param fishbait_seat Seat number of fishbait.

  @param callback A callback that will receive the binary string representation
    of the mutated commander.
*/
void CommanderReset(
  const char* buffer, std::size_t length,

  Chips stacks[3], PlayerId button, Chips big_blind, Chips small_blind,
  PlayerId fishbait_seat,

  CallbackFunc callback
) {
  try {
    std::unique_ptr<CommanderT> napoleon;
    CerealLoad(buffer, length, &napoleon);

    std::array<Chips, hparam::kPlayers> stack_arr;
    std::copy_n(stacks, hparam::kPlayers, stack_arr.begin());
    Node<hparam::kPlayers> start_state{
      stack_arr, button, big_blind, small_blind
    };
    napoleon->Reset(start_state, fishbait_seat);

    std::string saved = CerealSave(&napoleon);
    callback(saved.data(), saved.length());
  } catch (const std::exception& e) {
    HandleError(e);
  }
}

/* @brief Deals the given player the given hand. */
void CommanderSetHand(
  const char* buffer, std::size_t length,
  PlayerId player, ISO_Card hand[kHandCards],
  CallbackFunc callback
) {
  try {
    std::unique_ptr<CommanderT> napoleon;
    CerealLoad(buffer, length, &napoleon);

    Hand<ISO_Card> player_hand;
    std::copy_n(hand, player_hand.size(), player_hand.begin());
    napoleon->SetHand(player, player_hand);

    std::string saved = CerealSave(&napoleon);
    callback(saved.data(), saved.length());
  } catch (const std::exception& e) {
    HandleError(e);
  }
}

/* @brief Proceeds play to the next round. */
void CommanderProceedPlay(
  const char* buffer, std::size_t length, CallbackFunc callback
) {
  try {
    std::unique_ptr<CommanderT> napoleon;
    CerealLoad(buffer, length, &napoleon);

    napoleon->ProceedPlay();

    std::string saved = CerealSave(&napoleon);
    callback(saved.data(), saved.length());
  } catch (const std::exception& e) {
    HandleError(e);
  }
}

/* @brief Get the current state of the game. */
NodeSnapshotT CommanderState(const char* buffer, std::size_t length) {
  try {
    std::unique_ptr<CommanderT> napoleon;
    CerealLoad(buffer, length, &napoleon);
    return napoleon->State().Snapshot();
  } catch (const std::exception& e) {
    HandleError(e);
  }
  return {};
}

/* @brief Returns fishbait's player id. */
PlayerId CommanderFishbaitSeat(const char* buffer, std::size_t length) {
  try {
    std::unique_ptr<CommanderT> napoleon;
    CerealLoad(buffer, length, &napoleon);
    return napoleon->fishbait_seat();
  } catch (const std::exception& e) {
    HandleError(e);
  }
  return 0;
}

SequenceId CommanderGetIllegalActionId() {
  return kIllegalId;
}

int CommanderGetKActions() {
  return kActions;
}

/* out_arr is expected to be of size kActions */
void CommanderGetAvailableActions(
  const char* buffer, std::size_t length, CommanderT::AvailableAction* out_arr
) {
  try {
    std::unique_ptr<CommanderT> napoleon;
    CerealLoad(buffer, length, &napoleon);
    std::array aas = napoleon->GetAvailableActions();
    for (std::size_t i = 0; i < aas.size(); ++i) {
      out_arr[i] = aas[i];
    }
    return;
  } catch (const std::exception& e) {
    HandleError(e);
  }
}

struct ActionStruct {
  // The action fishbait decided to take
  Action action;
  // The amount of chips fishbait bet if the action was kBet
  Chips size;
  // The index of the chosen action
  std::size_t action_idx;
};

ActionStruct CommanderQuery(
  const char* buffer, std::size_t length, CallbackFunc callback
) {
  try {
    std::unique_ptr<CommanderT> napoleon;
    CerealLoad(buffer, length, &napoleon);
    auto [ action, size, action_idx ] = napoleon->Query();
    std::string saved = CerealSave(&napoleon);
    callback(saved.data(), saved.length());
    return { action, size, action_idx };
  } catch (const std::exception& e) {
    HandleError(e);
  }
  return {};
}

void CommanderApply(
  const char* buffer, std::size_t length,
  Action play, Chips size,
  CallbackFunc callback
) {
  try {
    std::unique_ptr<CommanderT> napoleon;
    CerealLoad(buffer, length, &napoleon);

    napoleon->Apply(play, size);

    std::string saved = CerealSave(&napoleon);
    callback(saved.data(), saved.length());
  } catch (const std::exception& e) {
    HandleError(e);
  }
}

void CommanderSetBoard(
  const char* buffer, std::size_t length,
  ISO_Card board[kBoardCards],
  CallbackFunc callback
) {
  try {
    std::unique_ptr<CommanderT> napoleon;
    CerealLoad(buffer, length, &napoleon);

    BoardArray<ISO_Card> board_arr;
    std::copy_n(board, kBoardCards, board_arr.begin());
    napoleon->SetBoard(board_arr);

    std::string saved = CerealSave(&napoleon);
    callback(saved.data(), saved.length());
  } catch (const std::exception& e) {
    HandleError(e);
  }
}

void CommanderAwardPot(
  const char* buffer, std::size_t length, CallbackFunc callback
) {
  try {
    std::unique_ptr<CommanderT> napoleon;
    CerealLoad(buffer, length, &napoleon);
    napoleon->AwardPot();
    std::string saved = CerealSave(&napoleon);
    callback(saved.data(), saved.length());
  } catch (const std::exception& e) {
    HandleError(e);
  }
}

void CommanderNewHand(
  const char* buffer, std::size_t length, CallbackFunc callback
) {
  try {
    std::unique_ptr<CommanderT> napoleon;
    CerealLoad(buffer, length, &napoleon);
    napoleon->NewHand();
    std::string saved = CerealSave(&napoleon);
    callback(saved.data(), saved.length());
  } catch (const std::exception& e) {
    HandleError(e);
  }
}

}  // extern "C"

}  // namespace fishbait
