// Copyright 2021 Marzuk Rashid

#ifndef SRC_CLUSTERING_CLUSTER_TABLE_H_
#define SRC_CLUSTERING_CLUSTER_TABLE_H_

#include <array>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "clustering/definitions.h"
#include "engine/definitions.h"
#include "engine/indexer.h"
#include "engine/node.h"

namespace clustering {

class ClusterTable {
 private:
  std::array<std::vector<CardCluster>, engine::kNRounds> table_;
  engine::Indexer<2> preflop_indexer_;
  engine::Indexer<2, 3> flop_indexer_;
  engine::Indexer<2, 4> turn_indexer_;
  engine::Indexer<2, 5> river_indexer_;

 public:
  explicit ClusterTable(bool verbose = false);

  /*
    @brief Returns an array of cards clusters for the given node.

    The clusters are only calculated for non folded players at the current
    betting round.
  */
  template <engine::PlayerN kPlayers>
  std::array<CardCluster, kPlayers> ClusterArray(
      const engine::Node<kPlayers>& node) {
    std::array<CardCluster, kPlayers> card_clusters;
    for (engine::PlayerId i = 0; i < kPlayers; ++i) {
      if (!node.folded(i)) {
        card_clusters[i] = Cluster(node, i);
      }
    }
    return card_clusters;
  }  // ClusterArray()

  /*
    @brief Returns the card cluster for the given player in the given node.
  */
  template <engine::PlayerN kPlayers>
  CardCluster Cluster(const engine::Node<kPlayers>& node,
                      engine::PlayerId player) {
    std::array player_cards = node.PlayerCards(player);
    switch (node.round()) {
      case engine::Round::kPreFlop:
        return preflop_indexer_.IndexLast(player_cards);
      case engine::Round::kFlop:
        return table_[1][flop_indexer_.IndexLast(player_cards)];
      case engine::Round::kTurn:
        return table_[2][turn_indexer_.IndexLast(player_cards)];
      case engine::Round::kRiver:
        return table_[3][river_indexer_.IndexLast(player_cards)];
    }
    std::stringstream ss;
    ss << +(+node.round()) << " is not a valid round." << std::endl;
    throw std::out_of_range(ss.str());
  }
};  // class ClusterTable

}  // namespace clustering

#endif  // SRC_CLUSTERING_CLUSTER_TABLE_H_
