// Copyright 2021 Marzuk Rashid

#ifndef SRC_HAND_STRENGTHS_LUT_GENERATORS_H_
#define SRC_HAND_STRENGTHS_LUT_GENERATORS_H_

#include <vector>

#include "array/array.h"
#include "array/matrix.h"
#include "hand_strengths/ochs.h"

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
nda::matrix<uint32_t> EHS_LUT(const uint32_t lut_size, const uint32_t buckets,
    const uint32_t simulation_size, const uint32_t lut_round,
    const std::vector<ShowdownStrength>& showdown_lut,
    const bool verbose = false);

nda::matrix<uint32_t> PreflopLUT(
    const std::vector<ShowdownStrength>& showdown_lut,
    const bool verbose = false);

nda::matrix<uint32_t> FlopLUT(
    const std::vector<ShowdownStrength>& showdown_lut,
    const bool verbose = false);

nda::matrix<uint32_t> TurnLUT(
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
