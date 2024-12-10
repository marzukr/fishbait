#include "clustering/cluster_table.h"

#include <array>
#include <numeric>
#include <utility>

#include "clustering/definitions.h"
#include "poker/definitions.h"
#include "poker/indexer.h"
#include "poker/node.h"
#include "utils/cereal.h"

namespace fishbait {

ClusterTable::ClusterTable() : table_{}, preflop_indexer_{}, flop_indexer_{},
                               turn_indexer_{}, river_indexer_{} {}

ClusterTable::ClusterTable(bool verbose) : table_{}, preflop_indexer_{},
                                           flop_indexer_{}, turn_indexer_{},
                                           river_indexer_{} {
  for (RoundId i = 0; i < kNRounds; ++i) {
    if (kClusterAssignmentFiles[i] == "") {
      table_[i].resize(kImperfectRecallHands[i]);
      std::iota(table_[i].begin(), table_[i].end(), 0);
    } else {
      CerealLoad(kClusterAssignmentFiles[i], &table_[i], verbose);
    }
  }
}

ClusterTable::ClusterTable(const ClusterTable& other) : table_{other.table_},
                                                        preflop_indexer_{},
                                                        flop_indexer_{},
                                                        turn_indexer_{},
                                                        river_indexer_{} { }

ClusterTable::ClusterTable(ClusterTable&& other)
    : table_{std::move(other.table_)}, preflop_indexer_{}, flop_indexer_{},
      turn_indexer_{}, river_indexer_{} { }

ClusterTable& ClusterTable::operator=(const ClusterTable& other) {
  table_ = other.table_;
  return *this;
}

ClusterTable& ClusterTable::operator=(ClusterTable&& other) {
  table_ = std::move(other.table_);
  return *this;
}

bool ClusterTable::operator!=(const ClusterTable& other) const {
  return table_ != other.table_;
}

bool ClusterTable::operator==(const ClusterTable& other) const {
  return !(*this != other);
}

}  // namespace fishbait
