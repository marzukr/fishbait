// Copyright 2021 Marzuk Rashid

#ifndef SRC_CLUSTERING_DEFINITIONS_H_
#define SRC_CLUSTERING_DEFINITIONS_H_

#include <array>
#include <cstdint>
#include <string>

#include "poker/definitions.h"

namespace fishbait {

using MeanId = uint32_t;
using MeansN = MeanId;

using CardCluster = MeanId;

constexpr std::array<std::string_view, kNRounds>
    kClusterAssignmentFiles = {{"", "out/clustering/flop_clusters.cereal",
                                "out/clustering/turn_clusters.cereal",
                                "out/clustering/river_clusters.cereal"}};
inline constexpr std::string_view ClusterAssignmentFile(Round r) {
  return kClusterAssignmentFiles[+r];
}

constexpr std::array<CardCluster, kNRounds> kNumClusters = {kUniqueHands, 200,
                                                            200, 200};
inline constexpr CardCluster NumClusters(Round r) {
  return kNumClusters[+r];
}

}  // namespace fishbait

#endif  // SRC_CLUSTERING_DEFINITIONS_H_
