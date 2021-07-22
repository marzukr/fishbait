// Copyright 2021 Marzuk Rashid

#ifndef SRC_CLUSTERING_DEFINITIONS_H_
#define SRC_CLUSTERING_DEFINITIONS_H_

#include <array>
#include <cstdint>
#include <string>

#include "engine/definitions.h"

namespace clustering {

using MeanId = uint32_t;
using MeansN = MeanId;

using CardCluster = MeanId;

constexpr std::array<std::string_view, engine::kNRounds>
    kClusterAssignmentFiles = {{"", "out/clustering/flop_clusters.cereal",
                                "out/clustering/turn_clusters.cereal",
                                "out/clustering/river_clusters.cereal"}};
inline constexpr std::string_view ClusterAssignmentFile(engine::Round r) {
  return kClusterAssignmentFiles[engine::GetRoundId(r)];
}

}  // namespace clustering

#endif  // SRC_CLUSTERING_DEFINITIONS_H_
