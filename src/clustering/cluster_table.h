// Copyright 2021 Marzuk Rashid

#ifndef SRC_CLUSTERING_CLUSTER_TABLE_H_
#define SRC_CLUSTERING_CLUSTER_TABLE_H_

#include <array>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "clustering/definitions.h"
#include "poker/definitions.h"
#include "poker/indexer.h"
#include "poker/node.h"
#include "utils/cereal.h"

namespace fishbait {

class ClusterTable {
 private:
  std::array<std::vector<CardCluster>, kNRounds> table_;
  Indexer<2> preflop_indexer_;
  Indexer<2, 3> flop_indexer_;
  Indexer<2, 4> turn_indexer_;
  Indexer<2, 5> river_indexer_;

 public:
  explicit ClusterTable(bool verbose = false);
  ClusterTable(const ClusterTable& other);
  ClusterTable& operator=(const ClusterTable& other);

  /*
    @brief Returns the number of card clusters in the given round.
  */
  static constexpr CardCluster NumClusters(Round r) {
    return kNumClusters[+r];
  }

  /*
    @brief Returns an array of cards clusters for the given node.

    The clusters are only calculated for non folded and non all-in players at
    the current betting round.
  */
  template <PlayerN kPlayers>
  std::array<CardCluster, kPlayers> ClusterArray(const Node<kPlayers>& node) {
    std::array<CardCluster, kPlayers> card_clusters;
    for (PlayerId i = 0; i < kPlayers; ++i) {
      if (!node.folded(i) && node.stack(i) != 0) {
        card_clusters[i] = Cluster(node, i);
      }
    }
    return card_clusters;
  }  // ClusterArray()

  /*
    @brief Returns the card cluster for the given player in the given node.
  */
  template <PlayerN kPlayers>
  CardCluster Cluster(const Node<kPlayers>& node, PlayerId player) {
    std::array player_cards = node.PlayerCards(player);
    switch (node.round()) {
      case Round::kPreFlop:
        return preflop_indexer_.IndexLast(player_cards);
      case Round::kFlop:
        return table_[1][flop_indexer_.IndexLast(player_cards)];
      case Round::kTurn:
        return table_[2][turn_indexer_.IndexLast(player_cards)];
      case Round::kRiver:
        return table_[3][river_indexer_.IndexLast(player_cards)];
    }
    std::stringstream ss;
    ss << +(+node.round()) << " is not a valid round." << std::endl;
    throw std::out_of_range(ss.str());
  }

  /* @brief ClusterTable serialize function */
  template<class Archive>
  void serialize(Archive& archive) {
    archive(table_);
  }
};  // class ClusterTable

}  // namespace fishbait

#endif  // SRC_CLUSTERING_CLUSTER_TABLE_H_
