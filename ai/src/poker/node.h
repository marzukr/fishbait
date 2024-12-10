#ifndef AI_SRC_POKER_NODE_H_
#define AI_SRC_POKER_NODE_H_

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <limits>
#include <numeric>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>

#include "mccfr/definitions.h"
#include "poker/card_utils.h"
#include "poker/definitions.h"
#include "SKPokerEval/src/SevenEval.h"
#include "utils/array.h"
#include "utils/fraction.h"
#include "utils/random.h"

namespace fishbait {

/* QuotaT is used to represent exact award quotas before being apportioned into
   discrete chips (double or utils::Fraction). This choice could impact the
   accuracy and/or speed of the awarded chip amounts. */
template <PlayerId kPlayers = 6, typename QuotaT = double>
class Node {
 private:
  // Attributes
  // --------------------------------------------------------------------------

  Chips big_blind_;         // how many chips the big blind is
  Chips small_blind_;       // how many chips the small blind is
  Chips ante_;              /* how many chips each player must contribute to the
                               pot on each hand in expectation */
  bool big_blind_ante_;     /* does the big blind pays the ante for everyone in
                               the game? */
  bool blind_before_ante_;  /* should players should cover the blind instead of
                               the ante if they don't have enough chips to pay
                               both? */
  QuotaT rake_;             // proportion of each hand taken as rake
  Chips rake_cap_;          /* maximum number of chips that can be raked;
                               rake_cap_ of 0 means no cap */
  bool no_flop_no_drop_;    // is rake taken on hands without a flop?

  // Progress information
  // --------------------------------------------------------------------------

  PlayerId button_;                    // index of player on the button
  bool in_progress_;                   // is a hand is in progress?
  Round round_;                        // current betting round
  PlayCount cycled_;                   /* number of players that have been
                                          cycled through on this betting round.
                                          Also used to internally keep track of
                                          the number of straddled players before
                                          play starts. */
  PlayerId acting_player_;             // index of player whose turn it is
  PlayerId pot_good_;                  /* number of players who still need to
                                          act before this round is over */
  PlayerId no_raise_;                  /* number of players who still need to
                                          act, but can only call or fold because
                                          another player went all in less than
                                          the min-raise */
  std::array<bool, kPlayers> folded_;  // has the given player folded?
  PlayerId players_left_;              // number of players who haven’t folded
  PlayerId players_all_in_;            // number of players who are all in

  // Chip information
  // --------------------------------------------------------------------------

  Chips pot_;                          // number of chips in the pot
  std::array<Chips, kPlayers> bets_;   // number of chips each player has bet
  std::array<Chips, kPlayers> stack_;  // number of chips each player has behind
  Chips min_raise_;                    // minimum bet amount
  Chips max_bet_;                      /* maximum amount that has been bet so
                                          far */

  // Card Information
  // --------------------------------------------------------------------------

  template <Round kRound>
  static constexpr CardN CumulativeCards() {
    if constexpr (kRound == Round::kPreFlop) {
      return kPlayers * kHandCards;
    } else {
      return CumulativeCards<Round{+kRound - 1}>() + kCardsPerRound[+kRound];
    }
  }
  static constexpr std::array<CardN, kNRounds> kCumulativeCards = {
    CumulativeCards<Round::kPreFlop>(), CumulativeCards<Round::kFlop>(),
    CumulativeCards<Round::kTurn>(), CumulativeCards<Round::kRiver>()
  };
  Deck<ISO_Card> deck_;   /* The deck of cards used by this node. Player i's
                             cards are in deck_[i * kHandCards] to
                             deck_[i * kHandCards + kHandCards - 1]. Board cards
                             come after the player hands. */
  enum class DeckState : uint8_t { kManual, kAuto, kAutoDealt };
  DeckState deck_state_;  /* kManual indicates that proper shuffling can no
                             longer be guaranteed by automatic dealing. kAuto
                             indicates that automatic dealing may be used.
                             kAutoDealt can only occur during a chance node and
                             indicates that cards were dealt at the chance
                             node. */
  inline static thread_local Random rng_;

 public:
  /*
    @brief Construct a node.

    @param button The index of the player on the button.
    @param big_blind How many chips the big blind is.
    @param small_blind How many chips the small blind is.
    @param ante How many chips each player pays as an ante each hand.
    @param big_blind_ante Does the big blind pays the ante for all players?
    @param blind_before_ante Should players should cover the blind instead of
        the ante if they don't have enough chips to pay both?
    @param default_stack Number of chips each player starts with.
    @param rake Proportion of each hand taken as rake.
    @param rake_cap Maximum number of chips that can be raked. 0 means no cap.
    @param no_flop_no_drop Is rake taken on hands without a flop?
  */
  Node(std::array<Chips, kPlayers> stacks = StackArray<kPlayers>(10000),
       PlayerId button = 0, Chips big_blind = 100, Chips small_blind = 50,
       Chips ante = 0, bool big_blind_ante = false,
       bool blind_before_ante = true, QuotaT rake = QuotaT{0},
       Chips rake_cap = 0, bool no_flop_no_drop = false)

         // Attributes
       : big_blind_{big_blind}, small_blind_{small_blind}, ante_{ante},
         big_blind_ante_{big_blind_ante}, blind_before_ante_{blind_before_ante},
         rake_{rake}, rake_cap_{rake_cap}, no_flop_no_drop_{no_flop_no_drop},

         // Progress Information
         button_{button}, in_progress_{false}, round_{Round::kPreFlop},
         cycled_{0}, acting_player_{0}, pot_good_{kPlayers}, no_raise_{0},
         folded_{}, players_left_{kPlayers}, players_all_in_{0},

         // Chip Information
         pot_{0}, bets_{}, stack_{stacks}, min_raise_{big_blind},
         max_bet_{big_blind},

