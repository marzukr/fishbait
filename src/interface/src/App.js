// Copyright 2021 Marzuk Rashid

import React from 'react';
import Game from 'screens/Game';
import Settings from 'screens/Settings';
import { isoToAsciiString } from 'utils/hands';

/*
  Routes the user to the proper screen in our app.

  state:
    gameState: The latest game state json string obtained from the flask
        backend.
    sessionId: The session id of our api session.
    settings: If we are currently on the settings page.
    makingRequest: If we are currently querying the api.
    apiIsUp: If the API is up.
    isLoading: If we should display a loading screen.

  Intended behavior:
    - Create and display a new session:
      - new visit, server up, capacity available, no session
      - new visit, server up, capacity available, session invalid
    - Display the user's previous session:
      - new visit, server up, capacity available, session valid
      - new visit, server up, capacity unavailable, session valid
    - Change game state and render new state:
      - post, server up, capacity available, session valid
      - post, server up, capacity unavailable, session valid
    - Display a capacity error message with email sign up:
      - new visit, server up, capacity unavailable, no session
      - new visit, server up, capacity unavailable, session invalid
    - Display oops, you stepped away for too long so we gave slot away error:
      - post, server up, capacity available, session invalid
      - post, server up, capacity unavailable, session invalid
    - Display a server down error message with contact info:
      - new visit, server down, no session
      - new visit, server down, session invalid
      - new visit, server down, session valid
      - post, server down, session invalid
      - post, server down, session valid
    - impossible:
      - post, server up, capacity available, no session
      - post, server up, capacity unavailable, no session
      - post, server down, no session
    - don't display anything:
      - if we are still loading on a new visit
*/
class App extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      gameState: null,
      sessionId: sessionStorage.getItem('sessionId'),
      settings: false,
      makingRequest: false,
      apiIsUp: true,
      isLoading: true,
    };
    this.setStatePromise = this.setStatePromise.bind(this);
    this.getNewSessionId = this.getNewSessionId.bind(this);
    this.reponseCallback = this.reponseCallback.bind(this);
    this.safeRequest = this.safeRequest.bind(this);
    this.getStatus = this.getStatus.bind(this);
    this.apiCall = this.apiCall.bind(this);
    this.toggleSettings = this.toggleSettings.bind(this);
  }

  componentDidMount() {
    let stopLoading = () => {
      this.setState({isLoading: false});
    };
    if (this.state.sessionId === null) {
      this.getNewSessionId().then(this.getStatus).then(stopLoading);
    } else {
      this.getStatus().then(() => {
        if (this.state.sessionId !== null) return stopLoading();
        this.getNewSessionId().then(this.getStatus).then(stopLoading);
      });
    }
  }

  setStatePromise(newState) {
    return new Promise((resolve, _) => {
      this.setState(newState, resolve);
    });
  }

  getNewSessionId() {
    let newSessionHandler = (response) => {
      if (response.status !== 200) return;
      return response.json().then(responseJson => {
        if (!('new_session_id' in responseJson)) return;
        let newSessionId = responseJson['new_session_id'];
        sessionStorage.setItem('sessionId', newSessionId);
        return this.setStatePromise({ sessionId: newSessionId });
      });
    };
    let request = () => (
      fetch(`/api/new_session`).then(newSessionHandler)
    );
    return this.safeRequest(request);
  }

  reponseCallback(response) {
    if (response.status === 200) {
      return response.json().then(gameState => {
        gameState['hands'] = gameState['hands'].map(h => {
          if (h === null) return h;
          if (h[0] === null && h[1] === null) return h;
          return h.map(c => isoToAsciiString[c]);
        });
        gameState['board'] = gameState['board'].map(c => {
          if (c === null) return c;
          return isoToAsciiString[c];
        });
        return this.setStatePromise({ gameState: gameState });
      });
    } else {
      sessionStorage.removeItem('sessionId');
      let newState = { sessionId: null };
      let serverDownHandler = () => {
        newState.apiIsUp = false;
        return this.setStatePromise(newState);
      };
      return response.json().then((responseJson) => {
        if (!('forseen_error_code' in responseJson)) return serverDownHandler();
        return this.setStatePromise(newState);
      }, serverDownHandler);
    }
  }

  safeRequest(request) {
    if (this.state.makingRequest) {
      return Promise.reject(new Error('Already making an api request.'));
    }
    return this.setStatePromise({ makingRequest: true }).then(request)
        .then(() => this.setStatePromise({ makingRequest: false }));
  }

  /*
    Attempts to load the game state from the flask backend with the current
    session id. Sets the session id and game state to null if it fails.
  */
  getStatus() {
    let request = () => (
      fetch(`/api/state?session_id=${this.state.sessionId}`)
          .then(this.reponseCallback)
    );
    return this.safeRequest(request);
  }

  async apiCall(route, obj) {
    let request = () => (
      fetch(`/api/${route}?session_id=${this.state.sessionId}`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json'
        },
        body: JSON.stringify(obj)
      }).then(this.reponseCallback)
    );
    return this.safeRequest(request);
  }

  toggleSettings() {
    this.setState({ settings: !this.state.settings });
  }

  render() {
    if (this.state.isLoading) {
      return null;
    } else if (!this.state.apiIsUp) {
      return "server down error message with contact info";
    } else if (this.state.gameState === null) {
      return "capacity error message with email sign up";
    } else if (this.state.sessionId === null) {
      return "oops, you stepped away for too long so we gave slot away error";
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
