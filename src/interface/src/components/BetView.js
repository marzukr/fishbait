// Copyright 2021 Marzuk Rashid

import React from 'react';
import 'components/BetView.css';
import PlayerRow from 'components/PlayerRow';

/*
  This component creates the board of public cards at the top of the interface.

  props:
    players: How many players are in the game?
    playerNames: An array of the names of each player.
    fishbaitSeat: Fishbait's player id.
    button: The player id of the player on the button.
    actingPlayer: The player id of the current acting player.
    bets: An array of how much each player has bet.
    stack: An array of how many chips each player has in their stack.
    neededToCall: How many chips the current acting player needs to call if
        applicable. Otherwise null.
    pot: The current size of the pot.
    minRaise: The minimum amount the current player must raise if he wishes to
        do so.
    hands: Array of hands for each player.
    folded: Array of if each player is folded or not.
    lastAction: Array of last actions for each player.
    modifyingHand: Object of {player, card, scratch} if we are currently
        modifying a player's hand. Otherwise null.
    enteredAction: The action entered by the user for the current acting player.
*/
class BetView extends React.Component {
  render() {
    let playerInfoBoxes = [];
    for (let i = 0; i < this.props.players; ++i) {
      let playerName = i === this.props.fishbaitSeat
          ? 'Fishbait 🐟' : this.props.playerNames[i];
      let modifying = this.props.modifyingHand !== null &&
                      this.props.modifyingHand.player === i
          ? this.props.modifyingHand.card
          : null;
      let hand = modifying === null
          ? this.props.hands[i]
          : this.props.modifyingHand.scratch;
      let isActingPlayer = i === this.props.actingPlayer;
      let action = isActingPlayer
        ? this.props.enteredAction
        : this.props.lastAction[i];
      playerInfoBoxes.push(
        <PlayerRow key={i} playerId={i} playerName={playerName}
                   isButton={i === this.props.button}
                   isActingPlayer={isActingPlayer} bet={this.props.bets[i]}
                   stack={this.props.stack[i]} hand={hand}
                   folded={this.props.folded[i]} action={action}
                   modifying={modifying}/>
      );
    }
    let callAmount = this.props.neededToCall === null
        ? 'n/a'
        : this.props.neededToCall;
    let minRaise = this.props.minRaise === null
        ? 'n/a'
        : this.props.minRaise;
    return (
      <div className='betView'>
        <div className='topInfo'>
          <span>Pot: {this.props.pot}</span>
          <span>Min Raise: {minRaise}</span>
          <span>To Call: {callAmount}</span>
        </div>
        {playerInfoBoxes}
      </div>
    );
  }  // render()
}  // class BetView

export default BetView;
