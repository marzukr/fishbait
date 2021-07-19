// Copyright 2021 Marzuk Rashid

#ifndef SRC_HAND_STRENGTHS_LUT_FILES_H_
#define SRC_HAND_STRENGTHS_LUT_FILES_H_

#include <vector>

#include "array/matrix.h"
#include "hand_strengths/definitions.h"
#include "utils/cereal.h"

namespace hand_strengths {

inline void ShowdownLUT_File(utils::FileAction action,
                             std::vector<ShowdownStrength>* data_points,
                             bool verbose = false) {
  utils::Cereal(action, "out/hand_strengths/showdown_lut_vector.cereal",
                data_points, verbose);
}

inline void PreflopLUT_File(utils::FileAction action,
                            nda::matrix<HistCount>* data_points,
                            bool verbose = false) {
  utils::Cereal(action, "out/hand_strengths/preflop_lut_nda.cereal",
                data_points, verbose);
}

inline void FlopLUT_File(utils::FileAction action,
                         nda::matrix<HistCount>* data_points,
                         bool verbose = false) {
  utils::Cereal(action, "out/hand_strengths/flop_lut_nda.cereal", data_points,
                verbose);
}

inline void TurnLUT_File(utils::FileAction action,
                         nda::matrix<HistCount>* data_points,
                         bool verbose = false) {
  utils::Cereal(action, "out/hand_strengths/turn_lut_nda.cereal", data_points,
                verbose);
}

inline void RiverLUT_File(utils::FileAction action,
                          nda::matrix<double>* data_points,
                          bool verbose = false) {
  utils::Cereal(action, "out/hand_strengths/river_lut_nda.cereal", data_points,
                verbose);
}

inline void OCHS_PreflopLUT_File(utils::FileAction action,
                                 nda::matrix<double>* data_points,
                                 bool verbose = false) {
  utils::Cereal(action, "out/hand_strengths/ochs_preflop_lut_nda.cereal",
                data_points, verbose);
}

}  // namespace hand_strengths

#endif  // SRC_HAND_STRENGTHS_LUT_FILES_H_
