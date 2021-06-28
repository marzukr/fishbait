// Copyright 2021 Marzuk Rashid

#include "hand_strengths/lut_generators.h"

#include <algorithm>
#include <iostream>
#include <ostream>
#include <vector>

#include "array/array.h"
#include "array/matrix.h"
#include "SKPokerEval/src/SevenEval.h"
#include "deck/card_combinations.h"
#include "deck/card_utils.h"
#include "deck/constants.h"
#include "deck/indexer.h"
#include "hand_strengths/ochs.h"
#include "utils/combination_matrix.h"
#include "utils/timer.h"

namespace hand_strengths {

std::vector<ShowdownStrength> ShowdownLUT(const bool verbose) {
  if (verbose) {
    std::cout << "Generating Showdown LUT..." << std::endl;
  }

  utils::CombinationMatrix<uint8_t> op_clusters = SKClusterLUT();

  const uint32_t one_hand_approx = deck::kIsoRivers / deck::kUniqueHands;
  const uint32_t n_op_hands = 990;  // 45 choose 2

  std::vector<ShowdownStrength> showdown_lut(deck::kIsoRivers);

  deck::Indexer isocalc(2, {2, 5});
  std::array<uint8_t, 7> rollout;
  deck::CardCombinations op_hands(2);

  uint32_t sd_count = 0;
  utils::Timer t;
  for (uint32_t idx = 0; idx < deck::kIsoRivers; ++idx) {
    isocalc.unindex(1, idx, &rollout);
    std::transform(rollout.begin(), rollout.end(), rollout.begin(),
                   deck::ConvertISOtoSK);

    for (op_hands.Reset(rollout); !op_hands.is_done(); ++op_hands) {
      uint16_t hero_rank = SevenEval::GetRank(rollout[0], rollout[1],
          rollout[2], rollout[3], rollout[4], rollout[5], rollout[6]);
      uint16_t villan_rank = SevenEval::GetRank(op_hands(0), op_hands(1),
          rollout[2], rollout[3], rollout[4], rollout[5], rollout[6]);

      double win_value = 0;
      if (hero_rank > villan_rank) {
        win_value = 1;
      } else if (hero_rank == villan_rank) {
        win_value = 0.5;
      }

      uint8_t cluster = op_clusters(op_hands(0), op_hands(1));
      showdown_lut[idx].ehs += win_value;
      showdown_lut[idx].ochs_wins[cluster] += win_value;
      showdown_lut[idx].ochs_totals[cluster] += 1;
    }  // for op_hands

    showdown_lut[idx].ehs /= n_op_hands;

    sd_count += 1;
    if (verbose && sd_count % one_hand_approx == 0) {
      std::cout << 100.0 * sd_count / deck::kIsoRivers << "%" << std::endl;
      t.StopAndReset("iteration time");
    }
  }  // for idx

  return showdown_lut;
}  // ShowdownLUT()

nda::matrix<uint32_t> EHS_LUT(const uint32_t lut_size, const uint32_t buckets,
    const uint32_t simulation_cards, const uint32_t iso_round,
    deck::Indexer* isocalc, const std::vector<ShowdownStrength>& showdown_lut,
    const bool verbose) {

  const uint32_t simulation_size = deck::CardCombinations::N_Choose_K(
      52 - (7 - simulation_cards), simulation_cards);
  const uint32_t one_percent_approx = lut_size * simulation_size / 100;
  const double bucket_size = 1.0 / buckets;

  nda::matrix<uint32_t> ehs_lut(nda::matrix_shape<>(lut_size, buckets), 0);

  deck::Indexer showdown_calc(2, {2, 5});
  std::array<uint8_t, 7> rollout;
  std::array<uint64_t, 2> indicies;
  deck::CardCombinations simulations(simulation_cards);

  uint32_t sd_count = 0;
  utils::Timer t;
  for (uint32_t idx = 0; idx < lut_size; ++idx) {
    isocalc->unindex(iso_round, idx, &rollout);
    uint32_t not_simulated = 7 - simulation_cards;
    nda::vector_ref<uint8_t> hand{rollout.data(), not_simulated};

    for (simulations.Reset(hand); !simulations.is_done(); ++simulations) {
      for (uint32_t j = 0; j < simulation_cards; ++j) {
        rollout[not_simulated + j] = simulations(j);
      }

      // Get the appropriate showdown LUT index
      showdown_calc.index(rollout, &indicies);

      uint32_t bucket_unbounded = showdown_lut[indicies[1]].ehs / bucket_size;
      uint32_t bucket = std::min(bucket_unbounded, buckets - 1);

      ehs_lut(idx, bucket) += 1;

      sd_count += 1;
      if (verbose && sd_count % one_percent_approx == 0) {
        std::cout << 100.0 * sd_count / (lut_size * simulation_size)
                  << "%" << std::endl;
        t.StopAndReset("iteration time");
      }
    }  // for simulations
  }  // for idx

  return ehs_lut;
}  // EHS_LUT()

nda::matrix<uint32_t> PreflopLUT(
    const std::vector<ShowdownStrength>& showdown_lut, const bool verbose) {
  if (verbose) {
    std::cout << "Generating Preflop LUT..." << std::endl;
  }

  const uint32_t lut_size = deck::kUniqueHands;
  const uint32_t buckets = 50;
  const uint32_t simulation_size = 5;
  const uint32_t iso_round = 0;
  deck::Indexer isocalc(1, {2});
  return EHS_LUT(lut_size, buckets, simulation_size, iso_round, &isocalc,
                 showdown_lut, verbose);
}

nda::matrix<uint32_t> FlopLUT(
    const std::vector<ShowdownStrength>& showdown_lut, const bool verbose) {
  if (verbose) {
    std::cout << "Generating Flop LUT..." << std::endl;
  }

  const uint32_t lut_size = 1286792;
  const uint32_t buckets = 50;
  const uint32_t simulation_size = 2;
  const uint32_t iso_round = 1;
  deck::Indexer isocalc(2, {2, 3});
  return EHS_LUT(lut_size, buckets, simulation_size, iso_round, &isocalc,
                 showdown_lut, verbose);
}

nda::matrix<uint32_t> TurnLUT(
    const std::vector<ShowdownStrength>& showdown_lut, const bool verbose) {
  if (verbose) {
    std::cout << "Generating Turn LUT..." << std::endl;
  }

  const uint32_t lut_size = 13960050;
  const uint32_t buckets = 50;
  const uint32_t simulation_size = 1;
  const uint32_t iso_round = 1;
  deck::Indexer isocalc(2, {2, 4});
  return EHS_LUT(lut_size, buckets, simulation_size, iso_round, &isocalc,
                 showdown_lut, verbose);
}

nda::matrix<double> RiverLUT(
    const std::vector<ShowdownStrength>& showdown_lut, const bool verbose) {
  if (verbose) {
    std::cout << "Generating River LUT..." << std::endl;
  }

  const uint32_t one_percent_approx = deck::kIsoRivers / 100;
  nda::matrix<double> river_lut(nda::matrix_shape<>{deck::kIsoRivers, kOCHS_N},
                                0);

  utils::Timer t;
  for (uint32_t idx = 0; idx < deck::kIsoRivers; ++idx) {
    for (uint32_t j = 0; j < kOCHS_N; ++j) {
      river_lut(idx, j) = showdown_lut[idx].ochs_wins[j] /
                          showdown_lut[idx].ochs_totals[j];
    }

    if (verbose && idx % one_percent_approx == 0) {
      std::cout << 100.0 * idx / (deck::kIsoRivers * 1.0)
                << "%" << std::endl;
      t.StopAndReset("iteration time");
    }
  }  // for idx
  return river_lut;
}  // RiverLUT()

nda::matrix<double> OCHS_PreflopLUT(
    const std::vector<ShowdownStrength>& showdown_lut, const bool verbose) {
  if (verbose) {
    std::cout << "Generating OCHS Preflop LUT..." << std::endl;
  }

  const uint32_t simulation_size = deck::CardCombinations::N_Choose_K(50, 5);
  const uint32_t one_percent_approx = deck::kUniqueHands*simulation_size / 100;

  nda::matrix<double> ochs_preflop_lut(
      nda::matrix_shape<>(deck::kUniqueHands, kOCHS_N), 0);

  deck::Indexer showdown_calc(2, {2, 5});
  std::array<uint8_t, 7> rollout;
  std::array<uint64_t, 2> indicies;
  std::array<uint32_t, kOCHS_N> sim_totals;
  deck::CardCombinations simulations(5);

  uint32_t sd_count = 0;
  utils::Timer t;
  for (uint32_t idx = 0; idx < deck::kUniqueHands; ++idx) {
    showdown_calc.unindex(0, idx, &rollout);
    nda::vector_ref<uint8_t> hand(rollout.data(), 2);
    std::fill(sim_totals.begin(), sim_totals.end(), 0);

    for (simulations.Reset(hand); !simulations.is_done(); ++simulations) {
      for (uint32_t j = 0; j < 5; ++j) {
        rollout[2 + j] = simulations(j);
      }

      // Get the appropriate showdown LUT index
      showdown_calc.index(rollout, &indicies);

      for (uint32_t k = 0; k < kOCHS_N; ++k) {
        ochs_preflop_lut(idx, k) += showdown_lut[indicies[1]].ochs_wins[k];
        sim_totals[k] += showdown_lut[indicies[1]].ochs_totals[k];
      }

      sd_count += 1;
      if (verbose && sd_count % one_percent_approx == 0) {
        std::cout << 100.0 * sd_count / (deck::kUniqueHands * simulation_size)
                  << "%" << std::endl;
        t.StopAndReset("iteration time");
      }
    }  // for simulations

    for (uint32_t k = 0; k < kOCHS_N; ++k) {
      ochs_preflop_lut(idx, k) /= sim_totals[k];
    }
  }  // for idx

  return ochs_preflop_lut;
}  // OCHS_PreflopLUT

}  // namespace hand_strengths
