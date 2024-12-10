#ifndef AI_SRC_HAND_STRENGTHS_LUT_FILES_H_
#define AI_SRC_HAND_STRENGTHS_LUT_FILES_H_

#include <vector>

#include "array/matrix.h"
#include "hand_strengths/definitions.h"
#include "utils/cereal.h"

namespace fishbait {

inline void ShowdownLUT_File(FileAction action,
                             std::vector<ShowdownStrength>* data_points,
                             bool verbose = false) {
  Cereal(
    action,
    "out/ai/hand_strengths/showdown_lut_vector.cereal",
    data_points,
    verbose
  );
}

inline void PreflopLUT_File(FileAction action,
                            nda::matrix<HistCount>* data_points,
                            bool verbose = false) {
  Cereal(
    action,
    "out/ai/hand_strengths/preflop_lut_nda.cereal",
    data_points,
    verbose
  );
}

inline void FlopLUT_File(FileAction action,
                         nda::matrix<HistCount>* data_points,
                         bool verbose = false) {
  Cereal(action, "out/ai/hand_strengths/flop_lut_nda.cereal", data_points,
         verbose);
}

inline void TurnLUT_File(FileAction action,
                         nda::matrix<HistCount>* data_points,
                         bool verbose = false) {
  Cereal(action, "out/ai/hand_strengths/turn_lut_nda.cereal", data_points,
         verbose);
}

inline void RiverLUT_File(FileAction action,
                          nda::matrix<double>* data_points,
                          bool verbose = false) {
  Cereal(action, "out/ai/hand_strengths/river_lut_nda.cereal", data_points,
         verbose);
}

inline void OCHS_PreflopLUT_File(FileAction action,
                                 nda::matrix<double>* data_points,
                                 bool verbose = false) {
  Cereal(
    action,
    "out/ai/hand_strengths/ochs_preflop_lut_nda.cereal",
    data_points,
    verbose
  );
}

}  // namespace fishbait

#endif  // AI_SRC_HAND_STRENGTHS_LUT_FILES_H_
