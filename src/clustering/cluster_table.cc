// Copyright 2021 Marzuk Rashid

#include "clustering/cluster_table.h"

#include <numeric>

#include "clustering/definitions.h"
#include "engine/definitions.h"
#include "utils/cereal.h"

namespace clustering {

ClusterTable::ClusterTable(bool verbose) : table_{}, preflop_indexer_{},
                                           flop_indexer_{}, turn_indexer_{},
                                           river_indexer_{} {
  for (engine::RoundId i = 0; i < engine::kNRounds; ++i) {
    if (kClusterAssignmentFiles[i] == "") {
      table_[i].resize(engine::kImperfectRecallHands[i]);
      std::iota(table_[i].begin(), table_[i].end(), 0);
    } else {
      utils::CerealLoad(kClusterAssignmentFiles[i], &table_[i], verbose);
    }
  }
}

}  // namespace clustering
