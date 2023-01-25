import React from 'react';
import './Card.css';
import { isSuit } from 'utils/hands';

/*
  This component is one player's hand in the BetView table.

  props:
    card: The card this component is representing.
    isModifying: Boolean if this card is being modified.
    shouldHide: If this card should be hidden by default when not being edited.
*/
class Card extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      hidden: this.props.shouldHide,
    }
  }

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
      return { number: '', suit: '', color: 'black', isComplete: false };
    }
    if (card.length === 2) {
      let asciiSymbol = this.asciiSuitToSymbol(card[1]);
      return { number: this.shortNumToCardNum(card[0]),
               suit: asciiSymbol.symbol, color: asciiSymbol.color,
               isComplete: true };
    }
    if (isSuit(card)) {
      let asciiSymbol = this.asciiSuitToSymbol(card);
      return { number: '', suit: asciiSymbol.symbol, color: asciiSymbol.color,
               isComplete: false };
    } else {
      return { number: this.shortNumToCardNum(card), suit: '', color: 'black',
               isComplete: false };
    }
  }

  render() {
    let decomposed = this.decomposeCard();
    let cardContainerClass = 'cardContainer';
    let cardClass = 'card';
    let cardClick = undefined;
    if (this.props.isModifying) {
      cardContainerClass += ' active';
    } else if (decomposed.isComplete) {
      cardClass += ' hideable';
      cardClick = () => {
        this.setState({ hidden: !this.state.hidden });
      }
    }
    if (!this.props.isModifying &&
        (this.state.hidden || !decomposed.isComplete)) {
      cardClass += ' flipped';
    }
    return (
      <div className={cardContainerClass}>
        <div className={cardClass} onClick={cardClick}>
          <div className='cardInnerBack'>
            <div className='cardBackPattern'></div>
          </div>
          <div className='cardInnerFront'
              style={{color: `var(--${decomposed.color})`}}>
            <div className='cardNumberLabel'>{decomposed.number}</div>
            <div className='cardSuitLabel'>{decomposed.suit}</div>
          </div>
        </div>
      </div>
    );
  }  // render()
}  // class Card

export default Card;