         // Card Information
         deck_{UnshuffledDeck<ISO_Card>()}, deck_state_{DeckState::kAuto} {
    NewHand(button_);
  }  // Node()
  Node(Chips default_stack, PlayerId button = 0, Chips big_blind = 100,
       Chips small_blind = 50, Chips ante = 0, bool big_blind_ante = false,
       bool blind_before_ante = true, QuotaT rake = QuotaT{0},
       Chips rake_cap = 0, bool no_flop_no_drop = false)
       : Node(StackArray<kPlayers>(default_stack), button, big_blind,
              small_blind, ante, big_blind_ante, blind_before_ante, rake,
              rake_cap, no_flop_no_drop) {}
  Node(const Node& other, std::array<Chips, kPlayers> stacks)
      : Node(stacks, other.button_, other.big_blind_, other.small_blind_,
             other.ante_, other.big_blind_ante_, other.blind_before_ante_,
             other.rake_, other.rake_cap_, other.no_flop_no_drop_) {}
  Node(const Node& other) = default;
  Node& operator=(const Node& other) = default;

  /* @brief Node serialize function */
  template<class Archive>
  void serialize(Archive& archive) {
    archive(CEREAL_NVP(big_blind_), CEREAL_NVP(small_blind_), CEREAL_NVP(ante_),
            CEREAL_NVP(big_blind_ante_), CEREAL_NVP(blind_before_ante_),
            CEREAL_NVP(rake_), CEREAL_NVP(rake_cap_),
            CEREAL_NVP(no_flop_no_drop_), CEREAL_NVP(button_),
            CEREAL_NVP(in_progress_), CEREAL_NVP(round_), CEREAL_NVP(cycled_),
            CEREAL_NVP(acting_player_), CEREAL_NVP(pot_good_),
            CEREAL_NVP(no_raise_), CEREAL_NVP(folded_),
            CEREAL_NVP(players_left_), CEREAL_NVP(players_all_in_),
            CEREAL_NVP(pot_), CEREAL_NVP(bets_), CEREAL_NVP(stack_),
            CEREAL_NVP(min_raise_), CEREAL_NVP(max_bet_), CEREAL_NVP(deck_),
            CEREAL_NVP(deck_state_));
  }

  /* @brief Equality comparison operator. */
  bool operator==(const Node& other) const {
    return big_blind_ == other.big_blind_ &&
        small_blind_ == other.small_blind_ && ante_ == other.ante_ &&
        big_blind_ante_ == other.big_blind_ante_ &&
        blind_before_ante_ == other.blind_before_ante_ &&
        rake_ == other.rake_ && rake_cap_ == other.rake_cap_ &&
        no_flop_no_drop_ == other.no_flop_no_drop_ &&
        button_ == other.button_ && in_progress_ == other.in_progress_ &&
        round_ == other.round_ && cycled_ == other.cycled_ &&
        acting_player_ == other.acting_player_ &&
        pot_good_ == other.pot_good_ && no_raise_ == other.no_raise_ &&
        folded_ == other.folded_ && players_left_ == other.players_left_ &&
        players_all_in_ == other.players_all_in_ && pot_ == other.pot_ &&
        bets_ == other.bets_ && stack_ == other.stack_ &&
        min_raise_ == other.min_raise_ && max_bet_ == other.max_bet_ &&
        deck_ == other.deck_ && deck_state_ == other.deck_state_;
  }

  /* @brief Inquality comparison operator. */
  bool operator!=(const Node& other) const {
    return !(*this == other);
  }

  /*
    @brief Resets the node to how it was before this hand started.

    Call NewHand() after Erase() to redo this hand.
  */
  void Erase() {
    if (pot_ == 0) {
      std::string func{__func__};
      throw std::logic_error(func + " called when pot is empty. If the pot is "
                                    "0, this means AwardPot() has been called. "
                                    "The node cannot be erased if AwardPot() "
                                    "has already been called for this hand.");
    }
    pot_ = 0;
    for (PlayerId p = 0; p < kPlayers; ++p) {
      stack_[p] += bets_[p];
      bets_[p] = 0;
    }
  }

  /*
    @brief Reset the state variables for the start of a new hand.
  */
  void NewHand(PlayerId button = kChancePlayer) {
    if (pot_ != 0) {
      std::string func{__func__};
      throw std::logic_error(func + " called when pot is not empty. If the pot "
                             "isn't 0, this means AwardPot() hasn't been "
                             "called yet. The last pot must be awarded before "
                             "a new hand starts.");
    }

    // Attributes are constants and don't need to be set

    // Progress Information
    if (button == kChancePlayer) {
      button_ = (button_ + 1) % kPlayers;
    } else {
      button_ = button;
    }
    in_progress_ = true;
    round_ = Round::kPreFlop;
    cycled_ = 0;  // no straddled players by default
    acting_player_ = kChancePlayer;
    pot_good_ = kPlayers;
    no_raise_ = 0;
    std::fill(folded_.begin(), folded_.end(), false);
    players_left_ = kPlayers;
    players_all_in_ = 0;

    // Chip Information
    /* pot_ should already be zero from either the constructor or AwardPot() on
       the previous hand */
    /* bets_ should already be all zero from either the constructor or
       AwardPot() on the previous hand */
    /* stack_ should already set to the right amounts from either the
       constructor or AwardPot() on the previous hand */
    // min_raise_ will be set after the straddle is posted
    // max_bet_ will be set after the blinds and antes are posted

    /* Card Information is dealt with by DealCards() and/or set_board() and
       set_hands(). */

    // Post Blinds, Antes, and Straddles
    Chips effective_ante = ante_;
    if (ante_ > 0 && !blind_before_ante_) effective_ante = PostAntes();
    PostBlind(PlayerIndex(1), small_blind_);
    PostBlind(PlayerIndex(2), big_blind_);
    if (ante_ > 0 && blind_before_ante_) effective_ante = PostAntes();
    players_all_in_ = std::count(stack_.begin(), stack_.end(), 0);
    min_raise_ = big_blind_;
    max_bet_ = big_blind_ + effective_ante;
  }  // NewHand()

