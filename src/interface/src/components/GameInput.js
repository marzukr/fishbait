// Copyright 2021 Marzuk Rashid

import React from 'react';
import 'components/GameInput.css';

/*
  This components is for the matrix of buttons at the bottom of the screen for
  the use to either input bets or cards.

  props:
    type: either 'card' to create the card selection input or 'bet' to create
        the bet selection input.
    handler: Function to call when input is received.
    disabled: Array of codes to disable.
    canCheck: If check is a legal move. Only applicable for the bet input.
*/
class GameInput extends React.Component {
  constructor(props) {
    super(props);
    this.handlePress = this.handlePress.bind(this);
  }

  handlePress(code) {
    if (this.props.disabled.indexOf(code) === -1) {
      this.props.handler(code);
    }
  }

  render() {
    let labels;
    if (this.props.type === 'card') {
      labels = [
        { label: 'A', class: 'nums', code: 'A' },
        { label: 'K', class: 'nums', code: 'K' },
        { label: 'Q', class: 'nums', code: 'Q' },
        { label: 'J', class: 'nums', code: 'J' },
        { label: 'T', class: 'nums', code: 'T' },
        { label: <i className='fas fa-chevron-right'/>, class: 'next',
          code: 'next' },
        { label: '9', class: 'nums', code: '9' },
        { label: '8', class: 'nums', code: '8' },
        { label: '7', class: 'nums', code: '7' },
        { label: '6', class: 'nums', code: '6' },
        { label: '♦', class: 'red', code: 'd' },
        { label: '♣', class: 'black', code: 'c' },
        { label: '5', class: 'nums', code: '5' },
        { label: '4', class: 'nums', code: '4' },
        { label: '3', class: 'nums', code: '3' },
        { label: '2', class: 'nums', code: '2' },
        { label: '♥', class: 'red', code: 'h' },
        { label: '♠', class: 'black', code: 's' }
      ];
    } else if (this.props.type === 'bet') {
      labels = [
        { label: '9', class: 'nums', code: 9 },
        { label: '8', class: 'nums', code: 8 },
        { label: '7', class: 'nums', code: 7 },
        { label: <i className='fas fa-backspace' style={{fontSize: '85%'}}></i>,
          class: 'black', code: 'delete', cols: '4 / 5', rows: '1 / 3'},
        { label: 'fold', class: 'black', code: 'fold', cols: '5 / 7' },
        { label: '6', class: 'nums', code: 6 },
        { label: '5', class: 'nums', code: 5 },
        { label: '4', class: 'nums', code: 4 },
        { label: (this.props.canCheck ? 'check' : 'call'), class: 'black',
          code: 'checkcall', cols: '5 / 7' },
        { label: '3', class: 'nums', code: 3 },
        { label: '2', class: 'nums', code: 2 },
        { label: '1', class: 'nums', code: 1 },
        { label: '0', class: 'nums', code: 0 },
        { label: <i className='fas fa-exclamation-triangle'></i>,
          class: 'black', code: 'allin' },
        { label: <i className='fas fa-chevron-right'/>, class: 'next',
          code: 'next' }
      ];
    } else if (this.props.type === 'new hand') {
      labels = [
        { label: 'New Hand', class: 'nums', code: 'new hand', cols: '2 / 6',
          rows: '2 / 3' }
      ];
    }
    let buttons = [];
    for (let i = 0; i < labels.length; ++i) {
      let style = {};
      if ('cols' in labels[i]) style['gridColumn'] = labels[i].cols;
      if ('rows' in labels[i]) style['gridRow'] = labels[i].rows;
      let className = 'gameInputButton ' + labels[i].class;
      if (this.props.disabled.indexOf(labels[i].code) !== -1) {
        className += ' disabled';
      }
      buttons.push(
        <button className={className} id={labels[i].code} key={i}
                onClick={_ => this.handlePress(labels[i].code)}
                style={style}>
          {labels[i].label}
        </button>
      );
    }
    return <div className='gameInput'>{buttons}</div>;
  }  // render()
}  // class GameInput

export default GameInput;
