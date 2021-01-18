// Copyright 2021 Marzuk Rashid

#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <set>

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

// void PreflopDistribution(std::array<uint8_t, 2> hand) {
//   for (rollout)
// }

int main(int argc, char *argv[]) {
  assert(argc == 2);
  int16_t hand_num = std::stoi(argv[1]);
  assert(hand_num < hand_strengths::kUniqueHands && hand_num >= 0);

  std::set<uint32_t> hands_idxs{};
  uint32_t hand[2];

  uint8_t cards_per_round[] = {2};
  hand_strengths::Indexer handcalc(1, cards_per_round);

  for (uint8_t i = 0; i < hand_strengths::kDeckSize; ++i) {
    uint8_t i_iso = hand_strengths::Indexer::ConvertSKtoISO(i);

    for (uint8_t j = i + 1; j < hand_strengths::kDeckSize; ++j) {
      uint8_t j_iso = hand_strengths::Indexer::ConvertSKtoISO(j);

      uint8_t current_hand[] = {i_iso, j_iso};
      uint32_t hand_idx = handcalc(current_hand);
      hands_idxs.insert(hand_idx);

      if (hands_idxs.size() == hand_num + 1) {
        hand[0] = i; hand[1] = j;
        break;
      }
    }
    if (hands_idxs.size() == hand_num + 1) break;
  }
  std::cout << hand[0] << ", " << hand[1] << std::endl;

  utils::CombinationMatrix<int16_t> op_clusters(52, -1);
  for (uint8_t i = 0; i < hand_strengths::kDeckSize; ++i) {
    uint8_t i_iso = hand_strengths::Indexer::ConvertSKtoISO(i);

    for (uint8_t j = i + 1; j < hand_strengths::kDeckSize; ++j) {
      uint8_t j_iso = hand_strengths::Indexer::ConvertSKtoISO(j);

      uint8_t current_hand[] = {i_iso, j_iso};
      uint32_t hand_idx = handcalc(current_hand);

      uint8_t cluster = hand_strengths::kOpClusters[hand_idx] - 1;
      op_clusters(i, j) = cluster;
    }
  }

  using hand_strengths::CardCombinations;
  using utils::VectorView;
  for (CardCombinations rollouts(2, VectorView<uint32_t>({3, 1}));
       !rollouts.is_done(); ++rollouts) {
    print(rollouts);
  }

  // isocalc = hand_evaluator.Indexer(2, [2,5])
  // unique_rollouts = {}
  // for rollout in rollouts(hand):
  //     iso_idx = isocalc.index([hand[0], hand[1],
  //         rollout[0], rollout[1], rollout[2], rollout[3], rollout[4]],
  //         False)
  //     if iso_idx not in unique_rollouts:
  //         unique_rollouts[iso_idx] = rollout

  // ret = {}
  // with concurrent.futures.ProcessPoolExecutor(max_workers=12) as executor:
  //     future_to_iso_idx = {
  //         executor.submit(generate_hand_strength_roll_ochs,
  //             hand, rollout, op_clusters): iso_idx
  //         for iso_idx, rollout in unique_rollouts.items()
  //     }
  //     for future in concurrent.futures.as_completed(future_to_iso_idx):
  //         vect = future.result()
  //         iso_idx = future_to_iso_idx[future]
  //         ret[iso_idx] = vect
  // save_binary("luts/{}_{}.ochs".format(hand[0], hand[1]), ret)

  return 0;
}