  /*
    @brief Have the appropriate players straddle.
    
    If a player does not have enough chips to straddle the appropriate amount,
    they do not straddle and no players behind them can straddle either. Can 
    only be called once per hand during the preflop chance node. If in auto deal
    mode, it also must only be called before any cards have been dealt.

    @param n How many players to try to straddle.
  */
  void PostStraddles(PlayerN n) {
    const std::string func{__func__};
    if (!in_progress_ || acting_player_ != kChancePlayer ||
        round_ != Round::kPreFlop || deck_state_ == DeckState::kAutoDealt ||
        cycled_ != 0) {
      throw std::logic_error(func + " must only be called once per hand during "
                                    "the preflop chance node. If in auto deal "
                                    "mode, it also must only be called before "
                                    "any cards have been dealt.");
    }
    acting_player_ = PlayerIndex(3);
    Chips max_straddle_size = big_blind_;
    PlayerN actual_n_straddle = 0;
    max_bet_ -= big_blind_;  // Set max_bet_ to just the effective ante.
    for (PlayerId i = 0; i < n; ++i) {
      Chips straddle_size = max_straddle_size * 2;
      if (straddle_size > stack_[acting_player_]) {
        break;
      } else {
        if (straddle_size == stack_[acting_player_]) players_all_in_ += 1;
        PostBlind(acting_player_, straddle_size);
        max_straddle_size = straddle_size;
        acting_player_ = NextPlayer();
        ++actual_n_straddle;
      }
    }
    min_raise_ = max_straddle_size;
    max_bet_ += max_straddle_size;

    /* Set cycled_ to the number of straddles so ProceedPlay() knows which
       player to start with */
    cycled_ = actual_n_straddle;

    acting_player_ = kChancePlayer;
  }  // PostStraddles()

  /*
    @brief Optionally deal random cards if we are at a chance node.
  */
  void Deal() {
    const std::string func{__func__};
    if (!in_progress_ || acting_player_ != kChancePlayer) {
      throw std::logic_error(func + " called when the game is not at a chance "
                                    "node.");
    } else if (deck_state_ != DeckState::kAuto) {
      throw std::logic_error(func + " called when the node is not in automatic "
                                    "dealing mode. Call ResetDeck() at the "
                                    "preflop chance node of the next hand to "
                                    "return to auto deal mode.");
    }
    CardN lb;
    if (round_ == Round::kPreFlop) {
      lb = 0;
    } else {
      RoundId prev_round = +round_ - 1;
      lb = kCumulativeCards[prev_round];
    }
    for (CardN i = lb; i < kCumulativeCards[+round_]; ++i) {
      UniformIntDistribution<CardN> rand_card(i, deck_.size() - 1);
      CardN selected_card = rand_card(rng_());
      std::swap(deck_[i], deck_[selected_card]);
    }
    deck_state_ = DeckState::kAutoDealt;
  }

  /*
    @brief Get the index of a player relative to the button.

    @param default_position The default position of the player to get the index
        of. Button is 0, small blind 1, big blind 2, etc. If it's heads up, 0
        and 1 return the same index.
  */
  PlayerId PlayerIndex(PlayerId default_position) const {
    if (kPlayers == 2 && default_position > 0) default_position -= 1;
    return (button_ + default_position) % kPlayers;
  }

  /* 
    @brief How many times the betting has gone around on this betting round.
  */ 
  PlayCount Rotation() const { return cycled_ / kPlayers; }

  /*
    @brief If the current acting_player_ can fold.
    
    A player can always fold unless they can check instead.
  */
  bool CanFold() const {
    Chips prev_bet = bets_[acting_player_];
    Chips needed_to_call = max_bet_ - prev_bet;
    return in_progress_ && needed_to_call > 0;
  }

  /*
    @brief Returns the number of chips the acting_player_ needs to call.

    If the acting_player_ can check, then returns 0.
  */
  Chips NeededToCall() const {
    Chips prev_bet = bets_[acting_player_];
    return max_bet_ - prev_bet;
  }

  /*
    @brief If the current acting_player_ can check/call.
    
    @return True if the current acting_player_ can check/call. Otherwise returns
        false. Also returns false if the player must go all in to call.
  */
  bool CanCheckCall() const {
    return in_progress_ && NeededToCall() < stack_[acting_player_];
  }

  /*
    @brief If the current acting_player_ can bet the given amount.
    
    @param size The amount of additional chips that the player would put into
        the pot.

    @return True if the current acting_player_ can bet the given amount.
        Otherwise returns false. Also returns false if the given amount is all
        of the players remaining chips.
  */
  bool CanBet(Chips size) const {
    Chips prev_bet = bets_[acting_player_];
    Chips total_bet = size + prev_bet;
    Chips raise_size = total_bet - max_bet_;
    return in_progress_ &&
           pot_good_ > 0 &&
           total_bet > max_bet_ &&
           raise_size >= min_raise_ &&
           size < stack_[acting_player_];
  }

  /*
    @brief If the current acting_player_ can perform the given AbstractAction.
  */
  bool IsLegal(const AbstractAction& a) const {
    if (a.play == Action::kFold) {
      return CanFold();
    } else if (a.play == Action::kCheckCall) {
      return CanCheckCall();
    } else if (a.play == Action::kAllIn) {
      return in_progress_;
    } else if (a.play == Action::kBet) {
      Chips bet_size = ProportionToChips(a.size);
      return CanBet(bet_size);
    }
    return false;
  }

  /*
    @brief Converts a pot proportion into chips for the current acting player.
  */
  Chips ProportionToChips(QuotaT pot_proportion) const {
    Chips prev_bet = bets_[acting_player_];
    Chips needed_to_call = max_bet_ - prev_bet;
    Chips new_pot = pot_ + needed_to_call;
    QuotaT exact_bet = pot_proportion * new_pot + needed_to_call;
    double double_bet = static_cast<double>(exact_bet);
    Chips rounded_bet = std::rint(double_bet);
    return rounded_bet;
  }

  /*
    @brief Converts chips into a pot proportion for the current acting player.
  */
  QuotaT ChipsToProportion(Chips bet_size) const {
    Chips prev_bet = bets_[acting_player_];
    Chips needed_to_call = max_bet_ - prev_bet;
    Chips new_pot = pot_ + needed_to_call;
    QuotaT surplus = QuotaT(bet_size) - QuotaT(needed_to_call);
    return surplus / new_pot;
  }

