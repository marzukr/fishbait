// Copyright 2021 Marzuk Rashid

#ifndef SRC_CLUSTERING_CLUSTER_TABLE_H_
#define SRC_CLUSTERING_CLUSTER_TABLE_H_

#include <array>
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
  std::array<CardCluster, kPlayers> Clusters(
      const engine::Node<kPlayers>& node) {
    std::array<CardCluster, kPlayers> card_clusters;
    for (engine::PlayerId i = 0; i < kPlayers; ++i) {
      if (!node.folded(i)) {
        std::array player_cards = node.PlayerCards(i);
        hand_index_t idx;
        switch (node.round()) {
          case engine::Round::kPreFlop:
            card_clusters[i] = preflop_indexer_.IndexLast(player_cards);
            break;
          case engine::Round::kFlop:
            idx = flop_indexer_.IndexLast(player_cards);
            card_clusters[i] = table_[1][idx];
            break;
          case engine::Round::kTurn:
            idx = turn_indexer_.IndexLast(player_cards);
            card_clusters[i] = table_[2][idx];
            break;
          case engine::Round::kRiver:
            idx = river_indexer_.IndexLast(player_cards);
            card_clusters[i] = table_[3][idx];
            break;
        }  // switch node.round()
      }
    }  // for i
    return card_clusters;
  }  // Clusters()
};  // class ClusterTable

}  // namespace clustering

#endif  // SRC_CLUSTERING_CLUSTER_TABLE_H_
