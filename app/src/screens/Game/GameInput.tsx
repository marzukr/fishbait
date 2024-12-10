import React from 'react';
import { keyBy, get, map } from 'lodash';

import { useKey } from 'utils/effects';

import './GameInput.css';

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
  /**
   * The KeyboardEvent.code code that triggers the button for this label to be
   * pressed
   */
  keyCode: string;
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
  type ButtonRefType = { current: HTMLButtonElement | null };
  const buttonRefs: ButtonRefType[] = labels.map(() => ({ current: null }));

  const handlePress = (code: GameInputCode) => {
    if (disabled.indexOf(code) !== -1) return;
    handler(code);
  };

  const buttonDatas = labels.map((label, idx) => {
    const style = {
      gridColumn: label.cols,
      gridRow: label.rows,
    };
    const isBlack = label.class.indexOf('black') !== -1;
    const isDisabled = disabled.indexOf(label.code) !== -1;
    const enabledClassName = (
      'gameInputButton focus-visible:outline-none ' + label.class
    );
    const focusClassName = (
      !isDisabled
        ? (
          enabledClassName + `
            focus-visible:brightness-[0.85]
          `
        )
        : enabledClassName
    );
    const blackClassName = (
      isBlack && !isDisabled
        ? (
          focusClassName + `
            active:bg-neutral-700 focus-visible:bg-neutral-700
          `
        )
        : focusClassName
    );
    const className = (
      disabled.indexOf(label.code) !== -1
        ? blackClassName + ' disabled'
        : blackClassName
    );
    return {
      button: (
        <button
          className={className} id={String(label.code)} key={idx}
          onClick={_ => handlePress(label.code)} style={style}
          ref={buttonRefs[idx]}
        >
          {label.label}
        </button>
      ),
      keyCode: label.keyCode,
      idx: idx,
    }
  });
  const buttonDataByKeyCode = keyBy(buttonDatas, 'keyCode');
  const buttons = map(buttonDatas, 'button');

  const getButtonDomNode = (keyCode: string) => {
    const buttonData = get(buttonDataByKeyCode, keyCode, undefined);
    if (buttonData === undefined) return;
    const buttonRef = get(buttonRefs, buttonData.idx, undefined);
    return buttonRef?.current;
  }
  const keyPressFn = (keyCode: string) => {
    const buttonDomNode = getButtonDomNode(keyCode);
    if (!buttonDomNode) return;
    buttonDomNode.focus();
    buttonDomNode.click();
  };
  const keyUpFn = (keyCode: string) => {
    const buttonDomNode = getButtonDomNode(keyCode);
    if (!buttonDomNode) return;
    buttonDomNode.blur();
  }
  useKey(keyPressFn, keyUpFn);

  return (
    <div className={`
      gameInput max-w-sm fixed bg-white mx-auto z-10 h-52 pb-4
    `}>
      {buttons}
    </div>
  );
}

export const cardInputLabels: GameInputLabel[] = [
  { label: 'A', class: 'nums', code: 'A', keyCode: 'KeyA' },
  { label: 'K', class: 'nums', code: 'K', keyCode: 'KeyK' },
  { label: 'Q', class: 'nums', code: 'Q', keyCode: 'KeyQ' },
  { label: 'J', class: 'nums', code: 'J', keyCode: 'KeyJ' },
  { label: 'T', class: 'nums', code: 'T', keyCode: 'KeyT' },
  { label: <i className='fas fa-chevron-right'/>, class: 'next',
    code: 'next', keyCode: 'Enter' },
  { label: '9', class: 'nums', code: '9', keyCode: 'Digit9' },
  { label: '8', class: 'nums', code: '8', keyCode: 'Digit8' },
  { label: '7', class: 'nums', code: '7', keyCode: 'Digit7' },
  { label: '6', class: 'nums', code: '6', keyCode: 'Digit6' },
  { label: '♦', class: 'red', code: 'd', keyCode: 'KeyD' },
  { label: '♣', class: 'black', code: 'c', keyCode: 'KeyC' },
  { label: '5', class: 'nums', code: '5', keyCode: 'Digit5' },
  { label: '4', class: 'nums', code: '4', keyCode: 'Digit4' },
  { label: '3', class: 'nums', code: '3', keyCode: 'Digit3' },
  { label: '2', class: 'nums', code: '2', keyCode: 'Digit2' },
  { label: '♥', class: 'red', code: 'h', keyCode: 'KeyH' },
  { label: '♠', class: 'black', code: 's', keyCode: 'KeyS' }
];

export const makeBetLabels = (canCheck: boolean): GameInputLabel[] => [
  { label: '9', class: 'nums', code: 9, keyCode: 'Digit9' },
  { label: '8', class: 'nums', code: 8, keyCode: 'Digit8' },
  { label: '7', class: 'nums', code: 7, keyCode: 'Digit7' },
  { label: <i className='fas fa-backspace' style={{fontSize: '85%'}}></i>,
    class: 'black', code: 'delete', cols: '4 / 5', rows: '1 / 3',
    keyCode: 'Backspace' },
  { label: 'fold', class: 'black', code: 'fold', cols: '5 / 7',
    keyCode: 'KeyF' },
  { label: '6', class: 'nums', code: 6, keyCode: 'Digit6' },
  { label: '5', class: 'nums', code: 5, keyCode: 'Digit5' },
  { label: '4', class: 'nums', code: 4, keyCode: 'Digit4' },
  { label: (canCheck ? 'check' : 'call'), class: 'black', code: 'checkcall',
    cols: '5 / 7', keyCode: 'KeyC' },
  { label: '3', class: 'nums', code: 3, keyCode: 'Digit3' },
  { label: '2', class: 'nums', code: 2, keyCode: 'Digit2' },
  { label: '1', class: 'nums', code: 1, keyCode: 'Digit1' },
  { label: '0', class: 'nums', code: 0, keyCode: 'Digit0' },
  { label: <i className='fas fa-exclamation-triangle'></i>,
    class: 'black', code: 'allin', keyCode: 'KeyA' },
  { label: <i className='fas fa-chevron-right'/>, class: 'next',
    code: 'next', keyCode: 'Enter' }
];

export const newHandLabels: GameInputLabel[] = [
  { label: 'New Hand', class: 'nums', code: 'new hand', cols: '2 / 6',
    rows: '2 / 3', keyCode: 'Enter' }
];