  /*
    @brief Apply the given move to this Node object.
    
    Must only be called when the game is in progress. Otherwise throws.

    @param play The Action to apply to this Node.
    @param size How much more the player is adding to the pot if play is kBet.
        Otherwise it has no significance.

    @return True if the game is still in progress after this move, false if this
        move has ended the game.
  */
  bool Apply(Action play, Chips size = 0) {
    if (!in_progress_) {
      const std::string func{__func__};
      throw std::logic_error(func + " called when a game is not in progress.");
    } else if (acting_player_ == kChancePlayer) {
      const std::string func{__func__};
      throw std::logic_error(func + " called at a chance node.");
    }

    switch (play) {
      case Action::kFold:
        if (CanFold()) {
          Fold();
        } else {
          throw std::invalid_argument("Fold is not a valid move for the "
                                      "current acting player.");
        }
        break;
      case Action::kAllIn:
        AllIn();
        break;
      case Action::kCheckCall:
        if (CanCheckCall()) {
          CheckCall();
        } else {
          throw std::invalid_argument("Check/Call is not a valid move for the "
                                      "current acting player.");
        }
        break;
      case Action::kBet:
        if (CanBet(size)) {
          Bet(size);
        } else {
          std::string err = "Betting " + std::to_string(size) + " is not a "
                            "valid move for the current acting player.";
          throw std::invalid_argument(err);
        }
        break;
    }  // switch play
    CyclePlayers(true);
    return in_progress_;
  }  // Apply()

  /*
    @brief Apply the given AbstractAction to this Node object.
    
    Must only be called when the game is in progress. Otherwise throws.

    @return True if the game is still in progress after this move, false if this
        move has ended the game.
  */
  bool Apply(const AbstractAction& action) {
    if (action.play == Action::kBet) {
      Chips bet_size = ProportionToChips(action.size);
      return Apply(action.play, bet_size);
    }
    return Apply(action.play);
  }

  /* @brief Proceeds play after a chance node. */
  void ProceedPlay() {
    if (!in_progress_ || acting_player_ != kChancePlayer) {
      const std::string func{__func__};
      throw std::logic_error(func + " called when the game is not at a chance "
                                    "node.");
    }

    /* If we dealt cards at this chance node, auto dealing can continue.
       Otherwise we must switch over to manual dealing mode as card shuffling
       can no longer be guaranteed. */
    if (deck_state_ == DeckState::kAutoDealt) {
      deck_state_ = DeckState::kAuto;
    } else {
      deck_state_ = DeckState::kManual;
    }

    if (round_ == Round::kPreFlop) {
      /* PostStraddles() sets cycled_ to the number of players who have
         straddled. Otherwise NewHand() sets it to 0. */
      acting_player_ = PlayerIndex(3 + cycled_);
    } else {
      acting_player_ = PlayerIndex(1);
    }
    cycled_ = 0;
    pot_good_ = kPlayers;
    no_raise_ = 0;

    /* If it's preflop, we don't need to set the min_raise_ because it is 
       already done by NewHand(), and we don't want to overwrite any
       straddles */
    if (round_ > Round::kPreFlop) min_raise_ = big_blind_;

    CyclePlayers(false);
  }

  constexpr static struct SameStackNoRake {} same_stack_no_rake_{};
  /*
    @brief Allocate the pot to the winner(s) at the end of a hand.
    
    After calling, pot will be 0, all bets will be 0, and the winner(s)'s stack
    will have increased by the appropriate amount. Must only be called when the
    game is no longer in progress and AwardPot() hasn't already been called for
    the current hand. This version applies optimizations based on the assumption
    that all players started the hand with the same stack amount, there is no
    rake, and no players have mucked their hands. The pot will not be awarded
    correctly if this is not the case. Note, this function will also not work
    with a big blind ante because the big blind has less effective chips than
    the other players.
  */
  void AwardPot(SameStackNoRake) {
    VerifyAwardablePot(__func__);
    if (players_left_ == 1) return FoldVictory(folded_);
    SevenEval::Rank ranks[kPlayers];
    RankPlayers(folded_, ranks);
    BestPlayersData best_players = BestPlayers(ranks, folded_);
    QuotaT exact_awards[kPlayers]{};
    QuotaT pot_precise = QuotaT{1} * pot_;
    DivideSidePot(pot_precise, ranks, folded_, best_players, exact_awards);
    DistributeChips(&pot_, exact_awards, stack_);
    std::fill(bets_.begin(), bets_.end(), 0);
  }

  constexpr static struct SingleRun {} single_run_{};
  /*
    @brief Allocate the pot to the winner(s) at the end of a hand.
    
    This version can only do one run of the board. After calling, pot will be 0,
    all bets will be 0, and the winner(s)'s stack will have increased by the
    appropriate amount. Must only be called when the game is no longer in
    progress and AwardPot() hasn't already been called for the current hand.
  */
  void AwardPot(SingleRun) {
    VerifyAwardablePot(__func__);
    std::array<bool, kPlayers> processed;
    PlayerN players_to_award = AwardTrivialVictory(processed);
    if (players_to_award == 0) return;

    SevenEval::Rank ranks[kPlayers];
    RankPlayers(processed, ranks);

    // Loop through each side pot and award it to the appropriate player(s)
    QuotaT exact_awards[kPlayers]{};
    while (players_to_award > 0) {
      QuotaT side_pot = QuotaT{1} * AllocateSidePot(bets_, processed);
      BestPlayersData best_players = BestPlayers(ranks, processed);
      DivideSidePot(side_pot, ranks, processed, best_players, exact_awards);
      players_to_award -= MarkProcessedPlayers(bets_, processed);
    }  // while players_to_award > 0
    if (ShouldRake()) Rake(&pot_, exact_awards);
    DistributeChips(&pot_, exact_awards, stack_);
  }  // AwardPot()

