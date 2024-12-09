import React, { useState } from 'react';

import {
  GameInput,
  cardInputLabels,
  GameInputCode,
  makeBetLabels,
  newHandLabels,
} from 'screens/Game/GameInput';
import { CardBoard } from 'screens/Game/CardBoard';
import { BetView } from 'screens/Game/BetView/BetView';
import { constructCard, asciiStringToIso } from 'utils/hands';
import {
  Api, Action, BoardNeedsCards, useSafeAsync, CardT, PartialAction
} from 'utils/api';

interface GameProps {
  /** Our Api object */
  api: Api,
  /** A callback to open the settings page */
  toggleSettings: () => void,
}

/**
 * This screen is the main screen of the interface that allows users to interact
 * with fishbait.
 */
export const Game: React.FC<GameProps> = ({ api, toggleSettings }) => {
  // Where an entered hand is written if a hand is being input:
  const [handScratch, setHandScratch] = useState<CardT[]>([null, null]);

  // Which card of the hand is being input if applicable:
  const [selectedCard, setSelectedCard] = useState(0);

  // Object of { action, size } where an entered action is written if an action
  // is being input:
  const [actionScratch, setActionScratch] = useState<PartialAction>({
    action: null,
    size: null,
    actionIdx: null,
  });

  // Array of board cards where the entered board cards are written if board
  // cards are being input:
  const [boardScratch, setBoardScratch] = useState<CardT[]>(
    [null, null, null, null, null]
  );

  // Offset of the selected board card being input from the start of the set of
  // board cards being input. For example offset 0 on the turn corresponds to
  // card 4 on the board. If null, then no card is selected.
  const [
    selectedBoardOffset,
    setSelectedBoardOffset
  ] = useState<number | null>(0);

  const safeAsync = useSafeAsync();

  const gameState = api.gameState;
  if (gameState === null) return null;

  const isChanceNode = (
    gameState.actingPlayer === gameState.players && gameState.inProgress
  );

  const isFishbaitTurn = (
    gameState.actingPlayer === gameState.fishbaitSeat && gameState.inProgress
  );

  const canCheck = (
    gameState.inProgress && !isChanceNode && gameState.neededToCall === 0
  );

  const canCheckCall = (
    gameState.inProgress
    && !isChanceNode
    && gameState.neededToCall < gameState.stack[gameState.actingPlayer]
  );

  const handleBetInput = (code: GameInputCode) => {
    if (code === 'fold') {
      setActionScratch({ action: Action.FOLD, size: null, actionIdx: null });
    } else if (code === 'checkcall') {
      setActionScratch({
        action: canCheck ? Action.CHECK : Action.CALL,
        size: null,
        actionIdx: null,
      });
    } else if (code === 'allin') {
      setActionScratch({
        action: Action.ALL_IN,
        size: null,
        actionIdx: null,
      });
    } else if (code === 'delete') {
      const newAction = (
        actionScratch.action === Action.BET && actionScratch.size !== null
          ? Action.BET
          : null
      );
      // If there is more than 1 digit, divide by 10 and truncate to delete the
      // last number:
      const newSize = (
        actionScratch.action === Action.BET && actionScratch.size !== null
        && actionScratch.size > 10
          ? actionScratch.size / 10 >> 0
          : null
      );
      setActionScratch({ action: newAction, size: newSize, actionIdx: null });
    } else if (typeof code === 'number') {
      const prevSize = actionScratch.size === null
        ? 0
        : actionScratch.size;
      const newSize = prevSize * 10 + code;
      setActionScratch({ action: Action.BET, size: newSize, actionIdx: null });

    // code is 'next':
    } else {
      safeAsync(api.apply(actionScratch)).then(() => setActionScratch({
        action: null,
        size: null,
        actionIdx: null,
      }));
    }
  }

  const handleHandInput = (code: GameInputCode) => {
    const sendCards = () => {
      const scratchMucked = (
        handScratch[0] === handScratch[1] && handScratch[0] === null
      );
      const afterSend = () => {
        setHandScratch([null, null]);
        setSelectedCard(0);
      };
      if (scratchMucked) {
        safeAsync(api.setHand([null, null])).then(afterSend);
      } else {
        const isoHand = handScratch.map(c => c ? asciiStringToIso[c] : null);
        safeAsync(api.setHand(isoHand)).then(afterSend);
      }
    };
    if (code !== 'next') {
      const newCard = constructCard(handScratch[selectedCard], String(code));
      const newHand = handScratch.slice();
      newHand[selectedCard] = newCard;
      setHandScratch(newHand);
    } else if (selectedCard === 0) {
      if (handScratch[0] === null) {
        sendCards();
      } else {
        setSelectedCard(1);
      }
    } else {
      sendCards();
    }
  }

  const handleBoardInput = (code: GameInputCode) => {
    const toBoard = gameState.boardNeedsCards;
    if (toBoard === null || selectedBoardOffset === null) return;
    const idx = toBoard.start + selectedBoardOffset;
    if (code !== 'next') {
      const newCard = constructCard(boardScratch[idx], String(code));
      const newHand = boardScratch.slice();
      newHand[idx] = newCard;
      setBoardScratch(newHand);
    } else if (idx < toBoard.end - 1) {
      const newOffset = selectedBoardOffset + 1;
      setSelectedBoardOffset(newOffset);
    } else {
      const newBoard = [];
      for (let i = 0; i < 5; ++i) {
        if (i >= toBoard.start && i < toBoard.end) {
          newBoard.push(boardScratch[i]);
        } else {
          newBoard.push(gameState.board[i]);
        }
      }
      const newIsoBoard = newBoard.map(c => c ? asciiStringToIso[c] : null);
      setSelectedBoardOffset(null);
      safeAsync(api.setBoard(newIsoBoard)).then(() => {
        setBoardScratch([null, null, null, null, null]);
        setSelectedBoardOffset(0);
      });
    }
  }

  const verifyScratchForPlayer = (player: number) => {
    const card = handScratch[selectedCard];
    if (card === null) return selectedCard === 0 && gameState.canMuck;
    if (card.length !== 2) return false;
    const otherCard = handScratch[selectedCard ^ 1];
    if (card === otherCard) return false;
    const hands = gameState.hands;
    for (let i = 0; i < gameState.players; ++i) {
      if (i === player) continue;
      const currentHand = hands[i];
      if (currentHand === null) continue;
      if (card === currentHand[0] || card === currentHand[1]) return false;
    }
    const board = gameState.board;
    for (let i = 0; i < 5; ++i) {
      if (board[i] === null) break;
      if (card === board[i]) return false;
    }
    return true;
  }

  const verifyScratchAction = () => {
    let action = actionScratch.action;
    let size = actionScratch.size;
    if (action === null) return false;
    if (action === Action.BET) {
      if (size === null) return false;
      if (size - gameState.neededToCall < gameState.minRaise) return false;
      if (size >= gameState.stack[gameState.actingPlayer]) return false;
    }
    return true;
  }

  const verifyScratchBoard = (toBoard: BoardNeedsCards) => {
    if (selectedBoardOffset === null) return false;
    const start = toBoard.start;
    const end = toBoard.end;
    const idx = start + selectedBoardOffset;
    const currentBoardScratch = boardScratch[idx];
    if (currentBoardScratch === null) return false;
    if (currentBoardScratch.length !== 2) return false;
    for (let i = 0; i < 5; ++i) {
      if (i === idx) continue;
      if (i >= start && i < end) {
        if (boardScratch[i] === boardScratch[idx]) return false;
      } else {
        if (gameState.board[i] === boardScratch[idx]) return false;
      }
    }
    for (let i = 0; i < gameState.players; ++i) {
      const currentHand = gameState.hands[i];
      if (currentHand === null) continue;
      for (let j = 0; j < 2; ++j) {
        if (boardScratch[idx] === currentHand[j]) return false;
      }
    }
    return true;
  }

  const toDeal = gameState.playerNeedsHand;
  const modifyingHand = (() => {
    if (toDeal === null) {
      return null;
    } else {
      return {
        player: toDeal,
        card: selectedCard,
        scratch: handScratch
      };
    }
  })();
  const toBoard = gameState.boardNeedsCards;
  const properInput = (() => {
    if (toDeal !== null) {
      const disabled: GameInputCode[] = (
        verifyScratchForPlayer(toDeal) ? [] : ['next']
      );
      return (
        <GameInput
          labels={cardInputLabels} handler={handleHandInput} disabled={disabled}
        />
      );
    } else if (!isChanceNode && !isFishbaitTurn && gameState.inProgress) {
      const disabled = (() => {
        const retVal: GameInputCode[] = [];
        if (canCheck) retVal.push('fold');
        if (!canCheckCall) retVal.push('checkcall');
        if (!verifyScratchAction()) retVal.push('next');
        return retVal;
      })();
      return (
        <GameInput
          labels={makeBetLabels(canCheck)} handler={handleBetInput}
          disabled={disabled}
        />
      );
    } else if (toBoard !== null) {
      const disabled: GameInputCode[] = (
        verifyScratchBoard(toBoard) ? [] : ['next']
      );
      return (
        <GameInput
          labels={cardInputLabels} handler={handleBoardInput}
          disabled={disabled}
        />
      );
    } else if (gameState.pot === 0 && !gameState.inProgress) {
      const callback = () => api.newHand();
      return (
        <GameInput labels={newHandLabels} handler={callback} disabled={[]}/>
      );
    }
  })();

  return (
    <div className='boxSpace'>
      <div className='header max-w-sm fixed bg-white mx-auto z-30 h-12'>
        <div className='headerPad'></div>
        <div>{gameState.round}</div>
        <button className='headerButton'
                onClick={toggleSettings}>
          <i className='fas fa-cog'></i>
        </button>
      </div>
      <CardBoard
        board={gameState.board} boardNeedsCards={gameState.boardNeedsCards}
        offset={selectedBoardOffset} scratch={boardScratch}
      />
      <div className={`
        flex flex-col h-full fixed w-full max-w-sm
      `}>
        <div className='h-12 flex-none'/>
        <div className='h-24 flex-none'/>
        <BetView
          gameState={gameState} isChanceNode={isChanceNode}
          modifyingHand={modifyingHand} enteredAction={actionScratch}
        />
      </div>
      {properInput}
    </div>
  );
}
