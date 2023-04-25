import React, { useState } from 'react';

import { Api, useSafeAsync } from 'utils/api';
import { DiscordButton } from 'components/DiscordButton';
import { Link } from 'components/Link';

import './Settings.css';

interface SettingsProps {
  api: Api;
  /** Callback to toggle the settings page */
  toggleSettings: () => void;
}

/** This screen allows the user to reset the game with new settings. */
export const Settings: React.FC<SettingsProps> = ({ api, toggleSettings }) => {
  const gameState = api.gameState;

  const [smallBlind, setSmallBlind] = useState(
    gameState?.smallBlind.toString() || ""
  );
  const [bigBlind, setBigBlind] = useState(
    gameState?.bigBlind.toString() || ""
  );
  const [playerNames, setPlayerNames] = useState(gameState?.playerNames || []);
  const [stack, setStack] = useState(
    gameState?.stack.map((s, idx) => `${s + gameState.bets[idx]}`) || []
  );
  const [button, setButton] = useState(gameState?.button || 0);
  const [fishbaitSeat, setFishbaitSeat] = useState(
    gameState?.fishbaitSeat || 0
  );

  const safeAsync = useSafeAsync();

  if (gameState === null) return null;

  const handleChange = (
    stateModifier: (newVal: string) => void,
    event: React.ChangeEvent<HTMLInputElement>
  ) => {
    stateModifier(event.target.value);
  };

  const handleChangeArray = (
    stateModifier: (fn: (p: string[]) => string[]) => void,
    idx: number,
    event: React.ChangeEvent<HTMLInputElement>
  ) => {
    stateModifier((p) => {
      const newState = p.slice();
      newState[idx] = event.target.value;
      return newState;
    });
  };

  const moveFishbait = (positions: number) => {
    const newSeat = (
      (fishbaitSeat + gameState.players + positions) % gameState.players
    );
    setFishbaitSeat(newSeat);
  };

  const dealerClick = (e: React.MouseEvent<Element, MouseEvent>) => {
    setButton(Number(e.currentTarget.id.slice(6)));
  };

  const saveClick = () => {
    safeAsync(api.reset({
      stack: stack.map(s => Number(s)),
      button,
      fishbaitSeat,
      playerNames,
      bigBlind: Number(bigBlind),
      smallBlind: Number(smallBlind),
    })).then(() => {
      toggleSettings();
    });
  };

  const tableRows = [];
  for (let i = 0; i < gameState.players; ++i) {
    const playerName = i === fishbaitSeat
      ? <div className='fishbaitNameSetting'>
          <div>Fishbait 🐟</div>
          <div className='moveFishbaitButtons'>
            <button
              onClick={() => moveFishbait(-1)} className="flex content-center"
            >
              <i className="fas fa-long-arrow-alt-up leading-4"></i>
            </button>
            <button
              onClick={() => moveFishbait(1)} className="flex content-center"
            >
              <i className="fas fa-long-arrow-alt-down leading-4"></i>
            </button>
          </div>
        </div>
      : <input
          type='text' className='settingsInput'
          value={playerNames[i]}
          onChange={e => handleChangeArray(setPlayerNames, i, e)}
          id={`player${i}`} />;
    const idBoxClass = i === button
      ? 'settingsPlayerIdBox dealer'
      : 'settingsPlayerIdBox';
    tableRows.push(
      <div className='settingsTableRow' key={i}>
        <div className={idBoxClass} id={`button${i}`} onClick={dealerClick}>
          <div className='settingsDealerButton'></div>
          <div className='settingsIdLabel'>{i + 1}.</div>
        </div>
        {playerName}
        <input
          type='text' className='settingsInput' value={stack[i]}
          pattern='[0-9]*' onChange={e => handleChangeArray(setStack, i, e)} />
      </div>
    );
  }  // for players
  const strIsInt = (s: string) => Number.isInteger(Number(s));
  const arrIsValid = (a: string[], p: (s: string) => boolean) => a.every(p);
  const isValid = (
    strIsInt(smallBlind) && strIsInt(bigBlind) && arrIsValid(stack, strIsInt)
  );
  const saveButtonClass = isValid ? 'saveButton' : 'saveButton disabled';

  return (
    <div className='flex flex-col h-full fixed w-screen'>
      <div className='header max-w-sm fixed bg-white mx-auto h-12 z-10'>
        <button className='headerButton' onClick={toggleSettings}>
          <i className='fas fa-chevron-left'></i>
        </button>
        <p>Settings</p>
        <div className='headerPad'></div>
      </div>
      <div className='shrink-0 h-12 max-w-sm'/>
      <div className={`
        flex flex-col items-center overflow-y-auto overflow-x-hidden px-2.5
        mx-auto max-w-sm
      `}>
        <div className='sectionHeader'>Game Attributes</div>
        <div className='shrink-0 settingsTable blind'>
          <div className='settingsTableRow headerCol1'>
            <div>Small Blind:</div>
            <input
              type='text' className='settingsInput' value={smallBlind}
              pattern='[0-9]*' onChange={e => handleChange(setSmallBlind, e)}
            />
          </div>
          <div className='settingsTableRow headerCol1'>
            <div>Big Blind:</div>
            <input
              type='text' className='settingsInput' value={bigBlind}
              pattern='[0-9]*' onChange={e => handleChange(setBigBlind, e)}
            />
          </div>
        </div>
        <div className='sectionHeader'>Players</div>
        <div className='shrink-0 settingsTable players'>
          <div className='settingsTableRow headerRow'>
            <div className='settingsPlayerIdBox'><div>#</div></div>
            <div>Name</div>
            <div>Stack</div>
          </div>
          {tableRows}
        </div>
        <button
          className={saveButtonClass} onClick={saveClick} disabled={!isValid}
        >
          Save
        </button>
        <div className='border-t-2 pt-3.5 mt-7 text-center w-full'>About</div>
        <p className='shrink-0 text-justify mt-3'>
          Fishbait is an{' '}
          <Link
            url='https://github.com/marzukr/fishbait/'
            displayText='open source'
          /> AI that
          plays poker. It has a unique strategy for over 600 million different
          situations.
        </p>
        <p className='shrink-0 text-justify mt-3'>
          Fishbait is under active development. Our mission is to make the
          world's best AIs available to everyone, starting with poker.
        </p>  
        <p className='shrink-0 text-justify mt-3 w-full'>
          Upcoming features include:
          <ul className='list-disc pl-6'>
            <li>Real time strategy fine-tuning</li>
            <li>Support for different table sizes</li>
            <li>Measure your performance vs. the AI</li>
            <li>Share a hand play-through with a link</li>
          </ul>
        </p>
        <p className='shrink-0 text-justify mt-3 pb-12'>
          If you want a question answered, or want a say in future development,
          join the Fishbait <DiscordButton/> server. You can also email{' '}
          <Link
            url='mailto:fishbait@marzuk.io'
            displayText='fishbait@marzuk.io'
          />
        </p>
      </div>
    </div>
  );  // return
}