  constexpr static struct MultiRun {} multi_run_{};
  /*
    @brief Allocate the pot to the winner(s) at the end of a hand.
    
    This version runs the board multiple times. After calling, pot will be 0,
    all bets will be 0, and the winner(s)'s stack will have increased by the
    appropriate amount. Must only be called when the game is no longer in
    progress and AwardPot() hasn't already been called for the current hand.

    @param boards Array of the public board cards. Follows the standard order.
        i.e. columns [0,2] are the flop, column 3 is the turn, and column 4 is
        the river. ISO indexing. Each row is a different run out.
  */
  template <std::size_t kRuns>
  void AwardPot(MultiRun, const MultiBoardArray<ISO_Card, kRuns>& boards) {
    VerifyAwardablePot(__func__);
    std::array<bool, kPlayers> processed;
    PlayerN players_to_award = AwardTrivialVictory(processed);
    if (players_to_award == 0) return;

    QuotaT exact_awards[kPlayers]{};

    // Variables to use throughout the loops
    SevenEval::Rank ranks_run[kPlayers] = {};
    std::array<bool, kPlayers> processed_run = {};
    std::array<Chips, kPlayers> bets_run = {};
    QuotaT side_pot;
    BestPlayersData best_players_run;
    PlayerId players_to_award_run;

    for (std::size_t run = 0; run < kRuns; ++run) {
      std::copy(processed.begin(), processed.end(), processed_run.begin());
      SetBoard(boards[run]);
      RankPlayers(processed_run, ranks_run);
      std::copy(bets_.begin(), bets_.end(), bets_run.begin());
      players_to_award_run = players_to_award;

      // Loop through each side pot and award it to the appropriate player(s)
      while (players_to_award_run > 0) {
        side_pot = AllocateSidePot(bets_run, processed_run) / QuotaT{kRuns};
        best_players_run = BestPlayers(ranks_run, processed_run);
        DivideSidePot(side_pot, ranks_run, processed_run, best_players_run,
                      exact_awards);
        players_to_award_run -= MarkProcessedPlayers(bets_run, processed_run);
      }  // while players_to_award_run > 0
    }  // for run
    if (ShouldRake()) Rake(&pot_, exact_awards);
    DistributeChips(&pot_, exact_awards, stack_);
    std::fill(bets_.begin(), bets_.end(), 0);
  }  // AwardPot()

  /* @brief Returns a snapshot of the current state of this Node. */
  NodeSnapshot<kPlayers> Snapshot() const {
    NodeSnapshot<kPlayers> ret{kPlayers, big_blind_, small_blind_, ante_,
        big_blind_ante_, blind_before_ante_, static_cast<double>(rake_),
        rake_cap_, no_flop_no_drop_, button_, in_progress_, +round_,
        acting_player_, {}, players_left_, players_all_in_, pot_, {}, {},
        min_raise_, NeededToCall(), {}, {}};
    std::copy(folded_.begin(), folded_.end(), ret.folded);
    std::copy(bets_.begin(), bets_.end(), ret.bets);
    std::copy(stack_.begin(), stack_.end(), ret.stack);
    auto hands_end = std::next(deck_.begin(), kPlayers * kHandCards);
    std::copy(deck_.begin(), hands_end, ret.hands[0]);
    std::copy_n(hands_end, kBoardCards, ret.board);
    return ret;
  }

  // Attribute getter functions
  // --------------------------------------------------------------------------

  static constexpr PlayerN players() { return kPlayers; }
  Chips big_blind() const { return big_blind_; }
  Chips small_blind() const { return small_blind_; }
  Chips ante() const { return ante_; }
  bool big_blind_ante() const { return big_blind_ante_; }
  bool blind_before_ante() const { return blind_before_ante_; }

  // Progress getter functions
  // --------------------------------------------------------------------------

  PlayerId button() const { return button_; }
  bool in_progress() const { return in_progress_; }
  Round round() const { return round_; }
  PlayCount cycled() const { return cycled_; }
  PlayerId acting_player() const { return acting_player_; }
  // no pot_good_ getter because it is an implementation detail
  // no no_raise_ getter because it is an implementation detail
  bool folded(PlayerId player) const { return folded_[player]; }
  PlayerId players_left() const { return players_left_; }
  PlayerId players_all_in() const { return players_all_in_; }

  // Progress constants
  // --------------------------------------------------------------------------

  constexpr static PlayerId kChancePlayer = kPlayers;

  // Chip information getter functions
  // --------------------------------------------------------------------------

  Chips pot() const { return pot_; }
  Chips bets(PlayerId player) const { return bets_[player]; }
  Chips stack(PlayerId player) const { return stack_[player]; }
  // no min_raise_ getter because it is an implementation detail
  // no max_bet_ getter because it is an implementation detail

  // Card information getter functions
  // --------------------------------------------------------------------------

  /*
    @brief Returns an array of all the cards that the given player can see.

    The first two cards are the hole cards. The rest are the cards on the board
    starting with the flop, then the turn, then the river. If a card has not
    been revealed yet (i.e. the node is on the Flop, so the turn and river cards
    would not have been revealed yet), then the value of the card at that
    position is undefined.

    @param player The player whose cards to return.

    @return An array of all the cards that the given player can see.
  */
  PublicHand<ISO_Card> PlayerCards(PlayerId player) const {
    PublicHand<ISO_Card> public_hand;
    std::copy_n(std::next(deck_.begin(), player * kHandCards), kHandCards,
                public_hand.begin());
    std::copy_n(std::next(deck_.begin(), kPlayers * kHandCards),
                kCumulativeCards[+round_] - kCumulativeCards[+Round::kPreFlop],
                std::next(public_hand.begin(), kHandCards));
    return public_hand;
  }

  // Card information setter functions
  // --------------------------------------------------------------------------

  void SetHand(PlayerId player, const Hand<ISO_Card>& hand) {
    std::copy(hand.begin(), hand.end(),
              std::next(deck_.begin(), player * kHandCards));
    deck_state_ = DeckState::kManual;
  }
  void SetHands(const HandArray<ISO_Card, kPlayers>& hands) {
    std::copy_n(&hands[0][0], kPlayers * kHandCards, deck_.begin());
    deck_state_ = DeckState::kManual;
  }
  void SetBoard(const BoardArray<ISO_Card>& board) {
    std::copy(board.begin(), board.end(),
              std::next(deck_.begin(), kPlayers * kHandCards));
    deck_state_ = DeckState::kManual;
  }

