// Copyright 2021 Marzuk Rashid

#ifndef SRC_HAND_EVALUATOR_OMPRANKER_H_
#define SRC_HAND_EVALUATOR_OMPRANKER_H_

#include <stdint.h>

#include "OMPEval/omp/HandEvaluator.h"

namespace hand_evaluator {

class OMPRanker {
 public:
  uint16_t RankHand(uint8_t i, uint8_t j, uint8_t k, uint8_t m,
                    uint8_t n, uint8_t p, uint8_t q);

 private:
  omp::HandEvaluator eval;
};

}  // namespace hand_evaluator

#endif  // SRC_HAND_EVALUATOR_OMPRANKER_H_
