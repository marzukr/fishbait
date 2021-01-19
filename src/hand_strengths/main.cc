// Copyright 2021 Marzuk Rashid

#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <algorithm>

#include "SKPokerEval/src/SevenEval.h"
#include "SKPokerEval/src/Deckcards.h"
#include "hand_strengths/Indexer.h"
#include "hand_strengths/OCHS.h"
#include "hand_strengths/CardCombinations.h"
#include "utils/CombinationMatrix.h"
#include "utils/VectorView.h"

void print(const hand_strengths::CardCombinations& c) {
  for (int i = 0; i < c.r(); ++i) {
    std::cout << c(i) << ", ";
  }
  std::cout << std::endl;
}

std::string CardStr(uint8_t i) {
    return pretty_card[i];
}

double Showdown(const std::vector<uint8_t>& hero,
                const std::vector<uint8_t>& villan,
                const std::vector<uint8_t>& rollout) {
  uint16_t hero_rank = SevenEval::GetRank(hero[0], hero[1], rollout[0],
                                          rollout[1], rollout[2], rollout[3],
                                          rollout[4]);
  uint16_t villan_rank = SevenEval::GetRank(villan[0], villan[1], rollout[0],
                                            rollout[1], rollout[2], rollout[3],
                                            rollout[4]);
  if (hero_rank > villan_rank) {
    return 1;
  } else if (hero_rank < villan_rank) {
    return 0;
  } else {
    return 0.5;
  }
}

std::map<uint32_t, double>
GenerateHandStrengthLookup(std::vector<uint8_t> hand) {
  using hand_strengths::CardCombinations;
  using utils::VectorView;

  const uint32_t kBins = 50;

  std::map<uint32_t, double> hand_strength_lookup{};
  // std::vector<double> bins(kBins, 0);
  hand_strengths::Indexer isocalc(2, {2, 5});
  // double total_hand_strength = 0;
  // uint32_t n_rollouts = 0;
  std::array<uint8_t, 7> all_cards{hand[0], hand[1], 0, 0, 0, 0, 0};
  CardCombinations op_hands(2);

  for (CardCombinations rollouts(5, VectorView(hand));
       !rollouts.is_done(); ++rollouts) {
    for (uint8_t i = 0; i < 5; ++i) {
      all_cards[i + 2] = rollouts(i);
    }

    uint32_t idx = isocalc(all_cards);
    // double rollout_strength;

    if (hand_strength_lookup.find(idx) == hand_strength_lookup.end()) {
      double wins = 0;
      double total = 0;
      for (op_hands.Reset(VectorView(all_cards));
           !op_hands.is_done(); ++op_hands) {
        wins += Showdown(hand, op_hands.state(), rollouts.state());
        total += 1;
      }

      hand_strength_lookup[idx] = wins / total;
    }

    // total_hand_strength += rollout_strength;
    // uint32_t correct_bin = rollout_strength * 100 / (100 / kBins);
    // correct_bin = std::min(kBins - 1, correct_bin);
    // bins[correct_bin] += 1;

    // ++n_rollouts;
  }

  // double expected_hand_strength = total_hand_strength / n_rollouts;

  // std::transform(bins.begin(), bins.end(), bins.begin(),
  //     [n_rollouts](const double& a) -> double { return a / n_rollouts; });

  return hand_strength_lookup;
}

// double PreflopHandStrength(std::array<uint8_t, 2> hand,
//     std::map<uint32_t, WeightedHandStrength>& preflop_hand_strength_lookup) {

// }

int main(int argc, char *argv[]) {
  assert(argc == 2);
  int16_t hand_num = std::stoi(argv[1]);
  assert(hand_num < hand_strengths::kUniqueHands && hand_num >= 0);

  std::set<uint32_t> hands_idxs{};
  std::vector<uint8_t> hand(2);

  hand_strengths::Indexer handcalc(1, {2});

  for (uint8_t i = 0; i < hand_strengths::kDeckSize; ++i) {
    uint8_t i_iso = hand_strengths::Indexer::ConvertSKtoISO(i);

    for (uint8_t j = i + 1; j < hand_strengths::kDeckSize; ++j) {
      uint8_t j_iso = hand_strengths::Indexer::ConvertSKtoISO(j);

      uint32_t hand_idx = handcalc({i_iso, j_iso});
      hands_idxs.insert(hand_idx);

      if (hands_idxs.size() == hand_num + 1) {
        hand[0] = i; hand[1] = j;
        break;
      }
    }
    if (hands_idxs.size() == hand_num + 1) break;
  }
  std::cout << CardStr(+hand[0]) << ", " << CardStr(+hand[1]) << std::endl;
  // 4s4h is 160
  // 6s6h is 144
  // TsJs is 70
  // QsKs is 26

  // utils::CombinationMatrix<int16_t> op_clusters(52, -1);
  // for (uint8_t i = 0; i < hand_strengths::kDeckSize; ++i) {
  //   uint8_t i_iso = hand_strengths::Indexer::ConvertSKtoISO(i);

  //   for (uint8_t j = i + 1; j < hand_strengths::kDeckSize; ++j) {
  //     uint8_t j_iso = hand_strengths::Indexer::ConvertSKtoISO(j);

  //     uint32_t hand_idx = handcalc({i_iso, j_iso});

  //     uint8_t cluster = hand_strengths::kOpClusters[hand_idx] - 1;
  //     op_clusters(i, j) = cluster;
  //   }
  // }

  std::map<uint32_t, double> hand_strength_lut =
      GenerateHandStrengthLookup(hand);

  return 0;
}
