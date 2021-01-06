// Copyright 2021 Marzuk Rashid

#include "hand_evaluator/Indexer.h"

#include <stdint.h>

#include <iostream>

#include <boost/python.hpp>
extern "C" {
  #include "hand-isomorphism/src/hand_index.h"
}

namespace hand_evaluator {

Indexer::Indexer(int rounds, const boost::python::list& cpr) {
  uint8_t cpr_cpp[rounds];  // NOLINT(runtime/arrays)
  for (int i = 0; i < rounds; ++i) {
    cpr_cpp[i] = boost::python::extract<uint8_t>(cpr[i]);
  }
  isocalc = new hand_indexer_t();
  hand_indexer_init(rounds, cpr_cpp, isocalc);
}

hand_index_t Indexer::Index(const boost::python::list& cards, bool is_omp) {
  uint8_t cards_cpp[len(cards)];  // NOLINT(runtime/arrays)
  for (int i = 0; i < len(cards); ++i) {
    uint8_t c = boost::python::extract<uint8_t>(cards[i]);
    if (is_omp) {
      cards_cpp[i] = ConvertOMPtoISO(c);
    } else {
      cards_cpp[i] = ConvertSKtoISO(c);
    }
  }
  return hand_index_last(isocalc, cards_cpp);
}

}  // namespace hand_evaluator
