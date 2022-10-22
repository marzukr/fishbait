// Copyright 2021 Marzuk Rashid

import React from 'react';
import 'components/GameInput.css';

type NextCode = 'next';

type CardValueCode = (
  'A' | 'K' | 'Q' | 'J' | 'T' | '9' | '8' | '7' | '6' | '5' | '4' | '3' | '2'
);
type CardSuiteCode =  'd' | 'c' | 'h' | 's';
type CardInputCode = CardValueCode | CardSuiteCode | NextCode;

type DeleteCode = 'delete';
type NumpadCode = 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0;
type ActionCode = 'fold' | 'checkcall' | 'allin';
type BetInputCode = ActionCode | NumpadCode | DeleteCode | NextCode;

export type GameInputCode = CardInputCode | BetInputCode | 'new hand';

interface GameInputLabel {
  label: JSX.Element | string;
  class: string;
  code: GameInputCode;
  cols?: string;
  rows?: string;
}

interface GameInputProps {
  labels: GameInputLabel[];
  /** Function to call when input is received. */
  handler: (code: GameInputCode) => void;
  /** Array of codes to disable */
  disabled: GameInputCode[];
}

/**
 * This components is for the matrix of buttons at the bottom of the screen for
 * the use to either input bets or cards.
 */
export const GameInput: React.FC<GameInputProps> = ({
  labels,
  handler,
  disabled,
}) => {
  const handlePress = (code: GameInputCode) => {
    if (disabled.indexOf(code) !== -1) return;
    handler(code);
  };

  const buttons = labels.map((label, idx) => {
    const style = {
      gridColumn: label.cols,
      gridRow: label.rows,
    };
    const enabledClassName = 'gameInputButton ' + label.class;
    const className = (
      disabled.indexOf(label.code) !== -1
        ? enabledClassName + ' disabled'
        : enabledClassName
    );
    return (
      <button
        className={className} id={String(label.code)} key={idx}
        onClick={_ => handlePress(label.code)} style={style}
      >
        {label.label}
      </button>
    )
  });
  return (
    <div className={`
      gameInput max-w-sm fixed bg-white mx-auto z-10 h-52 pb-4
    `}>
      {buttons}
    </div>
  );
}

export const cardInputLabels: GameInputLabel[] = [
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

export const makeBetLabels = (canCheck: boolean): GameInputLabel[] => [
  { label: '9', class: 'nums', code: 9 },
  { label: '8', class: 'nums', code: 8 },
  { label: '7', class: 'nums', code: 7 },
  { label: <i className='fas fa-backspace' style={{fontSize: '85%'}}></i>,
    class: 'black', code: 'delete', cols: '4 / 5', rows: '1 / 3'},
  { label: 'fold', class: 'black', code: 'fold', cols: '5 / 7' },
  { label: '6', class: 'nums', code: 6 },
  { label: '5', class: 'nums', code: 5 },
  { label: '4', class: 'nums', code: 4 },
  { label: (canCheck ? 'check' : 'call'), class: 'black', code: 'checkcall',
    cols: '5 / 7' },
  { label: '3', class: 'nums', code: 3 },
  { label: '2', class: 'nums', code: 2 },
  { label: '1', class: 'nums', code: 1 },
  { label: '0', class: 'nums', code: 0 },
  { label: <i className='fas fa-exclamation-triangle'></i>,
    class: 'black', code: 'allin' },
  { label: <i className='fas fa-chevron-right'/>, class: 'next',
    code: 'next' }
];

export const newHandLabels: GameInputLabel[] = [
  { label: 'New Hand', class: 'nums', code: 'new hand', cols: '2 / 6',
    rows: '2 / 3' }
];