  /* 
    @brief Resets the deck to the unshuffled state.

    If done at the preflop chance node, or when we have just concluded a hand,
    we can make auto dealing shuffle guarantees and return to auto dealing mode.
  */
  void ResetDeck() {
    deck_ = UnshuffledDeck<ISO_Card>();

    if ((in_progress_ && acting_player_ == kChancePlayer &&
        round_ == Round::kPreFlop) || (!in_progress_ && pot_ == 0)) {
      deck_state_ = DeckState::kAuto;
    } else {
      deck_state_ = DeckState::kManual;
    }
  }

  /* 
    @brief Sets the seed of the random number generator used to sample cards.
  */
  static void SetSeed(Random::Seed seed) {
    rng_.seed(seed);
  }

 private:
  /*
    @brief Make the given player post the given blind.
    
    If the player doesn't have enough chips to post the blind, the player goes
    all in. Does not change max_bet_ or min_raise_.
        
    @return The actual size of the blind posted.
  */
  Chips PostBlind(PlayerId player, Chips size) {
    Chips blind = std::min(size, stack_[player]);
    bets_[player] += blind;
    stack_[player] -= blind;
    pot_ += blind;
    return blind;
  }

  /*
    @brief Have the appropriate players pay the ante.
    
    If it is a big blind ante and the amount the big blind can put in is not
    divisible by the number of players, then the change is counted towards the
    big blind's bet total. Does not change max_bet_ or min_raise_.

    @return The effective ante (can be less than the mandated ante if a player
        must go all in to pay it).
  */
  Chips PostAntes() {
    Chips effective_ante = ante_;
    if (big_blind_ante_) {
      // Determine the effective ante
      Chips bb_stack = stack_[PlayerIndex(2)];
      Chips effective_ante_sum = std::min(bb_stack, ante_*kPlayers);
      effective_ante = effective_ante_sum / kPlayers;

      /* Mark the big blind's ante in their bet, and charge them for the antes
         of all other players */
      bets_[PlayerIndex(2)] += effective_ante;
      stack_[PlayerIndex(2)] -= effective_ante_sum;
      pot_ += effective_ante_sum;

      /* Assign any change in the ante amount as counting towards the big
         blind's bet. */
      bets_[PlayerIndex(2)] += effective_ante_sum % kPlayers;

      // Mark the ante in the bet of all other players
      for (PlayerId i = 0; i < kPlayers; ++i) {
        if (i != PlayerIndex(2)) {
          bets_[i] += effective_ante;
        }
      }
    } else {
      for (PlayerId i = 0; i < kPlayers; ++i) {
        PostBlind(i, effective_ante);
      }
    }
    return effective_ante;
  }  // PostAntes()

  /*
    @brief Fold the current acting player.
  */
  void Fold() {
    folded_[acting_player_] = true;
    players_left_ += -1;
  }

  /*
    @brief The current acting player goes all in.
  */
  void AllIn() {
    Chips prev_bet = bets_[acting_player_];
    Chips chips = stack_[acting_player_];
    Chips total_bet = prev_bet + chips;

    // call
    if (total_bet <= max_bet_) {
      // do nothing

    // call + extra
    } else if (total_bet < max_bet_ + min_raise_) {
      PlayerId already_acted = kPlayers - pot_good_;
      no_raise_ = already_acted;
      max_bet_ = total_bet;

    // raise
    } else {
      Chips raise_amount = total_bet - max_bet_;
      min_raise_ = raise_amount;
      max_bet_ = total_bet;
      pot_good_ = kPlayers;
      no_raise_ = 0;
    }

    pot_ += chips;
    stack_[acting_player_] -= chips;
    bets_[acting_player_] += chips;
    players_all_in_ += 1;
  }  // AllIn()

  /*
    @brief The current acting player check/calls.
  */
  void CheckCall() {
    Chips prev_bet = bets_[acting_player_];
    Chips additional_bet = max_bet_ - prev_bet;

    // check
    if (additional_bet == 0) {
      // no nothing

    // call
    } else {
      pot_ += additional_bet;
      stack_[acting_player_] -= additional_bet;
      bets_[acting_player_] += additional_bet;
    }
  }

  /*
    @brief The current acting player bets.

    @param size The amount of additional chips the player is contributing to
        the pot.
  */
  void Bet(Chips size) {
    Chips prev_bet = bets_[acting_player_];
    Chips total_bet = size + prev_bet;
    Chips raise_size = total_bet - max_bet_;
    max_bet_ = total_bet;
    min_raise_ = raise_size;
    pot_good_ = kPlayers;
    no_raise_ = 0;
    pot_ += size;
    stack_[acting_player_] -= size;
    bets_[acting_player_] += size;
  }

  /*
    @brief Returns who the next acting_player_ would be if rotated.
  */
  PlayerId NextPlayer() const {
    return (acting_player_ + 1) % kPlayers;
  }

  /*
    @brief Cycle through the players until the next player who needs to act.

    @param cycleOnceBeforeCheck If true, this function will cycle one player
        before checking if it should continue to cycle.
  */
  void CyclePlayers(bool cycleBeforeCheck) {
    do {
      if (!cycleBeforeCheck) {
        cycleBeforeCheck = true;
        continue;
      }
      if (pot_good_ > 0) {
        pot_good_ -= 1;
      } else {
        no_raise_ -= 1;
      }
      cycled_ += 1;
      acting_player_ = NextPlayer();
    } while (pot_good_ + no_raise_ > 0 &&
             (folded_[acting_player_] || stack_[acting_player_] == 0 ||

              /* If all but one player is all in, we can skip this player's
                 action if they've already matched the max_bet_ because their
                 bets no longer have an effect on the game if everyone else is
                 already all in. */
              (players_left_ - players_all_in_ <= 1 &&
               bets_[acting_player_] == max_bet_)));

    /* If there is only one player left, the current acting_player_ is the
       singular winner and the game is over. */
    if (players_left_ == 1) {
      in_progress_ = false;
      return;
    }

    // The round has ended
    if (pot_good_ + no_raise_ == 0) {
      NextRound();
    }
  }

