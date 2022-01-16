// Copyright 2021 Marzuk Rashid

import React from 'react';
import 'screens/Settings.css';

/*
  This screen allows the user to rest the game with new settings.

  props:
    toggleSettings: Callback to close the settings page.
    players: How many players there are in the game.
    smallBlind: What the current small blind of the game is.
    bigBlind: What the current big blind of the game is.
    playerNames: Array of names for each player.
    fishbaitSeat: Fishbait's player id.
    button: Player id of the button.
    stack: Array stack sizes for each player.
    apiCall: Callback to make an api call.

  state:
    smallBlind: What the current small blind of the game is.
    bigBlind: What the current big blind of the game is.
    playerNames: Array of names for each player.
    stack: Array stack sizes for each player.
    button: Player id of the button.
    fishbaitSeat: Fishbait's player id.
*/
class Settings extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      smallBlind: `${props.smallBlind}`,
      bigBlind: `${props.bigBlind}`,
      playerNames: props.playerNames.slice(),
      stack: props.stack.map(s => `${s}`),
      button: props.button,
      fishbaitSeat: props.fishbaitSeat,
    };
    this.dealerClick = this.dealerClick.bind(this);
    this.saveClick = this.saveClick.bind(this);
  }

  handleChange(stateName, event) {
    let newState = {};
    newState[stateName] = event.target.value;
    this.setState(newState);
  }

  handleChangeArray(stateName, idx, event) {
    let newState = {};
    newState[stateName] = this.state[stateName];
    newState[stateName][idx] = event.target.value;
    this.setState(newState);
  }

  moveFishbait(positions) {
    let newSeat = (this.state.fishbaitSeat + this.props.players + positions)
        % this.props.players;
    this.setState({ fishbaitSeat: newSeat });
  }

  dealerClick(e) {
    this.setState({ button: Number(e.target.id.slice(6)) });
  }

  saveClick() {
    let obj = {
      stack: this.state.stack.map(s => Number(s)),
      button: this.state.button,
      fishbait_seat: this.state.fishbaitSeat,
      player_names: this.state.playerNames,
      big_blind: Number(this.state.bigBlind),
      small_blind: Number(this.state.smallBlind)
    };
    this.props.apiCall('reset', obj).then(() => {
      this.props.toggleSettings();
    });
  }

  render() {
    let tableRows = [];
    for (let i = 0; i < this.props.players; ++i) {
      let playerName = i === this.state.fishbaitSeat
          ? <div className='fishbaitNameSetting'>
              <div>Fishbait 🐟</div>
              <div className='moveFishbaitButtons'>
                <button onClick={this.moveFishbait.bind(this, -1)}>
                  <i className="fas fa-long-arrow-alt-up"></i>
                </button>
                <button onClick={this.moveFishbait.bind(this, 1)}>
                  <i className="fas fa-long-arrow-alt-down"></i>
                </button>
              </div>
            </div>
          : <input type='text' className='settingsInput'
                value={this.state.playerNames[i]}
                onChange={this.handleChangeArray.bind(this, 'playerNames', i)}
                id={`player${i}`}>
            </input>;
      let idBoxClass = i === this.state.button
          ? 'settingsPlayerIdBox dealer'
          : 'settingsPlayerIdBox';
      tableRows.push(
        <div className='settingsTableRow' key={i}>
          <div className={idBoxClass} id={`button${i}`}
               onClick={this.dealerClick}>
            <div className='settingsDealerButton'></div>
            <div className='settingsIdLabel'>{i + 1}.</div>
          </div>
          {playerName}
          <input type='text' className='settingsInput'
                 value={this.state.stack[i]} pattern='[0-9]*'
                 onChange={this.handleChangeArray.bind(this, 'stack', i)}>
          </input>
        </div>
      );
    }  // for players
    let strIsInt = s => Number.isInteger(Number(s));
    let arrIsValid = (a, p) => {
      for (let i = 0; i < a.length; ++i) {
        if (!p(a[i])) return false;
      }
      return true;
    };
    let isValid = strIsInt(this.state.smallBlind) &&
        strIsInt(this.state.bigBlind) && arrIsValid(this.state.stack, strIsInt);
    let saveButtonClass = isValid ? 'saveButton' : 'saveButton disabled';
    return (
      <div className='boxSpace'>
        <div className='header'>
          <button className='headerButton'
                  onClick={this.props.toggleSettings}>
            <i className='fas fa-chevron-left'></i>
          </button>
          <p>Settings</p>
          <div className='headerPad'></div>
        </div>
        <div className='contentArea'>
          <div className='sectionHeader'>Game Attributes</div>
          <div className='settingsTable blind'>
            <div className='settingsTableRow headerCol1'>
              <div>Small Blind:</div>
              <input type='text' className='settingsInput'
                     value={this.state.smallBlind} pattern='[0-9]*'
                     onChange={this.handleChange.bind(this, 'smallBlind')}>
              </input>
            </div>
            <div className='settingsTableRow headerCol1'>
              <div>Big Blind:</div>
              <input type='text' className='settingsInput'
                     value={this.state.bigBlind} pattern='[0-9]*'
                     onChange={this.handleChange.bind(this, 'bigBlind')}>
              </input>
            </div>
          </div>
          <div className='sectionHeader'>Players</div>
          <div className='settingsTable players'>
            <div className='settingsTableRow headerRow'>
              <div className='settingsPlayerIdBox'><div>#</div></div>
              <div>Name</div>
              <div>Stack</div>
            </div>
            {tableRows}
          </div>
          <button className={saveButtonClass} onClick={this.saveClick}
                  disabled={!isValid}>
            Save
          </button>
        </div>
      </div>
    );  // return
  }  // render()
}  // class Settings

export default Settings;
