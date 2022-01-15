// Copyright 2021 Marzuk Rashid

import React from 'react';
import GameInput from 'components/GameInput';
import CardBoard from 'components/CardBoard';
import BetView from 'components/BetView';
import { constructCard, asciiStringToIso } from 'utils/hands';

/*
  This screen is the main screen of the interface that allows users to interact
  with fishbait.

  props:
    gameState: Object representing the game state to render.
    apiCall: Callback to make an api call.
    toggleSettings: Callback to go to the settings page.

  state:
    handScratch: Where an entered hand is written if a hand is being input.
    selectedCard: Which card of the hand is being input if applicable.
    actionScratch: Object of action, size where an entered action is written if
        an action is being input.
    boardScratch: Array of board cards where the entered board cards are written
        if board cards are being input.
    selectedBoardOffset: Offset of the selected board card being input from the
        start of the set of board cards being input. For example offset 0 on the
        turn corresponds to card 4 on the board.
*/
class Game extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      handScratch: [null, null],
      selectedCard: 0,
      actionScratch: { action: null, size: null },
      boardScratch: [null, null, null, null, null],
      selectedBoardOffset: 0,
    };
    this.handleBetInput = this.handleBetInput.bind(this);
    this.handleHandInput = this.handleHandInput.bind(this);
    this.handleBoardInput = this.handleBoardInput.bind(this);
  }

  isChanceNode() {
    let gameState = this.props.gameState;
    return gameState.acting_player === gameState.players &&
           gameState.in_progress;
  }

  isFishbaitTurn() {
    let gameState = this.props.gameState;
    return gameState.acting_player === gameState.fishbait_seat &&
           gameState.in_progress;
  }

  canCheck() {
    return this.props.gameState.in_progress && !this.isChanceNode() &&
           this.props.gameState.needed_to_call === 0;
  }

  canCheckCall() {
    let gameState = this.props.gameState;
    return gameState.in_progress && !this.isChanceNode() &&
           gameState.needed_to_call < gameState.stack[gameState.acting_player];
  }

  handleBetInput(code) {
    if (code === 'fold') {
      this.setState({ actionScratch: { action: 'Fold', size: null } });
    } else if (code === 'checkcall') {
      let actionName = this.canCheck() ? 'Check' : 'Call';
      this.setState({ actionScratch: { action: actionName, size: null } });
    } else if (code === 'allin') {
      this.setState({ actionScratch: { action: 'All In', size: null } });
    } else if (code === 'bet') {
      this.setState({ actionScratch: { action: 'Bet', size: null } });
    } else if (code !== 'next') {
      let prevSize = this.state.actionScratch.size === null
          ? 0
          : this.state.actionScratch.size;
      let newSize = prevSize * 10 + code;
      this.setState({ actionScratch: { action: 'Bet', size: newSize } });
    } else {
      this.props.apiCall('apply', this.state.actionScratch);
      this.setState({ actionScratch: { action: null, size: null } });
    }
  }

  handleHandInput(code) {
    let sendCards = () => {
      let scratchMucked =
          this.state.handScratch[0] === this.state.handScratch[1] &&
          this.state.handScratch[0] === null;
      if (scratchMucked) {
        this.props.apiCall('set_hand', { hand: this.state.handScratch });
      } else {
        let isoHand = this.state.handScratch.map(c => asciiStringToIso[c]);
        this.props.apiCall('set_hand', { hand: isoHand });
      }
      this.setState({ handScratch: [null, null], selectedCard: 0 });
    };
    if (code !== 'next') {
      let newCard =
          constructCard(this.state.handScratch[this.state.selectedCard], code);
      let newHand = this.state.handScratch.slice();
      newHand[this.state.selectedCard] = newCard;
      this.setState({ handScratch: newHand });
    } else if (this.state.selectedCard === 0) {
      if (this.state.handScratch[0] === null) {
        sendCards();
      } else {
        this.setState({ selectedCard: 1 });
      }
    } else {
      sendCards();
    }
  }

  handleBoardInput(code) {
    let toBoard = this.props.gameState.board_needs_cards;
    let idx = toBoard.start + this.state.selectedBoardOffset;
    if (code !== 'next') {
      let currentIdx = toBoard.start + this.state.selectedBoardOffset;
      let newCard = constructCard(this.state.boardScratch[currentIdx], code);
      let newHand = this.state.boardScratch.slice();
      newHand[currentIdx] = newCard;
      this.setState({ boardScratch: newHand });
    } else if (idx < toBoard.end - 1) {
      let newOffset = this.state.selectedBoardOffset + 1;
      this.setState({ selectedBoardOffset: newOffset });
    } else {
      let newBoard = [];
      for (let i = 0; i < 5; ++i) {
        if (i >= toBoard.start && i < toBoard.end) {
          newBoard.push(this.state.boardScratch[i]);
        } else {
          newBoard.push(this.props.gameState.board[i]);
        }
      }
      newBoard = newBoard.map(c => asciiStringToIso[c]);
      this.props.apiCall('set_board', { board: newBoard });
      this.setState({ boardScratch: [null, null, null, null, null],
                      selectedBoardOffset: 0 });
    }
  }

  verifyScratchForPlayer(player) {
    let card = this.state.handScratch[this.state.selectedCard];
    if (card === null) {
      return this.state.selectedCard === 0 && this.props.gameState.can_muck;
    }
    if (card.length !== 2) return false;
    let otherCard = this.state.handScratch[this.state.selectedCard ^ 1];
    if (card === otherCard) return false;
    let hands = this.props.gameState.hands;
    for (let i = 0; i < this.props.gameState.players; ++i) {
      if (i === player) continue;
      if (hands[i] === null) continue;
      if (card === hands[i][0] || card === hands[i][1]) {
        return false;
      }
    }
    let board = this.props.gameState.board;
    for (let i = 0; i < 5; ++i) {
      if (board[i] === null) break;
      if (card === board[i]) return false;
    }
    return true;
  }

  verifyScratchAction() {
    let action = this.state.actionScratch.action;
    let size = this.state.actionScratch.size;
    if (action === null) return false;
    if (action === 'Bet') {
      if (size === null) return false;
      let gameState = this.props.gameState;
      if (size - gameState.needed_to_call < gameState.min_raise) {
        return false;
      }
      if (size >= gameState.stack[gameState.acting_player]) {
        return false;
      }
    }
    return true;
  }

  verifyScratchBoard(toBoard) {
    let start = toBoard.start;
    let end = toBoard.end;
    let idx = start + this.state.selectedBoardOffset;
    let gameState = this.props.gameState;
    let boardScratch = this.state.boardScratch;
    if (boardScratch[idx] === null) return false;
    if (boardScratch[idx].length !== 2) return false;
    for (let i = 0; i < 5; ++i) {
      if (i === idx) continue;
      if (i >= start && i < end) {
        if (boardScratch[i] === boardScratch[idx]) return false;
      } else {
        if (gameState.board[i] === boardScratch[idx]) return false;
      }
    }
    for (let i = 0; i < gameState.players; ++i) {
      if (gameState.hands[i] === null) continue;
      for (let j = 0; j < 2; ++j) {
        if (boardScratch[idx] === gameState.hands[i][j]) return false;
      }
    }
    return true;
  }

  render() {
    let gameState = this.props.gameState;
    let neededToCall = this.isChanceNode() || !gameState.in_progress
        ? null
        : gameState.needed_to_call;
    let minRaise = this.isChanceNode() || !gameState.in_progress
        ? null
        : gameState.min_raise;
    let toDeal = gameState.player_needs_hand;
    let modifyingHand = (() => {
      if (toDeal === null) {
        return null;
      } else {
        return { player: toDeal, card: this.state.selectedCard,
                 scratch: this.state.handScratch };
      }
    })();
    let toBoard = gameState.board_needs_cards;
    let properInput = (() => {
      if (toDeal !== null) {
        let disabled = this.verifyScratchForPlayer(toDeal) ? [] : ['next'];
        return <GameInput type={'card'} handler={this.handleHandInput}
                          disabled={disabled}/>
      } else if (!this.isChanceNode() && !this.isFishbaitTurn() &&
                 gameState.in_progress) {
        let disabled = (() => {
          let retVal = [];
          if (this.canCheck()) retVal.push('fold');
          if (!this.canCheckCall()) retVal.push('checkcall');
          if (!this.verifyScratchAction()) retVal.push('next');
          return retVal;
        })();
        return <GameInput type={'bet'} handler={this.handleBetInput}
                          disabled={disabled} canCheck={this.canCheck()}/>
      } else if (toBoard !== null) {
        let disabled = this.verifyScratchBoard(toBoard) ? [] : ['next'];
        return <GameInput type={'card'} handler={this.handleBoardInput}
                          disabled={disabled}/>
      } else if (gameState.pot === 0 && !gameState.in_progress) {
        let callback = () => {
          this.props.apiCall('new_hand');
        };
        return <GameInput type={'new hand'} handler={callback} disabled={[]}/>
      }
    })();
    let modifyingBoard = (() => {
      if (toBoard === null) {
        return null;
      } else {
        return { start: toBoard.start, end: toBoard.end,
                 offset: this.state.selectedBoardOffset,
                 scratch: this.state.boardScratch };
      }
    })();
    let actingPlayer = gameState.in_progress ? gameState.acting_player : null;
    return (
      <div className='boxSpace'>
        <div className='header'>
          <div className='headerPad'></div>
          <div>{gameState.round}</div>
          <button className='headerButton'
                  onClick={this.props.toggleSettings}>
            <i className='fas fa-cog'></i>
          </button>
        </div>
        <CardBoard board={gameState.board} modifyingBoard={modifyingBoard}/>
        <BetView players={gameState.players}
                 playerNames={gameState.player_names}
                 fishbaitSeat={gameState.fishbait_seat}
                 button={gameState.button} actingPlayer={actingPlayer}
                 bets={gameState.bets} stack={gameState.stack}
                 neededToCall={neededToCall} pot={gameState.pot}
                 minRaise={minRaise} hands={gameState.hands}
                 folded={gameState.folded} lastAction={gameState.last_action}
                 modifyingHand={modifyingHand}
                 enteredAction={this.state.actionScratch}/>
        {properInput}
      </div>
    );
  }  // render()
}  // class Game

export default Game;
