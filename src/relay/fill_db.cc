// Copyright 2021 Marzuk Rashid

#include <array>
#include <functional>
#include <iostream>
#include <string>

#include "array/array.h"
#include "pqxx/pqxx"
#include "blueprint/hyperparameters.h"
#include "blueprint/strategy.h"
#include "clustering/cluster_table.h"
#include "clustering/definitions.h"
#include "poker/definitions.h"
#include "utils/config.h"
#include "utils/thread.h"

namespace pqxx {

template<typename T>
std::string const type_name<nda::vector_ref<T>>{"nda::vector_ref"};  // NOLINT(*)

template<typename T>
struct nullness<nda::vector_ref<T>> : pqxx::no_null<nda::vector_ref<T>>
{};

template<typename T>
struct string_traits<nda::vector_ref<T>>
        : internal::array_string_traits<nda::vector_ref<T>>
{};

}  // namespace pqxx

using AverageT = typename fishbait::Strategy<fishbait::hparam::kPlayers,
                                             fishbait::hparam::kActions,
                                             fishbait::ClusterTable>::Average;

void CommitClusters(fishbait::RoundId r, fishbait::CardCluster start,
                    fishbait::CardCluster end, const AverageT& bp) {
  fishbait::Round round{r};
  fishbait::SequenceN seqs = bp.action_abstraction().States(round);
  std::size_t action_count = bp.action_abstraction().ActionCount(round);
  int64_t prev_round_states = 0;
  for (fishbait::RoundId pr = 0; pr < r; ++pr) {
    fishbait::Round pround{pr};
    prev_round_states +=
        static_cast<int64_t>(bp.action_abstraction().States(pround)) *
        static_cast<int64_t>(fishbait::NumClusters(pround));
  }
  int64_t game_state_id = prev_round_states +
      (static_cast<int64_t>(seqs) * static_cast<int64_t>(start));

  for (fishbait::CardCluster cluster = start; cluster < end; ++cluster) {
    std::cout << round << " cluster " << cluster << " started" << std::endl;
    pqxx::connection c(fishbait::kDbCxnStr.data());
    pqxx::work w(c);
    pqxx::stream_to stream = pqxx::stream_to::table(w, {"blueprint"});

    for (fishbait::SequenceId s = 0; s < seqs; ++s) {
      std::array<float, fishbait::hparam::kActions> policy =
          bp.Policy(round, cluster, s);
      nda::vector_ref<float> policy_trunc{policy.data(), action_count};
      stream.write_values(game_state_id, policy_trunc);
      ++game_state_id;
    }

    stream.complete();
    w.commit();
    std::cout << round << " cluster " << cluster << " commited" << std::endl;
  }
}

int main() {
  AverageT bp = AverageT::LoadAverage(fishbait::hparam::kAvgPath, true);

  for (fishbait::RoundId r = 0; r < fishbait::kNRounds; ++r) {
    auto worker = std::bind(CommitClusters, r, std::placeholders::_1,
                            std::placeholders::_2, std::cref(bp));
    fishbait::Round round{r};
    fishbait::DivideWork(fishbait::NumClusters(round), worker);
  }

  return 0;
}
