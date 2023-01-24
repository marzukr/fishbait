import React from 'react';

import Card from 'screens/Game/Card';
import { BoardNeedsCards, CardT } from 'utils/api';

interface CardBoardProps {
  /** The board cards to display */
  board: CardT[],
  /**
   * Which cards we are currently modifying. Null if we are not modifying the
   * board.
   */
  boardNeedsCards: BoardNeedsCards | null,
  /** Which card is currently selected. Null if no card is selected. */
  offset: number | null,
  /** The boards cards we have entered but not submitted yet. */
  scratch: CardT[],
}

export const CardBoard: React.FC<CardBoardProps> = ({
  board, boardNeedsCards, offset, scratch
}) => {
  const boardCards = board.map((boardCard, idx) => {
    const isModifying = (
      boardNeedsCards !== null
      && offset !== null
      && idx === boardNeedsCards.start + offset
    );
    const card = (
      boardNeedsCards === null
        ? boardCard
        : idx >= boardNeedsCards.start && idx < boardNeedsCards.end
          ? scratch[idx]
          : boardCard
    );
    return <Card card={card} isModifying={isModifying} key={idx}/>
  });
  return (
    <div className={`
      h-24 flex justify-evenly px-2.5 inset-x-0 fixed max-w-sm mx-auto top-12
      z-10
    `}>
      {boardCards}
    </div>
  );
};
