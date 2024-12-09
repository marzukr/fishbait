#ifndef AI_SRC_CLUSTERING_DEFINITIONS_H_
#define AI_SRC_CLUSTERING_DEFINITIONS_H_

#include <array>
#include <cstdint>
#include <string>

#include "poker/definitions.h"

namespace fishbait {

using MeanId = uint32_t;
using MeansN = MeanId;

using CardCluster = MeanId;

constexpr std::array<std::string_view, kNRounds>
    kClusterAssignmentFiles = {{"", "out/ai/clustering/flop_clusters.cereal",
                                "out/ai/clustering/turn_clusters.cereal",
                                "out/ai/clustering/river_clusters.cereal"}};
inline constexpr std::string_view ClusterAssignmentFile(Round r) {
  return kClusterAssignmentFiles[+r];
}

constexpr std::array<CardCluster, kNRounds> kNumClusters = {kUniqueHands, 200,
                                                            200, 200};
inline constexpr CardCluster NumClusters(Round r) {
  return kNumClusters[+r];
}

}  // namespace fishbait

#endif  // AI_SRC_CLUSTERING_DEFINITIONS_H_
