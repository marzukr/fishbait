// Copyright 2021 Marzuk Rashid

#ifndef SRC_HAND_STRENGTHS_LUT_GENERATORS_H_
#define SRC_HAND_STRENGTHS_LUT_GENERATORS_H_

#include <cstdint>
#include <vector>

#include "array/array.h"
#include "array/matrix.h"
#include "deck/definitions.h"
#include "deck/indexer.h"
#include "hand_strengths/definitions.h"
#include "hand_strengths/ochs.h"
#include "poker_engine/definitions.h"

namespace hand_strengths {

std::vector<ShowdownStrength> ShowdownLUT(const bool verbose = false);

/*
  @brief Generic EHS LUT generating function.

  @param lut_size The number of rows in the LUT to generate.
  @param buckets The number of buckets per row in the LUT to generate.
  @param simulation_cards The number of cards to simulate per row in the LUT.
  @param iso_round The isocalc round that the LUT should generate rows from.
  @param isocalc The Indexer object to generate rows from.
  @param showdown_lut The showdown LUT to use.
  @param verbose Option to print progress.
*/
nda::matrix<HistCount> EHS_LUT(const hand_index_t lut_size,
    const HistBucketN buckets, const deck::CardN simulation_cards,
    const poker_engine::RoundN iso_round, deck::Indexer* isocalc,
    const std::vector<ShowdownStrength>& showdown_lut,
    const bool verbose = false);

nda::matrix<HistCount> PreflopLUT(
    const std::vector<ShowdownStrength>& showdown_lut,
    const bool verbose = false);

nda::matrix<HistCount> FlopLUT(
    const std::vector<ShowdownStrength>& showdown_lut,
    const bool verbose = false);

nda::matrix<HistCount> TurnLUT(
    const std::vector<ShowdownStrength>& showdown_lut,
    const bool verbose = false);

nda::matrix<double> RiverLUT(
    const std::vector<ShowdownStrength>& showdown_lut,
    const bool verbose = false);

nda::matrix<double> OCHS_PreflopLUT(
    const std::vector<ShowdownStrength>& showdown_lut,
    const bool verbose = false);

}  // namespace hand_strengths

#endif  // SRC_HAND_STRENGTHS_LUT_GENERATORS_H_
