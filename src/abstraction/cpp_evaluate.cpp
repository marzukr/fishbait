#include "SevenEval.h"
#include <boost/python.hpp>
#include "HandEvaluator.h"
extern "C" {
    #include "hand_index.h"
    #include "deck.h"
}

using namespace boost::python;

class Indexer
{
    hand_indexer_t* isocalc;
    uint8_t iso_card_hr(uint8_t hr_card) {
        uint8_t res = 51 - hr_card;
        uint8_t rem = hr_card % 4;
        return res + (rem - 3 + rem);
    }
    uint8_t iso_card_omp(uint8_t omp_card) {
        uint8_t rem = omp_card % 4;
        if (rem == 2) return omp_card + 1;
        else if (rem == 3) return omp_card - 1;
        return omp_card;
    }

    public:
      Indexer(int rounds, list& cpr) {
          uint8_t cpr_cpp[rounds];
          for (int i = 0; i < rounds; i++) {
              cpr_cpp[i] = extract<uint8_t>(cpr[i]);
          }
          isocalc = new hand_indexer_t();
          hand_indexer_init(rounds, cpr_cpp, isocalc);
      }

      hand_index_t index(list& cards, bool is_omp) {
          uint8_t cards_cpp[len(cards)];
          for (int i = 0; i < len(cards); i++) {
              uint8_t c = extract<uint8_t>(cards[i]);
              if (is_omp) cards_cpp[i] = iso_card_omp(c);
              else cards_cpp[i] = iso_card_hr(c);
          }
          return hand_index_last(isocalc, cards_cpp);
      }
};

uint16_t RankFn(uint8_t i, uint8_t j, uint8_t k, uint8_t m,
    uint8_t n, uint8_t p, uint8_t q) {
    return SevenEval::GetRank(i,j,k,m,n,p,q);
}

class OMP
{
    omp::HandEvaluator eval;
    public:
      uint16_t rank_hand(uint8_t i, uint8_t j, uint8_t k, uint8_t m,
          uint8_t n, uint8_t p, uint8_t q) {
          omp::Hand h = omp::Hand::empty();
          h += omp::Hand(i) + omp::Hand(j) + omp::Hand(k) + omp::Hand(m) 
            + omp::Hand(n) + omp::Hand(p) + omp::Hand(q);
          return eval.evaluate(h);
      }
};


BOOST_PYTHON_MODULE(hand_evaluator)
{
    using namespace boost::python;
    def("GetRank", RankFn, 
        args("c1", "c2", "c3", "c4", "c5", "c6", "c7"));
    class_<Indexer>("Indexer", init<int, list&>())
        .def("index", &Indexer::index);
    class_<OMP>("OMP")
        .def("GetRank", &OMP::rank_hand);
}
