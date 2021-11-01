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

constexpr SequenceId kLeafId = std::numeric_limits<SequenceId>::max() - 1;
constexpr SequenceId kIllegalId = std::numeric_limits<SequenceId>::max();

const double kPotCommittedThreshold = 0.5;

struct AbstractAction {
  Action play;

  // Bet size proportional to the pot. Only matters if play is a kBet.
  double size = 0;

  // Maximum raise number for this action. Inclusive.
  int max_raise_num = 0;

  // First round this action can be played. Inclusive.
  Round min_round = Round::kPreFlop;

  // Last round this action can be played. Inclusive.
  Round max_round = Round::kRiver;

  // Max number of players allowed for this action. Inclusive.
  PlayerN max_players = 0;

  // Min pot size for this action. Inclusive
  Chips min_pot = 0;

  bool operator==(const AbstractAction& rhs) const {
    return play == rhs.play && size == rhs.size &&
           max_raise_num == rhs.max_raise_num && min_round == rhs.min_round &&
           max_round == rhs.max_round && max_players == rhs.max_players &&
           min_pot == rhs.min_pot;
  }

  /* @brief AbstractAction serialize function */
  template<class Archive>
  void serialize(Archive& archive) {
    archive(play, size, max_raise_num, min_round, max_round, max_players,
            min_pot);
  }
};

struct node_count {
  SequenceN internal_nodes = 0;
  SequenceN leaf_nodes = 0;
  SequenceN illegal_nodes = 0;
};
using NumNodesArray = std::array<node_count, kNRounds>;

inline std::ostream& operator<<(std::ostream& os, NumNodesArray& s) {
  uint64_t total_internal = 0;
  uint64_t total_leaf = 0;
  uint64_t total_illegal = 0;
  os << "illegal nodes: " << s[0].illegal_nodes;
  total_illegal += s[0].illegal_nodes;
  for (std::size_t i = 1; i < s.size(); ++i) {
    total_illegal += s[i].illegal_nodes;
    os << ", " << s[i].illegal_nodes;
  }
  os << std::endl;
  os << "total illegal nodes: " << total_illegal << std::endl;
  os << "internal nodes: " << s[0].internal_nodes;
  total_internal += s[0].internal_nodes;
  for (std::size_t i = 1; i < s.size(); ++i) {
    total_internal += s[i].internal_nodes;
    os << ", " << s[i].internal_nodes;
  }
  os << std::endl;
  os << "total internal nodes: " << total_internal << std::endl;
  os << "leaf nodes: " << s[0].leaf_nodes;
  total_leaf += s[0].leaf_nodes;
  for (std::size_t i = 1; i < s.size(); ++i) {
    total_leaf += s[i].leaf_nodes;
    os << ", " << s[i].leaf_nodes;
  }
  os << std::endl;
  os << "total leaf nodes: " << total_leaf << std::endl;
  return os;
}

using Regret = int32_t;
using ActionCount = uint32_t;

}  // namespace fishbait

#endif  // SRC_BLUEPRINT_DEFINITIONS_H_