  /* @brief Continue to the next betting round. */
  void NextRound() {
    switch (round_) {
      case Round::kPreFlop:
        round_ = Round::kFlop;
        break;
      case Round::kFlop:
        round_ = Round::kTurn;
        break;
      case Round::kTurn:
        round_ = Round::kRiver;
        break;
      case Round::kRiver:
        in_progress_ = false;
        return;  /* When the game is over, we don't need to cycle acting players
                    or change other game state variables. */
    }  // switch round_
    acting_player_ = kChancePlayer;
  }

  /* @brief Checks if the pot can be awarded. Throws if not. */
  void VerifyAwardablePot(const std::string& func) const {
    if (in_progress_) {
      throw std::logic_error(func + " called when the game is still in "
                             "progress. If the game is still in progress, we "
                             "cannot award the pot yet.");
    } else if (pot_ == 0) {
      throw std::logic_error(func + " called when the pot is 0. If the pot is "
                             "0, this means we already awarded the pot for "
                             "this hand and we can't do it again.");
    }
  }

  /*
    @brief Mark all folded or mucked players as processed.
    
    Mark all other players as not processed.

    @param processed An array to write if a player has been processed or not to.
    
    @return The number of players that have not been processed.
  */
  PlayerN PlayersToProcess(std::array<bool, kPlayers>& processed) const {
    PlayerId players_to_award = kPlayers;
    for (PlayerId i = 0; i < kPlayers; ++i) {
      auto is_mucked = [&]() -> bool {
        for (CardN card = 1; card < kHandCards; ++card) {
          if (this->deck_[i * kHandCards + card - 1] !=
              this->deck_[i * kHandCards + card]) {
            return false;
          }
        }
        return true;
      };
      if (folded_[i] || is_mucked()) {
        processed[i] = true;
        players_to_award -= 1;
      } else {
        processed[i] = false;
      }
    }  // for i
    return players_to_award;
  }

  /*
    @brief Awards the pot to the singular player that remains not folded.
    
    It is assumed there is only one player left who hasn't folded or mucked
    their cards.

    @param processed An array of whether each player is folded or mucked.
  */
  void FoldVictory(const std::array<bool, kPlayers>& processed) {
    PlayerId winner = std::find(processed.begin(), processed.end(), false) -
                      processed.begin();
    if (ShouldRake()) pot_ -= CalculateRakeChips(pot_);
    stack_[winner] += pot_;
    pot_ = 0;
    std::fill(bets_.begin(), bets_.end(), 0);
  }

  /*
    @brief Awards the pot to the singular player that remains.

    That is, the singular player that is not folded or mucked. If more than one
    such player exists, we return false. Otherwise we return true. If there is
    no such player, we throw.

    @param processed An array to write if a player has been processed or not to.
      We only write to this if we don't award the pot.

    @return The number of players we still need to award. If 0 but we didn't
      throw, then we already awarded the pot.
  */
  PlayerN AwardTrivialVictory(std::array<bool, kPlayers>& processed) {
    if (players_left_ == 1) {
      FoldVictory(folded_);
      return 0;
    }
    PlayerId players_to_award = PlayersToProcess(processed);
    if (players_to_award == 1) {
      FoldVictory(processed);
      return 0;
    } else if (players_to_award == 0) {
      throw std::logic_error(
        std::string{__func__} + " called when there are no remaining players "
        "in the game."
      );
    }
    return players_to_award;
  }

  /*
    @brief Writes the hand ranking of each player to a given output array.

    @param filter Players marked as true in this array will not be ranked.
    @param output Array to write hand rankings to.
  */
  void RankPlayers(const std::array<bool, kPlayers>& filter,
                   SevenEval::Rank output[kPlayers]) const {
    for (PlayerId i = 0; i < kPlayers; ++i) {
      if (!filter[i]) {
        PublicHand<ISO_Card> player_cards = PlayerCards(i);
        std::transform(player_cards.begin(), player_cards.end(),
                       player_cards.begin(), ConvertISOtoSK);
        output[i] = std::apply(SevenEval::GetRank<>, player_cards);
      }
    }
  }  // RankPlayers()

  /*
    @brief Take chips from each player's bets to allocate the next side pot.

    @param bets An array of how much each player's bet still hasn't been
        awarded.
    @param processed An array of if each player has been processed or not. We
        don't form side pots from the bets of processed players.
    
    @return The size of the side pot formed.
  */
  Chips AllocateSidePot(std::array<Chips, kPlayers>& bets,
                        const std::array<bool, kPlayers>& processed) const {
    // Find the smallest non zero bet from a non processed player
    Chips min_bet = std::numeric_limits<Chips>::max();
    for (PlayerId i = 0; i < kPlayers; ++i) {
      if (bets[i] < min_bet && !processed[i]) min_bet = bets[i];
    }

    /* Consider the side pot formed by the smallest bet. For instance, if there
       are 3 players remaining, with player 1 all in for 100 chips, player 2 all
       in for 50 chips, and player 3 all in for 25 chips, the smallest bet is 25
       chips. So, this forms a 75 chip (25 chips * 3 players) side pot out of
       the total 175 chip pot. */
    Chips side_pot = 0;
    for (PlayerId i = 0; i < kPlayers; ++i) {
      if (bets[i] >= min_bet) {
        side_pot += min_bet;
        bets[i] -= min_bet;
      } else if (bets[i] < min_bet) {
        side_pot += bets[i];
        bets[i] -= bets[i];
      }
    }
    return side_pot;
  }  // AllocateSidePot()

