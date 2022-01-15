// Copyright 2021 Marzuk Rashid

import React from 'react';
import 'components/Card.css';
import { isSuit } from 'utils/hands';

/*
  This component is one player's hand in the BetView table.

  props:
    card: The card this component is representing.
    isModifying: Boolean if this card is being modified.
*/
class Card extends React.Component {
  asciiSuitToSymbol(suit) {
    if (suit === 'h') return {symbol: '♥', color: 'red'};
    if (suit === 'c') return {symbol: '♣', color: 'black'};
    if (suit === 's') return {symbol: '♠', color: 'black'};
    if (suit === 'd') return {symbol: '♦', color: 'red'};
  }
  
  shortNumToCardNum(num) {
    if (num === 'T') return '10';
    return num;
  }
  
  decomposeCard() {
    let card = this.props.card;
    if (card === null) {
      return { number: '', suit: '', color: 'black', hide: true };
    }
    if (card.length === 2) {
      let asciiSymbol = this.asciiSuitToSymbol(card[1]);
      return { number: this.shortNumToCardNum(card[0]),
               suit: asciiSymbol.symbol, color: asciiSymbol.color,
               hide: false };
    }
    if (isSuit(card)) {
      let asciiSymbol = this.asciiSuitToSymbol(card);
      return { number: '', suit: asciiSymbol.symbol, color: asciiSymbol.color,
               hide: true };
    } else {
      return { number: this.shortNumToCardNum(card), suit: '', color: 'black',
               hide: true };
    }
  }

  render() {
    let decomposed = this.decomposeCard();
    let backClass = 'cardInnerBack';
    let frontClass = 'cardInnerFront';
    let cardClass = 'card';
    if (!decomposed.hide || this.props.isModifying) {
      backClass += ' hide';
    } else {
      frontClass += ' hide';
    }
    if (this.props.isModifying) cardClass += ' active';
    return (
      <div className={cardClass}>
        <div className={backClass}></div>
        <div className={frontClass}
             style={{color: `var(--${decomposed.color})`}}>
          <div className='cardNumberLabel'>{decomposed.number}</div>
          <div className='cardSuitLabel'>{decomposed.suit}</div>
        </div>
      </div>
    );
  }  // render()
}  // class Card

export default Card;
