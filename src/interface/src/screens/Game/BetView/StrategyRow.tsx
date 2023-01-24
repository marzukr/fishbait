import React, { useState } from 'react';

import { Action, GameState, AvailableAction } from 'utils/api';

/** Number of significant figures to display bet sizes */
const ACTION_SIZE_PRECISION = 2;
const ACTION_POLICY_PRECISION = 3;

interface StrategyRowProps {
  /** The current game state */
  gameState: GameState;
}

/** This component displays Fishbait's mixed strategy */
export const StrategyRow: React.FC<StrategyRowProps> = ({
  gameState,
}) => {
  /** This state stores the last non null available actions */
  const [availableActions, setAvailableActions] = (
    useState<AvailableAction[]>([])
  );
  const fishbaitAction = gameState.lastAction[gameState.fishbaitSeat];
  const [fishbaitActionIdx, setFishbaitActionIdx] = (
    useState(fishbaitAction?.actionIdx)
  );

  /*
   * This logic updates availableActions state if gameState.availableActions has
   * changed since the last render and it isn't null. If we update the state,
   * then this triggers a re-render of the component with the new state as soon
   * as we return. For this reason, we return early so we can skip straight to
   * that re-render.
   */
  if (
    gameState.availableActions !== null
    && availableActions !== gameState.availableActions
  ) {
    setAvailableActions(gameState.availableActions);
    setFishbaitActionIdx(fishbaitAction?.actionIdx);
    return null;
  }

  /*
   * If gameState.availableActions is null, we want to hide the component but
   * still show the actions from the last non-null gameState.availableActions so
   * that as the hide animation is occuring, we don't see a blank chart. 
   */
  const shouldShow = gameState.availableActions !== null;

  // Here we create a local copy of the availableActions so we can sort it
  const actions = [...(availableActions)];
  const actionToSortVal = {
    [Action.FOLD]: 0,
    [Action.CHECK]: 1,
    [Action.CALL]: 2,
    [Action.BET]: 3,
    [Action.ALL_IN]: 4,
  };
  actions.sort((a, b) => {
    if (a.action === Action.BET && b.action === Action.BET) {
      return a.size - b.size;
    }
    return actionToSortVal[a.action] - actionToSortVal[b.action];
  });

  const labels = actions.map((action, idx) => {
    const roundedSize = action.size.toPrecision(ACTION_SIZE_PRECISION);
    const label = action.action === Action.BET ? roundedSize : action.action;
    return (
      <div className='text-right h-6' key={idx}>{label}</div>
    );
  });
  const bars = actions.map((action, idx) => {
    const actionPercent = action.policy * 100;
    const roundedString = (
      actionPercent >= 1
        ? actionPercent.toPrecision(ACTION_POLICY_PRECISION)
        : (Math.round(actionPercent * 100) / 100)
          .toFixed(ACTION_POLICY_PRECISION - 1)
    );
    const roundedNumber = Number(roundedString);
    const roundedTo100 =  roundedNumber === 100 && actionPercent !== 100;
    const roundedTo0 = roundedNumber === 0 && actionPercent !== 0;
    const percentText = (
      roundedTo100
        ? '>99.9%'
        : roundedTo0
          ? '<0.01%'
          : `${roundedString}%`
    );
    const barClass = (
      fishbaitActionIdx === action.actionIdx ? ' opacity-40' : 'opacity-10'
    );
    return (
      <div className={`flex h-6 ${idx !== 0 ? 'border-t' : ''}`} key={idx}>
        <div
          className='h-full shrink-0 py-1'
          style={{
            flexBasis: `max(3px, ${percentText})`
          }}
        >
          <div className='h-full w-full border-y border-r border-sky-500'>
            <div className={`h-full w-full bg-sky-500 ${barClass}`}/>
          </div>
        </div>
        <div className='pl-1'>{percentText}</div>
      </div>
    );
  });
  const gridColumns = Array(6).fill(null).map((_, idx) => {
    return <div key={idx} className={`${idx !== 0 ? 'border-l' : ''}`}/>
  });
  const endRowColumns = actions.map((_, idx) => {
    return <div key={idx} className={`${idx !== 0 ? 'border-t' : ''} h-6`}/>
  })
  return (
    <div
      className={`
        ${shouldShow && 'px-3 py-2.5'} overflow-hidden flex bg-slate-50
        shadow-[inset_0_1px_4px_0_rgba(0,0,0,0.1)] box-content
        ${gameState.folded[gameState.fishbaitSeat] ? 'opacity-50' : ''}
      `}
      style={{
        height: shouldShow ? `${actions.length * 24}px` : 0,
        transitionProperty: 'height, padding-top, padding-bottom',
        transitionTimingFunction: 'ease-in-out',
        transitionDuration: '300ms',
      }}
    >
      <div className='border-r pr-1 pl-[5px] border-black'>{labels}</div>
      <div className='grow flex relative'>
        <div className='grow overflow-y-clip overflow-x-visible z-10'>
          {bars}
        </div>
        <div className='absolute w-full h-full grid grid-cols-6'>
          {gridColumns}
        </div>
        <div className='w-14'>
          {endRowColumns}
        </div>
      </div>
    </div>
  );
}
