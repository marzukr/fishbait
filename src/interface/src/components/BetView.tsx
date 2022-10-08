// Copyright 2021 Marzuk Rashid

import React from 'react';

import PlayerRow from 'components/PlayerRow';
import { GameState, CardT, PartialAction } from 'utils/api';

import './BetView.css';


/** An object describing the hand currently being modified */
interface ModifyingHandProps {
  /** The player whose hand is being modified */
  player: number;
  /** Which card of the player's hand is being modified */
  card: number;
  /** The values we are going to set the player's new hand to be */
  scratch: CardT[];
}

interface BetViewProps {
  /** The current game state */
  gameState: GameState;
  /** If the game is currently at a chance node */
  isChanceNode: boolean;
  /**
   * Details of the hand we are currently modifying or null if we are not
   * currently modifying a hand
   */
  modifyingHand: ModifyingHandProps | null;
  /** Details of the action we are currently contemplating */
  enteredAction: PartialAction;
}

export const BetView: React.FC<BetViewProps> = (
  { gameState, isChanceNode, modifyingHand, enteredAction }
) => {
  const actingPlayer = gameState.inProgress ? gameState.actingPlayer : null;
  const neededToCall = (
    isChanceNode || !gameState.inProgress
      ? null
      : gameState.neededToCall
  );
  const minRaise = (
    isChanceNode || !gameState.inProgress
      ? null
      : gameState.minRaise
  );

  const playerInfoBoxes = [];
  for (let i = 0; i < gameState.players; ++i) {
    const playerName = (
      i === gameState.fishbaitSeat 
        ? 'Fishbait 🐟'
        : gameState.playerNames[i]
    );
    const modifying = (
      modifyingHand !== null && modifyingHand.player === i
        ? modifyingHand.card
        : null
    );
    const hand = (
      modifying === null
        ? gameState.hands[i]
        : modifyingHand && modifyingHand.scratch
    );
    const isActingPlayer = i === actingPlayer;
    const action = isActingPlayer ? enteredAction : gameState.lastAction[i];
    playerInfoBoxes.push(
      <PlayerRow
        key={i} playerId={i} playerName={playerName}
        isButton={i === gameState.button} isActingPlayer={isActingPlayer}
        bet={gameState.bets[i]} stack={gameState.stack[i]} hand={hand}
        folded={gameState.folded[i]} action={action} modifying={modifying}
        hideCards={i === gameState.fishbaitSeat}
      />
    );
  }
  const callAmount = neededToCall === null ? 'n/a' : neededToCall;
  const minRaiseLabel = minRaise === null ? 'n/a' : minRaise;
  return (
    <div className='betView'>
      <div className='topInfo'>
        <span>Pot: {gameState.pot}</span>
        <span>Min Raise: {minRaiseLabel}</span>
        <span>To Call: {callAmount}</span>
      </div>
      {playerInfoBoxes}
    </div>
  );
}
