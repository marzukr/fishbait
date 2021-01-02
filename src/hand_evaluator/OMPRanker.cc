// Copyright 2021 Marzuk Rashid

#include "hand_evaluator/OMPRanker.h"

#include <stdint.h>

#include "OMPEval/omp/HandEvaluator.h"

namespace hand_evaluator {

uint16_t OMPRanker::RankHand(uint8_t i, uint8_t j, uint8_t k, uint8_t m,
                              uint8_t n, uint8_t p, uint8_t q) {
  omp::Hand h = omp::Hand::empty();
  h += omp::Hand(i) + omp::Hand(j) + omp::Hand(k) + omp::Hand(m)
     + omp::Hand(n) + omp::Hand(p) + omp::Hand(q);
  return eval.evaluate(h);
}

}  // namespace hand_evaluator
