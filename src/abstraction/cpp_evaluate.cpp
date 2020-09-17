#include "SevenEval.h"
#include <boost/python.hpp>
#include "HandEvaluator.h"

uint16_t RankFn(uint8_t i, uint8_t j, uint8_t k, uint8_t m,
  uint8_t n, uint8_t p, uint8_t q) {
    return SevenEval::GetRank(i,j,k,m,n,p,q);
}

uint16_t RankFnOMP(uint8_t i, uint8_t j, uint8_t k, uint8_t m,
  uint8_t n, uint8_t p, uint8_t q) {
    omp::HandEvaluator eval;
    omp::Hand h = omp::Hand::empty();
    h += omp::Hand(i) + omp::Hand(j) + omp::Hand(k) + omp::Hand(m) 
      + omp::Hand(n) + omp::Hand(p) + omp::Hand(q);
    return eval.evaluate(h);
}

BOOST_PYTHON_MODULE(hand_evaluator)
{
    using namespace boost::python;
    def("GetRank", RankFn, 
        args("c1", "c2", "c3", "c4", "c5", "c6", "c7"));
    def("GetRankOMP", RankFnOMP, 
        args("c1", "c2", "c3", "c4", "c5", "c6", "c7"));
}
