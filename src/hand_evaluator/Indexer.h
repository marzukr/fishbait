// Copyright 2021 Marzuk Rashid

#ifndef SRC_HAND_EVALUATOR_INDEXER_H_
#define SRC_HAND_EVALUATOR_INDEXER_H_

#include <stdint.h>

#include <boost/python.hpp>
extern "C" {
  #include "hand-isomorphism/src/hand_index.h"
}

namespace hand_evaluator {

class Indexer {
 public:
  Indexer(int rounds, const boost::python::list& cpr);
  ~Indexer() { delete isocalc; }

  hand_index_t Index(const boost::python::list& cards, bool is_omp);

  static uint8_t ConvertSKtoISO(uint8_t sk_card) {
    uint8_t res = 51 - sk_card;
    uint8_t rem = sk_card % 4;
    return res + (rem - 3 + rem);
  }

  static uint8_t ConvertOMPtoISO(uint8_t omp_card) {
    uint8_t rem = omp_card % 4;
    if (rem == 2) return omp_card + 1;
    else if (rem == 3) return omp_card - 1;
    return omp_card;
  }

 private:
  hand_indexer_t* isocalc;
};

}  // namespace hand_evaluator

#endif  // SRC_HAND_EVALUATOR_INDEXER_H_
