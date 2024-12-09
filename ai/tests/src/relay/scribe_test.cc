#include <array>
#include <filesystem>
#include <functional>

#include "array/array.h"
#include "catch2/catch.hpp"
#include "clustering/matchmaker.h"
#include "clustering/test_clusters.h"
#include "mccfr/definitions.h"
#include "mccfr/strategy.h"
#include "poker/definitions.h"
#include "poker/node.h"
#include "relay/scribe.h"

TEST_CASE("scribe test", "[relay][scribe]") {
  constexpr fishbait::PlayerN kPlayers = 3;
  constexpr int kActions = 5;

  fishbait::Node<kPlayers> start_state;
  std::array<fishbait::AbstractAction, kActions> actions = {{
      {fishbait::Action::kFold},
      {fishbait::Action::kAllIn},
      {fishbait::Action::kCheckCall},

      {fishbait::Action::kBet, 2.0, 1, fishbait::Round::kTurn,
       fishbait::Round::kTurn, 2, 0},
      {fishbait::Action::kBet, 0.25, 1, fishbait::Round::kFlop,
       fishbait::Round::kRiver, 0, 10000}
  }};
  fishbait::TestClusters info_abstraction;
  int prune_constant = 0;
  int regret_floor = -10000;
  fishbait::Strategy s(start_state, actions, info_abstraction,
                       prune_constant, regret_floor);
  auto avg = s.InitialAverage();

  for (int i = 0; i < 10000; ++i) {
    for (fishbait::PlayerId p = 0; p < kPlayers; ++p) {
      s.TraverseMCCFR(p, false);
      s.UpdateStrategy(p);
    }
    avg += s;
  }
  avg.Normalize();

  std::filesystem::remove("out/tests/scribe_test.hdf");
  using ScribeT = fishbait::Scribe<kPlayers, kActions, fishbait::TestClusters>;
  ScribeT scribe{avg, "out/tests/scribe_test.hdf"};

  // Test attribute check throws
  using ScribeWrong1 = fishbait::Scribe<6, kActions, fishbait::TestClusters>;
  REQUIRE_THROWS(ScribeWrong1{"out/tests/scribe_test.hdf"});
  using ScribeWrong2 = fishbait::Scribe<kPlayers, 10, fishbait::TestClusters>;
  REQUIRE_THROWS(ScribeWrong2{"out/tests/scribe_test.hdf"});
  using ScribeWrong3 = fishbait::Scribe<9, 2, fishbait::TestClusters>;
  REQUIRE_THROWS(ScribeWrong3{"out/tests/scribe_test.hdf"});

  // Test StartState()
  INFO("test StartState()");
  REQUIRE(scribe.StartState() == avg.action_abstraction().start_state());

  // Test GetCluster()
  INFO("test GetCluster()");
  for (fishbait::RoundId rid = 0; rid < fishbait::kNRounds; ++rid) {
    fishbait::Round round{rid};
    INFO(rid);
    for (hand_index_t hidx = 0; hidx < fishbait::kImperfectRecallHands[rid];
         hidx = hidx + 100001) {
      INFO(hidx);
      REQUIRE(scribe.GetCluster(round, hidx) ==
              avg.info_abstraction().table()[rid][hidx]);
    }
  }

  // Test Bind and Matchmaker
  auto access_fn = std::bind(&ScribeT::GetCluster, &scribe,
                             std::placeholders::_1, std::placeholders::_2);
  fishbait::Matchmaker mm;
  fishbait::CardCluster test_cc = mm.Cluster(start_state, 0, access_fn);
  fishbait::CardCluster ref_cc =
      avg.info_abstraction().Cluster(start_state, 0);
  REQUIRE(test_cc == ref_cc);

  // Test Policy()
  INFO("test Policy()");
  for (fishbait::RoundId rid = 0; rid < fishbait::kNRounds; ++rid) {
    INFO(+rid);
    fishbait::Round round{rid};
    nda::size_t round_actions = avg.action_abstraction().ActionCount(round);
    for (fishbait::CardCluster cc = 0;
         cc < avg.info_abstraction().NumClusters(round); ++cc) {
      INFO(cc);
      for (fishbait::SequenceId seq = 0;
           seq < avg.action_abstraction().States(round); ++seq) {
        INFO(seq);
        std::array<float, kActions> ref_policy = avg.Policy(round, cc, seq);
        std::array<float, kActions> test_policy = scribe.Policy(round, cc, seq);
        for (std::size_t i = 0; i < round_actions; ++i) {
          INFO(i);
          REQUIRE(test_policy[i] == ref_policy[i]);
        }
      }
    }
  }

  // Test Actions()
  INFO("test Actions()");
  for (fishbait::RoundId rid = 0; rid < fishbait::kNRounds; ++rid) {
    fishbait::Round round{rid};
    nda::const_vector_ref<fishbait::AbstractAction> ref_actions =
        avg.action_abstraction().Actions(round);
    std::array<fishbait::AbstractAction, kActions> action_arr =
        scribe.Actions(round);
    nda::const_vector_ref<fishbait::AbstractAction>
        test_actions{action_arr.data(), ref_actions.size()};
    REQUIRE((ref_actions == test_actions));
  }

  // Test ActionCount()
  INFO("test ActionCount()");
  for (fishbait::RoundId rid = 0; rid < fishbait::kNRounds; ++rid) {
    fishbait::Round round{rid};
    nda::size_t ref_action_count = avg.action_abstraction().ActionCount(round);
    hsize_t test_action_count = scribe.ActionCount(round);
    REQUIRE(ref_action_count == test_action_count);
  }

  // Test Next()
  INFO("test Next()");
  for (fishbait::RoundId rid = 0; rid < fishbait::kNRounds; ++rid) {
    fishbait::Round round{rid};
    INFO(+rid);
    for (fishbait::SequenceId seq = 0;
         seq < avg.action_abstraction().States(round); ++seq) {
      INFO(seq);
      for (hsize_t action_idx = 0;
           action_idx < avg.action_abstraction().ActionCount(round);
           ++action_idx) {
        INFO(action_idx);
        fishbait::SequenceId ref_seq =
            avg.action_abstraction().Next(round, seq, action_idx);
        fishbait::SequenceId test_seq = scribe.Next(round, seq, action_idx);
        REQUIRE(ref_seq == test_seq);
      }
    }
  }
}  // TEST_CASE "scribe test"
