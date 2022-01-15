// Copyright 2021 Marzuk Rashid

import React from 'react';
import Login from 'screens/Login';
import Game from 'screens/Game';
import Settings from 'screens/Settings';
import { isoToAsciiString } from 'utils/hands';

/*
  Routes the user to the proper screen in our app.

  state:
    gameState: The latest game state json string obtained from the flask
        backend.
    apiKey: The API key to use to access the flask backend.
    settings: If we are currently on the settings page.
*/
class App extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      gameState: null,
      apiKey: sessionStorage.getItem('apiKey'),
      settings: false
    };
    this.updateKey = this.updateKey.bind(this);
    this.reponseCallback = this.reponseCallback.bind(this);
    this.apiCall = this.apiCall.bind(this);
    this.toggleSettings = this.toggleSettings.bind(this);
  }

  componentDidMount() {
    if (this.state.apiKey !== null) this.loadStatus();
  }

  /*
    Sets this.state.apiKey to the given key if it is a valid key.
  */
  updateKey(newKey) {
    sessionStorage.setItem('apiKey', newKey);
    this.setState({apiKey: newKey}, () => {
      this.loadStatus();
    });
  }

  reponseCallback(response) {
    if (response.status !== 200) {
      sessionStorage.removeItem('apiKey');
      this.setState({gameState: null, apiKey: null});
    } else {
      response.json().then(gameState => {
        gameState['hands'] = gameState['hands'].map(h => {
          if (h === null) return h;
          if (h[0] === null && h[1] === null) return h;
          return h.map(c => isoToAsciiString[c]);
        });
        gameState['board'] = gameState['board'].map(c => {
          if (c === null) return c;
          return isoToAsciiString[c];
        });
        this.setState({gameState: gameState});
      });
    }
  }

  /*
    Attempts to load the game state from the flask backend with the current API
    key. Sets the key and game state to null if it fails.
  */
  loadStatus() {
    fetch('/api/state', {
      headers: {
        'Authorization': 'Bearer: ' + this.state.apiKey
      }
    }).then(this.reponseCallback);
  }

  async apiCall(route, obj) {
    return fetch('/api/' + route, {
      method: 'POST',
      headers: {
        'Authorization': 'Bearer: ' + this.state.apiKey,
        'Content-Type': 'application/json'
      },
      body: JSON.stringify(obj)
    }).then(this.reponseCallback);
  }

  toggleSettings() {
    this.setState({ settings: !this.state.settings });
  }

  render() {
    if (this.state.gameState === null) {
      return (
        <Login updateKey={this.updateKey}/>
      );
    } else if (this.state.settings) {
      let origStack = this.state.gameState.stack.slice();
      for (let i = 0; i < this.state.gameState.players; ++i) {
        origStack[i] += this.state.gameState.bets[i];
      }
      return <Settings players={this.state.gameState.players}
                       toggleSettings={this.toggleSettings}
                       smallBlind={this.state.gameState.small_blind}
                       bigBlind={this.state.gameState.big_blind}
                       playerNames={this.state.gameState.player_names}
                       fishbaitSeat={this.state.gameState.fishbait_seat}
                       button={this.state.gameState.button} stack={origStack}
                       apiCall={this.apiCall}/>
    } else {
      return (
        <Game gameState={this.state.gameState} apiCall={this.apiCall}
              toggleSettings={this.toggleSettings}/>
      );
    }
  }
}

export default App;
