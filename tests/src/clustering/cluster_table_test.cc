// Copyright 2021 Marzuk Rashid

#include <type_traits>

#include "catch2/catch.hpp"
#include "clustering/cluster_table.h"
#include "engine/node.h"

TEST_CASE("ClusterTable bounds check", "[.][clustering][cluster_table]") {
  constexpr engine::PlayerN kPlayers = 6;
  clustering::ClusterTable table;
  engine::Node<kPlayers> game;
  for (engine::RoundId round = 0; round < engine::kNRounds; ++round) {
    for (int trial = 0; trial < 100; ++trial) {
      game.DealCards();
      std::array<clustering::CardCluster, kPlayers> clusters =
          table.ClusterArray(game);
      for (std::size_t j = 0; j < clusters.size(); ++j) {
        REQUIRE(clusters[j] < clustering::NumClusters(game.round()));
        static_assert(!std::is_signed_v<clustering::CardCluster>);
      }
    }
    for (engine::PlayerId i = 0; i < kPlayers; ++i) {
      game.Apply(engine::Action::kCheckCall);
    }
  }
}
