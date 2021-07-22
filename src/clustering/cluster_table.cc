// Copyright 2021 Marzuk Rashid

#include "clustering/cluster_table.h"

#include "clustering/definitions.h"
#include "engine/definitions.h"
#include "utils/cereal.h"

namespace clustering {

ClusterTable::ClusterTable(bool verbose) : table_{}, preflop_indexer_{},
                                           flop_indexer_{}, turn_indexer_{},
                                           river_indexer_{} {
  for (engine::RoundId i = 0; i < engine::kNRounds; ++i) {
    utils::CerealLoad(kClusterAssignmentFiles[i], &table_[i], verbose);
  }
}

}  // namespace clustering
