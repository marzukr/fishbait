// Copyright 2021 Marzuk Rashid

#include <stdint.h>

#include <string>

#include <boost/python.hpp>
#include "SKPokerEval/src/SevenEval.h"
#include "SKPokerEval/src/Deckcards.h"

// Must include OMPRanker before Indexer to avoid name conflict
#include "hand_evaluator/OMPRanker.h"
#include "hand_evaluator/Indexer.h"

namespace hand_evaluator {

uint16_t RankFn(uint8_t i, uint8_t j, uint8_t k, uint8_t m,
    uint8_t n, uint8_t p, uint8_t q) {
    return SevenEval::GetRank(i, j, k, m, n, p, q);
}

std::string CardStr(uint8_t i) {
    return pretty_card[i];
}

}  // namespace hand_evaluator

BOOST_PYTHON_MODULE(hand_evaluator) {
    using boost::python::def;
    using boost::python::args;
    using boost::python::class_;
    using boost::python::init;

    def("GetRank", hand_evaluator::RankFn,
        args("c1", "c2", "c3", "c4", "c5", "c6", "c7"));
    def("CardString", hand_evaluator::CardStr, args("c"));
    class_<hand_evaluator::Indexer>("Indexer", init<int, list&>())
        .def("index", &hand_evaluator::Indexer::Index);
    class_<hand_evaluator::OMPRanker>("OMP")
        .def("GetRank", &hand_evaluator::OMPRanker::RankHand);
}
