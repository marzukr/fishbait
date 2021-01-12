#include <vector>
#include <random>
#include <stdint.h>
#include "sequence_table/Regret.h"

const uint32_t kT = 1000000;
const int32_t kC = -300000000;
const uint32_t kStrategyInterval = 10000;
const uint32_t kPruneThreshold = 200;
const uint32_t kLCFRThreshold = 400;
const uint32_t kDiscountInterval = 10;

namespace blueprint_strategy{

void Regret::MCCFRP() {
  for(int r = 0; r < regret_table_rows_; ++r) {
    for(int i = 0; i < num_infosets_; ++i) {
      regret_table_[r][i] = 0;
    }
  }
  for (int t = 1; t <= kT; t++) {
    for (int p = 0; p < num_players_; p++) {
      Game g = Game(num_players_, num_rounds_, small_blind_,
                    big_blind_, starting_amounts_, small_blind_pos_);
      if (t % kStrategyInterval == 0) {
        UpdateStrategy(0, g, p);
      }
      if (t > kPruneThreshold) {
        std::default_random_engine generator (1);
        std::discrete_distribution<double> sample(0.0,1.0);
        double q = sample(generator);
        if (q < 0.05) {
          TraverseMCCFR(0, g, p, false);
        }
        else {
          TraverseMCCFR(0, g, p, true);
        }
      }
      else {
        TraverseMCCFR(0, g, p, false);
      }
    }
    if (t < kLCFRThreshold && t % kDiscountInterval == 0) {
      double d = (t/kDiscountInterval)/((t/kDiscountInterval)+1);
      for (int r = 0; r < regret_table_rows_; ++r) {
        for (int i = 0; i < num_infosets_; i++) {
          regret_table_[r][i] = round(regret_table_[r][i]*d);
          if (r < action_counter_rows_) {
            action_counter_[r][i] = round(action_counter_[r][i]*d);
          }
        }
      }
    }
  }
}  
std::vector<float> Regret::CalculateStrategy(uint32_t action_index, uint16_t infoset_index, poker_engine::GameState& state) {
  int64_t sum = 0;
  std::vector<uint32_t> action_indicies = action_table_.GetAllActionIndicies(action_index, state);
  uint32_t regret_pos;
  std::vector<float> sigma;
  for (uint32_t a : action_indicies) {
    regret_pos = std::max(0,regret_table_[a][infoset_index]);
    sum += regret_pos;
  }
  for (uint32_t a : action_indicies) {
    regret_pos = std::max(0,regret_table_[a][infoset_index]);
    if (sum > 0) {
      sigma.push_back((1.0*regret_pos)/sum);
    }
    else {
      sigma.push_back(1.0/action_indicies.size());
    }
  }
  return sigma;  
}

void Regret::UpdateStrategy(uint32_t action_index, Game& game, char player) {
  if(game.state_.is_done_ == true || game.state_.in_game_[player] == false) {
    return;
  }
  else if(game.IsChanceNode() == true) {
    game.Deal();
    UpdateStrategy(action_index, game, player);
    return;
  }
  std::vector<int32_t> actions = action_table_.GetAllActions(action_index, game.state_);
  std::vector<uint32_t> action_indicies = action_table_.GetAllActionIndicies(action_index, game.state_);
  char acting_player = game.state_.acting_player_;
  int32_t max_bet = game.state_.max_bet_;
  int32_t pot_good = game.state_.pot_good_;
  int32_t min_raise = game.state_.min_raise_;
  char round = game.state_.current_round_;
  char betting_round = game.state_.betting_round_;
  int32_t pot = game.state_.pot_;
  bool is_done = game.state_.is_done_;
  char num_all_in = game.state_.num_all_in_;
  char num_left = game.state_.num_left_;
  int32_t number_of_raises = game.state_.number_of_raises_;
  if (game.state_.acting_player_ == player) {
    uint16_t player_infoset = game.GetInfosetIndex(player);
    std::vector<float> sigma = CalculateStrategy(action_index, player_infoset, game.state_);
    std::default_random_engine generator (1);
    std::discrete_distribution<int> sample(sigma.begin(), sigma.end());
    int a = sample(generator);
    int32_t action = actions[a];
    action_counter_[action_indicies[a]][player_infoset] += 1;
    game.state_.TakeAction(action);
    UpdateStrategy(a, game, player);
    return;
  }
  int i = 0;
  for(uint32_t a : action_indicies) {
    int32_t action = actions[i];
    game.state_.TakeAction(action);
    UpdateStrategy(a, game, player);
    game.state_.UndoAction(acting_player, action, max_bet, min_raise, pot_good, round,
                    betting_round, num_all_in, pot, num_left, is_done,
                    number_of_raises);
    ++i;
  }
}



int32_t Regret::TraverseMCCFR(uint32_t action_index, Game& game, char player, bool prune) {
  if (game.state_.is_done_) {
    std::vector<int32_t> awards = game.GetAwards();
    return awards[player];
  }
  //WHAT TO DO IF ALREADY ALL IN?
  if (game.state_.in_game_[player] == false) {
    game.state_.TakeAction(-2);
    return TraverseMCCFR(action_index, game, player, prune);
  }
  if (game.IsChanceNode() == true) {
    game.Deal();
    return TraverseMCCFR(action_index, game, player, prune);
  }
  char acting_player = game.state_.acting_player_;
  int32_t max_bet = game.state_.max_bet_;
  int32_t pot_good = game.state_.pot_good_;
  int32_t min_raise = game.state_.min_raise_;
  char round = game.state_.current_round_;
  char betting_round = game.state_.betting_round_;
  int32_t pot = game.state_.pot_;
  bool is_done = game.state_.is_done_;
  char num_all_in = game.state_.num_all_in_;
  char num_left = game.state_.num_left_;
  int32_t number_of_raises = game.state_.number_of_raises_;
  std::vector<int32_t> actions = action_table_.GetAllActions(action_index, game.state_);
  std::vector<uint32_t> action_indicies = action_table_.GetAllActionIndicies(action_index, game.state_);
  if (game.state_.acting_player_ == player) {
    uint16_t player_infoset = game.GetInfosetIndex(player);
    std::vector<float> sigma = CalculateStrategy(action_index, player_infoset, game.state_);
    std::vector<double> ev(sigma.size());
    double v = 0;
    int i = 0;
    int32_t action;
    float this_sigma;
    std::vector<bool> explored;
    if (prune) {
      explored.reserve(action_indicies.size());
    }
    for (uint32_t a : action_indicies) {
      action = actions[i];
      this_sigma = sigma[i];
      if( !prune || regret_table_[a][player_infoset] > kC) {
        game.state_.TakeAction(action);
        ev[i] = TraverseMCCFR(a, game, player, prune);
        if (prune){
          explored[i] = true;
        }
        v = v + this_sigma*ev[i];
        game.state_.UndoAction(player, action, max_bet, min_raise, pot_good, round,
                      betting_round, num_all_in, pot, num_left, is_done,
                      number_of_raises);
      } else if(prune) {
        explored[i] = false;
      }
      ++i;
    }
    i = 0;
    for (uint32_t a : action_indicies) {
      if (!prune || explored[i]) {
        regret_table_[a][player_infoset] = regret_table_[a][player_infoset] + ev[i] - v;
      }
      ++i;
    }
    return v;
  } else {
    char p = game.state_.acting_player_;
    uint16_t new_infoset = game.GetInfosetIndex(p);
    std::vector<float> sigma = CalculateStrategy(action_index, new_infoset, game.state_);
    std::vector<int32_t> actions = action_table_.GetAllActions(action_index, game.state_);
    std::default_random_engine generator (1);
    std::discrete_distribution<int> sample(sigma.begin(), sigma.end());
    int a = sample(generator);
    int32_t action = actions[a];
    game.state_.TakeAction(action);
    return TraverseMCCFR(action_indicies[a], game, player, prune);
  }
}
} // namespace blueprint_strategy