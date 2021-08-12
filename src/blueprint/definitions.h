// Copyright 2021 Marzuk Rashid

#ifndef SRC_BLUEPRINT_DEFINITIONS_H_
#define SRC_BLUEPRINT_DEFINITIONS_H_

#include <cstdint>
#include <limits>
#include <string>

#include "poker/definitions.h"

namespace fishbait {

using SequenceId = uint32_t;
using SequenceN = SequenceId;

constexpr SequenceId kLeafId = std::numeric_limits<SequenceId>::max()-1;
constexpr SequenceId kIllegalId = std::numeric_limits<SequenceId>::max();

struct AbstractAction {
  Action play;

  // Bet size proportional to the pot. Only matters if play is a kBet.
  double size = 0;

  /* The largest rotation this action can be player. Not inclusive. 0 indicates
     the action can be player on any rotation. */
  PlayCount max_rotation = 0;

  // The latest round this action can be player. Inclusive.
  Round max_round = Round::kRiver;

  bool operator==(const AbstractAction& rhs) const {
    return play == rhs.play && size == rhs.size &&
           max_rotation == rhs.max_rotation && max_round == rhs.max_round;
  }
};

struct node_count {
  SequenceN internal_nodes = 0;
  SequenceN leaf_nodes = 0;
};
using NumNodesArray = std::array<node_count, kNRounds>;

inline std::ostream& operator<<(std::ostream& os, NumNodesArray& s) {
  os << "internal nodes: " << s[0].internal_nodes;
  for (std::size_t i = 1; i < s.size(); ++i) {
    os << ", " << s[i].internal_nodes;
  }
  os << std::endl;
  os << "leaf nodes: " << s[0].leaf_nodes;
  for (std::size_t i = 1; i < s.size(); ++i) {
    os << ", " << s[i].leaf_nodes;
  }
  return os;
}

using Regret = int32_t;
using ActionCount = uint32_t;

constexpr std::string_view kBlueprintSaveDir = "out/blueprint";

}  // namespace fishbait

#endif  // SRC_BLUEPRINT_DEFINITIONS_H_
