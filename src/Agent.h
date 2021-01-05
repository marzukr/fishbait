#ifndef SRC_POKER_ENGINE_AGENT_H_
// Copyright 2021 Emily Dale
#define SRC_POKER_ENGINE_AGENT_H_

#include <string>

namespace game_engine {

class Agent {
 private:
  int c1_;
  int c2_;
  std::string name_;
  friend std::ostream& operator<<(std::ostream&, const Agent&);

 public:
  explicit Agent(std::string agent_name) {name_ = agent_name; }
  int get_c1() { return c1_; }
  int get_c2() { return c2_; }
  double action(double max_bet, double min_raise, double total_bet,
                double chips);
  void deal_cards(int card1, int card2);
};  // class Agent

}  // namespace game_engine

#endif  // SRC_POKER_ENGINE_AGENT_H_
