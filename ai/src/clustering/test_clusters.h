#ifndef AI_SRC_CLUSTERING_TEST_CLUSTERS_H_
#define AI_SRC_CLUSTERING_TEST_CLUSTERS_H_

#include <array>
#include <vector>

#include "clustering/definitions.h"
#include "poker/definitions.h"
#include "poker/indexer.h"
#include "poker/node.h"

namespace fishbait {

/* Dummy clustering scheme that just partitions each round into 4 clusters with
   modulus of the ISO card index. */
class TestClusters {
 private:
  std::array<std::vector<CardCluster>, kNRounds> table_;
  Indexer<2> preflop_indexer_;
  Indexer<2, 3> flop_indexer_;
  Indexer<2, 4> turn_indexer_;
  Indexer<2, 5> river_indexer_;
  static constexpr CardCluster kNClusters = 4;

 public:
  TestClusters() {
    for (RoundId rid = 0; rid < kNRounds; ++rid) {
      table_[rid].resize(kImperfectRecallHands[rid]);
      for (hand_index_t i = 0; i < kImperfectRecallHands[rid]; ++i) {
        table_[rid][i] = i % kNClusters;
      }
    }
  }
  TestClusters(const TestClusters& other) : table_{other.table_} {};
  TestClusters& operator=(const TestClusters& other) {
    table_ = other.table_;
    return *this;
  }
  bool operator==(const TestClusters&) const { return true; }
  bool operator!=(const TestClusters&) const { return false; }

  static TestClusters BlankTable() {
    return TestClusters();
  }

  /* @brief TestClusters serialize function */
  template<class Archive>
  void serialize(Archive&) {
    return;
  }

  static constexpr CardCluster NumClusters(Round) {
    return kNClusters;
  }

  template <PlayerN kPlayers>
  std::array<CardCluster, kPlayers> ClusterArray(
      const Node<kPlayers>& node) const {
    std::array<CardCluster, kPlayers> card_clusters;
    for (PlayerId i = 0; i < kPlayers; ++i) {
      if (!node.folded(i) && node.stack(i) != 0) {
        card_clusters[i] = Cluster(node, i);
      }
    }
    return card_clusters;
  }  // ClusterArray()

  template <PlayerN kPlayers>
  CardCluster Cluster(const Node<kPlayers>& node, PlayerId player) const {
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
      default:
        std::stringstream ss;
        ss << +(+node.round()) << " is not a valid round." << std::endl;
        throw std::out_of_range(ss.str());
    }
    return idx % kNClusters;
  }

  /* @brief Returns a const reference to the raw table. */
  const auto& table() const { return table_; }
};  // class TestClusters

}  // namespace fishbait

#endif  // AI_SRC_CLUSTERING_TEST_CLUSTERS_H_
