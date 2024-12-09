#ifndef AI_SRC_MCCFR_DEFINITIONS_H_
#define AI_SRC_MCCFR_DEFINITIONS_H_

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

  // Maximum raise number for this action. Inclusive. 0 is always allow.
  int max_raise_num = 0;

  // First round this action can be played. Inclusive.
  Round min_round = Round::kPreFlop;

  // Last round this action can be played. Inclusive.
  Round max_round = Round::kRiver;

  /* Max number of players allowed for this action. Inclusive. 0 is always
     allow. */
  PlayerN max_players = 0;

  // Min pot size for this action. Inclusive. 0 is always allow.
  Chips min_pot = 0;

  bool operator==(const AbstractAction& rhs) const {
    return play == rhs.play && size == rhs.size &&
           max_raise_num == rhs.max_raise_num && min_round == rhs.min_round &&
           max_round == rhs.max_round && max_players == rhs.max_players &&
           min_pot == rhs.min_pot;
  }

  bool operator!=(const AbstractAction& rhs) const {
    return !(*this == rhs);
  }

  /* @brief AbstractAction serialize function */
  template<class Archive>
  void serialize(Archive& archive) {
    archive(play, size, max_raise_num, min_round, max_round, max_players,
            min_pot);
  }
};

struct node_count {
  SequenceN internal_nodes = 0;  /* The number of sequence table rows in each
                                    round. */
  SequenceN leaf_nodes = 0;      /* The number of leaf nodes in each round that
                                    end directly in that round. For instance in
                                    preflop, folding to the big blind would
                                    count, but two players going all in would
                                    not count since the cards must be dealt to
                                    the river. */
  SequenceN illegal_nodes = 0;   /* The number of sequence table entries that
                                    are illegal actions in this round. Both
                                    actions that are illegal in the game and
                                    actions that are illegal because they are
                                    not in the action abstraction are
                                    counted. */
  SequenceN legal_actions = 0;   /* The number of legal actions in this round.
                                    This is the same as the number of legal
                                    sequence table entries in the round (the    // NOLINT(*)
                                    action either leads to a leaf node or
                                    another row) and is also the same as the
                                    number of regrets in that round. */
};
using NumNodesArray = std::array<node_count, kNRounds>;

inline std::ostream& operator<<(std::ostream& os, const NumNodesArray&& s) {
  uint64_t total_illegal = 0;
  os << "illegal nodes: " << s[0].illegal_nodes;
  total_illegal += s[0].illegal_nodes;
  for (std::size_t i = 1; i < s.size(); ++i) {
    total_illegal += s[i].illegal_nodes;
    os << ", " << s[i].illegal_nodes;
  }
  os << std::endl;
  os << "total illegal nodes: " << total_illegal << std::endl;

  uint64_t total_internal = 0;
  os << "internal nodes: " << s[0].internal_nodes;
  total_internal += s[0].internal_nodes;
  for (std::size_t i = 1; i < s.size(); ++i) {
    total_internal += s[i].internal_nodes;
    os << ", " << s[i].internal_nodes;
  }
  os << std::endl;
  os << "total internal nodes: " << total_internal << std::endl;

  uint64_t total_legal = 0;
  os << "legal actions: " << s[0].legal_actions;
  total_legal += s[0].legal_actions;
  for (std::size_t i = 1; i < s.size(); ++i) {
    total_legal += s[i].legal_actions;
    os << ", " << s[i].legal_actions;
  }
  os << std::endl;
  os << "total legal actions: " << total_legal << std::endl;

  uint64_t total_leaf = 0;
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

#endif  // AI_SRC_MCCFR_DEFINITIONS_H_
