// Copyright 2021 Marzuk Rashid

import React from 'react';
import 'components/CardBoard.css';
import Card from './Card';

/*
  This component creates the board of public cards at the top of the interface.

  props:
    board: The board cards to display.
    modifyingBoard: Object of {start, end, offset, scratch} if we are currently
        modifying the board. Otherwise null.
*/
class CardBoard extends React.Component {
  render() {
    let boardCards = [];
    let modifyingBoard = this.props.modifyingBoard;
    for (let i = 0; i < 5; ++i) {
      let isModifying = (() => {
        if (modifyingBoard === null) return false;
        if (i === modifyingBoard.start + modifyingBoard.offset) return true;
        return false;
      })();
      let card = (() => {
        if (modifyingBoard === null) return this.props.board[i];
        if (i >= modifyingBoard.start && i < modifyingBoard.end) {
          return modifyingBoard.scratch[i];
        } else {
          return this.props.board[i];
        }
      })();
      boardCards.push(
        <Card card={card} isModifying={isModifying} key={i}/>
      );
    }
    return <div className='cardView'>{boardCards}</div>;
  }  // render()
}  // class CardBoard

export default CardBoard;
