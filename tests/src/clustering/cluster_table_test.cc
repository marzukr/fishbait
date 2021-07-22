// Copyright 2021 Marzuk Rashid

#include "catch2/catch.hpp"
#include "clustering/cluster_table.h"
#include "engine/node.h"

TEST_CASE("ClusterTable bounds check", "[.][clustering][cluster_table]") {
  clustering::ClusterTable table;
  engine::Node<2> game;
  for (engine::RoundId round = 0; round < engine::kNRounds; ++round) {
    for (int trial = 0; trial < 100; ++trial) {
      game.DealCards();
      std::array clusters = table.Clusters(game);
      static_assert(clusters.size() == 2);
      for (std::size_t j = 0; j < clusters.size(); ++j) {
        REQUIRE(clusters[j] < clustering::NumClusters(game.round()));
        REQUIRE(clusters[j] >= 0);
      }
    }
    game.Apply(engine::Action::kCheckCall);
    game.Apply(engine::Action::kCheckCall);
  }
}
