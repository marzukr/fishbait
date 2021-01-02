#ifndef FISHBAIT_HANDEVALUATOR_OMPRANKER
#define FISHBAIT_HANDEVALUATOR_OMPRANKER

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

}

#endif