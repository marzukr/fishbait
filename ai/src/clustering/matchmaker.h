#ifndef AI_SRC_CLUSTERING_MATCHMAKER_H_
#define AI_SRC_CLUSTERING_MATCHMAKER_H_

#include <array>

#include "clustering/definitions.h"
#include "poker/definitions.h"
#include "poker/indexer.h"
#include "poker/node.h"

namespace fishbait {

class Matchmaker {
 private:
  Indexer<2> preflop_indexer_;
  Indexer<2, 3> flop_indexer_;
  Indexer<2, 4> turn_indexer_;
  Indexer<2, 5> river_indexer_;

 public:
  /*
    @brief Returns an array of cards clusters for the given node.

    The clusters are only calculated for non folded and non all-in players at
    the current betting round.
  */
  template <PlayerN kPlayers, typename AccessFn>
  std::array<CardCluster, kPlayers> ClusterArray(
      const Node<kPlayers>& node, AccessFn fn) const {
    std::array<CardCluster, kPlayers> card_clusters = {0};
    for (PlayerId i = 0; i < kPlayers; ++i) {
      if (!node.folded(i) && node.stack(i) != 0) {
        card_clusters[i] = Cluster<kPlayers, AccessFn>(node, i, fn);
      }
    }
    return card_clusters;
  }  // ClusterArray()

  /*
    @brief Returns the card cluster for the given player in the given node.
  */
  template <PlayerN kPlayers, typename AccessFn>
  CardCluster Cluster(const Node<kPlayers>& node, PlayerId player,
                      AccessFn fn) const {
    std::array player_cards = node.PlayerCards(player);
    hand_index_t idx;
    switch (node.round()) {
      case Round::kPreFlop:
        idx = preflop_indexer_.IndexLast(player_cards);
        break;
      case Round::kFlop:
        idx = flop_indexer_.IndexLast(player_cards);
        break;
      case Round::kTurn:
        idx = turn_indexer_.IndexLast(player_cards);
        break;
      case Round::kRiver:
        idx = river_indexer_.IndexLast(player_cards);
        break;
    }
    return fn(node.round(), idx);
  }  // Cluster()
};  // class Matchmaker

}  // namespace fishbait

#endif  // AI_SRC_CLUSTERING_MATCHMAKER_H_
