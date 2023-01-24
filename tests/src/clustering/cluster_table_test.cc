#include <type_traits>

#include "catch2/catch.hpp"
#include "clustering/cluster_table.h"
#include "poker/node.h"

TEST_CASE("ClusterTable bounds check", "[.][clustering][cluster_table]") {
  constexpr fishbait::PlayerN kPlayers = 6;
  fishbait::ClusterTable table{false};
  fishbait::Node<kPlayers> game;
  for (fishbait::RoundId round = 0; round < fishbait::kNRounds; ++round) {
    for (int trial = 0; trial < 100; ++trial) {
      game.Deal();
      std::array<fishbait::CardCluster, kPlayers> clusters =
          table.ClusterArray(game);
      for (std::size_t j = 0; j < clusters.size(); ++j) {
        REQUIRE(clusters[j] < fishbait::NumClusters(game.round()));
        static_assert(!std::is_signed_v<fishbait::CardCluster>);
      }
    }
    for (fishbait::PlayerId i = 0; i < kPlayers; ++i) {
      game.Apply(fishbait::Action::kCheckCall);
    }
  }
}
