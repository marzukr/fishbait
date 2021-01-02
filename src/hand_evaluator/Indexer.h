#ifndef FISHBAIT_HANDEVALUATOR_INDEXER
#define FISHBAIT_HANDEVALUATOR_INDEXER

#include <stdint.h>

#include <boost/python.hpp>
extern "C" {
  #include "hand-isomorphism/src/hand_index.h"
}

namespace hand_evaluator {

class Indexer {
 public:
  Indexer(int rounds, boost::python::list& cpr);
  ~Indexer() { delete isocalc; }

  hand_index_t Index(boost::python::list& cards, bool is_omp);

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

}

#endif