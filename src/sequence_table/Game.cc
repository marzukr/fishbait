#include "sequence_table/Game.h"
#include "SKPokerEval/src/SevenEval.h"
#include "SKPokerEval/src/Deckcards.h"
#include "SKPokerEval/src/Constants.h"

namespace blueprint_strategy {

class Game {
  std::vector<int32_t> Game::GetUtility() {
    char num_players = state_.num_players_;
    char players_left = state_.num_left_;
    std::vector<int32_t> awards(num_players);
    std::vector<int> ranks(num_players);

    // if only one player left don't evaluate hands
    if (players_left <= 1) {
      for (int i = 0; i < num_players; ++i) {
        if (state_.in_game_[i] == true) {
          awards[i] = state_.pot_;
        } else {
          awards[i] = 0;
        }
      }
      return awards;
    }

    char card_start = 2 * num_players;
    for (int i = 0; i < num_players; ++i) {
        ranks[i] = SevenEval::GetRank(
            agents_[i]->get_c1(), agents_[i]->get_c2(),
            deck_[card_start], deck_[card_start+1], deck_[card_start+2],
            deck_[card_start+3],
            deck_[card_start+4]);
        awards[i] = 0;
    }

    while (players_left > 0) {
        double min_bet = state_.pot_;
        for (int i = 1; i < num_players; ++i) {
            double bet_i = state_.total_bets_[i];
            if (bet_i < min_bet && bet_i > 0) min_bet = bet_i;
        }
        double side_pot = 0;
        int best_hand = -1;
        int best_players = 0;
        for (int i = 0; i < num_players; ++i) {
            if (state_.total_bets_[i] >= min_bet) {
                side_pot += min_bet;
                state_.total_bets_[i] = state_.total_bets_[i] - min_bet;
            }
            if (state_.in_game_[i]) {
                if (ranks[i] > best_hand) {
                    best_hand = ranks[i];
                    best_players = 1;
                } else if (ranks[i] == best_hand) {
                    best_players += 1;
                }
            }
        }
        for (int i = 0; i < num_players; ++i) {
            if (state_.in_game_[i] && ranks[i] == best_hand) {
                awards[i] += side_pot / best_players;
            }
            if (state_.total_bets_[i] == 0) {
                if (state_.in_game_[i]) players_left += -1;
                state_.in_game_[i] = false;
            }
        }
    }
    return awards;
  }
}
}