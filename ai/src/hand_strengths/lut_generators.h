#ifndef AI_SRC_HAND_STRENGTHS_LUT_GENERATORS_H_
#define AI_SRC_HAND_STRENGTHS_LUT_GENERATORS_H_

#include <cstdint>
#include <vector>

#include "array/array.h"
#include "array/matrix.h"
#include "hand_strengths/definitions.h"
#include "hand_strengths/ochs.h"
#include "poker/definitions.h"
#include "poker/indexer.h"

namespace fishbait {

std::vector<ShowdownStrength> ShowdownLUT(const bool verbose = false);

/*
  @brief Generic EHS LUT generating function.

  @param kLUTSize The number of rows in the LUT to generate.
  @param kBuckets The number of buckets per row in the LUT to generate.
  @param kSimulationCards The number of cards to simulate per row in the LUT.
  @param kISORound The isocalc round that the LUT should generate rows from.
  @param IndexerT The indexer type to use.
  @param isocalc The Indexer object to generate rows from.
  @param showdown_lut The showdown LUT to use.
  @param verbose Option to print progress.
*/
template <hand_index_t kLUTSize, HistBucketN kBuckets, CardN kSimulationCards,
          RoundN kISORound, typename IndexerT>
nda::matrix<HistCount> EHS_LUT(IndexerT& isocalc,
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

}  // namespace fishbait

#endif  // AI_SRC_HAND_STRENGTHS_LUT_GENERATORS_H_