  struct BestPlayersData { SevenEval::Rank rank; PlayerId n; };
  /*
    @brief Find and count the player(s) with the best hand(s).

    @param ranks Hand rankings for each player.
    @param filter Players marked as true in this array will not be considered.
    
    @return Struct containing the best rank (rank) and the number of players
        with that rank (n).
  */
  BestPlayersData BestPlayers(const SevenEval::Rank ranks[kPlayers],
                              const std::array<bool, kPlayers>& filter) const {
    std::optional<SevenEval::Rank> best_rank;
    PlayerId best_players = 0;
    for (PlayerId i = 0; i < kPlayers; ++i) {
      if (!filter[i]) {
        if (!best_rank || ranks[i] > *best_rank) {
          best_rank = ranks[i];
          best_players = 1;
        } else if (ranks[i] == *best_rank) {
          best_players += 1;
        }
      }
    }
    return BestPlayersData{*best_rank, best_players};
  }

  /*
    @brief Divides the given side_pot equally among the winners.

    @param side_pot The side pot to divide.
    @param ranks Hand rankings for each player.
    @param filter Players marked as true in this array will not be considered.
    @param best_players Specifies the rank and number of players to award.
    @param exact_awards Array to add awards for each player to.
  */
  void DivideSidePot(QuotaT side_pot, const SevenEval::Rank ranks[kPlayers],
                     const std::array<bool, kPlayers>& filter,
                     const BestPlayersData best_players,
                     QuotaT exact_awards[kPlayers]) const {
    QuotaT prize = side_pot / best_players.n;
    for (PlayerId i = 0; i < kPlayers; ++i) {
      if (!filter[i] && ranks[i] == best_players.rank) {
        exact_awards[i] += prize;
      }
    }
  }  // DivideSidePot()

  /*
    @brief Mark players whose entire bet has been awarded as processed.
    
    If the entirety of a player's bet has been awarded, they are no longer
    eligible to win any more chips because a player can only win as many chips
    as they put in from each other player.

    @param bets An array of how much each player's bet still hasn't been
        awarded.
    @param processed An array of if each player has been processed or not.

    @return The number of players marked as processed.
  */
  PlayerId MarkProcessedPlayers(const std::array<Chips, kPlayers>& bets,
                                std::array<bool, kPlayers>& processed) const {
    PlayerId players_processed = 0;
    for (PlayerId i = 0; i < kPlayers; ++i) {
      if (bets[i] == 0 && !processed[i]) {
        players_processed += 1;
        processed[i] = true;
      }
    }
    return players_processed;
  }  // MarkProcessedPlayers()

  /*
    @brief Should any rake be taken from the pot?
  */
  bool ShouldRake() {
    return rake_ && !(no_flop_no_drop_ && round_ == Round::kPreFlop);
  }

  /*
    @brief Calculate how many chips will be raked from the pot.

    @param pot The pot to rake from.

    @return The number of chips raked from the given pot.
  */
  Chips CalculateRakeChips(Chips pot) {
    double rake_quota = static_cast<double>(pot * rake_);
    Chips rake_chips = std::rint(rake_quota);
    Chips rake_chips_capped;
    if (rake_cap_) {
      rake_chips_capped = std::min(rake_cap_, rake_chips);
    } else {
      rake_chips_capped = rake_chips;
    }
    return rake_chips_capped;
  }

  /*
    @brief Take the rake from the given pot and exact awards.

    @param pot The pot to rake from.
    @param exact_awards An array of awards to take rakes from.
  */
  void Rake(Chips* pot, QuotaT exact_awards[kPlayers]) {
    Chips chips_to_rake = CalculateRakeChips(*pot);
    QuotaT proportion_not_raked = 1 - (QuotaT{1} * chips_to_rake / *pot);
    *pot -= chips_to_rake;
    for (PlayerId i = 0; i < kPlayers; ++i) {
      exact_awards[i] *= proportion_not_raked;
    }
  }

  /*
    @brief Distribute the given pot to the players, minimizing roundoff error.

    Takes an array of exact awards for each player, and maps it as best as
    possible to discrete chip values while preserving the total amount of chips
    and minimizing roundoff error. Uses the Hamilton apportionment method.

    @param pot The pot to award. It will be left at zero after this function is
        called.
    @param exact_awards What exact amount (represented by a floating point or
        fraction type) the player has won.
    @param distributions Where to distribute each player's actual discrete
        winnings.
  */
  void DistributeChips(Chips* pot, const QuotaT exact_awards[kPlayers],
                       std::array<Chips, kPlayers>& distributions) const {
    // Award each player the floor of their exact award.
    Chips pot_awarded = 0;
    for (PlayerId i = 0; i < kPlayers; ++i) {
      if (exact_awards[i] > 0) {
        Chips to_award = static_cast<Chips>(exact_awards[i]);
        distributions[i] += to_award;
        pot_awarded += to_award;
      }
    }
    if (pot_awarded < *pot) {
      /* Calculate the decimal part of each player's award (the decimal part of
         10.82 is 0.82). */
      QuotaT decimals[kPlayers];
      for (PlayerId i = 0; i < kPlayers; ++i) {
        decimals[i] = exact_awards[i] - static_cast<Chips>(exact_awards[i]);
      }

      /* Sort the player indexes in order of who will get awarded rounding
         change first. Player's with a higher decimal part of their award are
         "first in line" to get change. */
      PlayerId change_line[kPlayers];
      std::iota(change_line, change_line + kPlayers, 0);
      std::sort(change_line, change_line + kPlayers,
                [&](const PlayerId& a, const PlayerId& b) {
                  return decimals[a] > decimals[b];
                });

      /* Award change in order of change_line until all the change has been
         awarded. */
      PlayerId position = 0;
      while (pot_awarded < *pot) {
        PlayerId player = change_line[position];
        distributions[player] += 1;
        pot_awarded += 1;
        ++position;
      }
    }
    if (pot_awarded > *pot) {
        throw std::runtime_error("Rounding errors resulted in more chips being "
                                 "awarded to players than were in the pot.");
    }
    *pot -= pot_awarded;
  }  // DistributeChips()
};  // Node

}  // namespace fishbait

#endif  // AI_SRC_POKER_NODE_H_
