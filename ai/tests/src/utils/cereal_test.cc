#include <cstdint>
#include <filesystem>
#include <iostream>
#include <vector>
#include <memory>

#include "array/array.h"
#include "catch2/catch.hpp"
#include "utils/cereal.h"
#include "utils/random.h"
#include "poker/definitions.h"
#include "poker/node.h"
#include "clustering/test_clusters.h"
#include "mccfr/strategy.h"
#include "relay/commander.h"

TEST_CASE("Save and load vector test", "[utils][cereal]") {
  const uint32_t n = 100;

  std::vector<double> save_vect(n);
  for (uint32_t i = 0; i < n; ++i) {
    save_vect[i] = i + i / 10.0;
  }
  std::filesystem::remove("out/tests/vector_save_test.cereal");
  fishbait::Cereal(fishbait::FileAction::Save,
                   "out/tests/vector_save_test.cereal", &save_vect);
  std::string string_save = fishbait::CerealSave(&save_vect);

  std::vector<double> load_vect;
  fishbait::CerealLoad("out/tests/vector_save_test.cereal", &load_vect);
  std::vector<double> string_load_vect;
  fishbait::CerealLoad(
    string_save.data(), string_save.length(), &string_load_vect
  );
  REQUIRE(load_vect.size() == n);
  REQUIRE(string_load_vect.size() == n);
  for (uint32_t i = 0; i < n; ++i) {
    REQUIRE(load_vect[i] == (i + i / 10.0));
    REQUIRE(string_load_vect[i] == (i + i / 10.0));
  }
}  // TEST_CASE "Save and load vector test"

TEST_CASE("Save and load nda test", "[utils][cereal]") {
  const uint32_t n = 100;
  const uint32_t m = 100;

  using dense_col_t = nda::shape<nda::dim<>, nda::dense_dim<>>;
  nda::array<int, dense_col_t> save_matrix({n, m});
  for (uint32_t i = 0; i < n; ++i) {
    for (uint32_t j = 0; j < m; ++j) {
      save_matrix(i, j) = i + j;
    }
  }
  std::filesystem::remove("out/tests/nda_save_test.cereal");
  fishbait::CerealSave("out/tests/nda_save_test.cereal", &save_matrix);
  std::string string_save = fishbait::CerealSave(&save_matrix);

  nda::array<int, dense_col_t> load_matrix({1, 1});
  fishbait::Cereal(fishbait::FileAction::Load, "out/tests/nda_save_test.cereal",
                   &load_matrix);
  REQUIRE(load_matrix.rows() == n);
  REQUIRE(load_matrix.columns() == m);
  nda::array<int, dense_col_t> string_load_matrix({1, 1});
  fishbait::CerealLoad(
    string_save.data(), string_save.length(), &string_load_matrix
  );
  REQUIRE(string_load_matrix.rows() == n);
  REQUIRE(string_load_matrix.columns() == m);
  for (uint32_t i = 0; i < n; ++i) {
    for (uint32_t j = 0; j < m; ++j) {
      REQUIRE(load_matrix(i, j) == (int)(i + j));
      REQUIRE(string_load_matrix(i, j) == (int)(i + j));
    }
  }
  REQUIRE(load_matrix == save_matrix);
}  // TEST_CASE "Save and load nda test"

TEST_CASE("Save and load std::filesystem::path", "[utils][cereal]") {
  std::filesystem::path save_path = "out/tests/path_save_test.cereal";
  std::filesystem::remove(save_path);
  fishbait::Cereal(fishbait::FileAction::Save, save_path.string(), &save_path);

  std::filesystem::path load_path;
  fishbait::CerealLoad(save_path.string(), &load_path);

  REQUIRE(load_path == save_path);
}  // TEST_CASE "Save and load std::filesystem::path"

TEST_CASE("Save and load Random", "[utils][cereal]") {
  std::filesystem::path save_path = "out/tests/random_save_test.cereal";
  std::filesystem::remove(save_path);
  fishbait::Random rng(fishbait::Random::Seed(8));
  rng()();

  fishbait::CerealSave(save_path.string(), &rng);
  std::string string_save = fishbait::CerealSave(&rng);

  fishbait::Random load_rng(fishbait::Random::Seed(10));
  fishbait::Cereal(fishbait::FileAction::Load, save_path.string(), &load_rng);
  fishbait::Random string_load_rng(fishbait::Random::Seed(10));
  fishbait::CerealLoad(
    string_save.data(), string_save.length(), &string_load_rng
  );

  for (int i = 0; i < 100; ++i) {
    std::size_t sampled = rng()();
    REQUIRE(sampled == load_rng()());
    REQUIRE(sampled == string_load_rng()());
  }
  REQUIRE(rng() == load_rng());
  REQUIRE(rng() == string_load_rng());
}  // TEST_CASE "Save and load Random"

TEST_CASE("Save and load Commander", "[utils][cereal]") {
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

  std::filesystem::remove("out/tests/commander_cereal.hdf");
  using CommanderT = fishbait::Commander<kPlayers, kActions,
                                         fishbait::TestClusters>;
  std::unique_ptr<CommanderT> napoleon = std::make_unique<CommanderT>(
    CommanderT::ScribeT{avg, "out/tests/commander_cereal.hdf"}
  );
  std::string string_save = fishbait::CerealSave(&napoleon);

  std::unique_ptr<CommanderT> napoleon_load;
  fishbait::CerealLoad(
    string_save.data(), string_save.length(), &napoleon_load
  );

  std::array<float, kActions> orig_policy = (
    napoleon->GetNormalizedLegalPolicy()
  );
  std::array<float, kActions> saved_policy = (
    napoleon_load->GetNormalizedLegalPolicy()
  );
  std::array<CommanderT::AvailableAction, kActions> orig_actions = (
    napoleon->GetAvailableActions()
  );
  std::array<CommanderT::AvailableAction, kActions> saved_actions = (
    napoleon_load->GetAvailableActions()
  );
  for (std::size_t i = 0; i < kActions; ++i) {
    REQUIRE(orig_policy[i] == saved_policy[i]);
    if (orig_policy[i] != 0) {
      REQUIRE(orig_actions[i] == saved_actions[i]);
    }
  }
  REQUIRE(napoleon->State() == napoleon_load->State());
  REQUIRE(napoleon->fishbait_seat() == napoleon_load->fishbait_seat());
}  // TEST_CASE "Save and load Commander"
