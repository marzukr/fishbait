import React from 'react';
import Login from './Login';

class App extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      gameStatus: null,
      apiKey: sessionStorage.getItem("apiKey")
    };
    if (this.state.apiKey !== null) this.loadStatus();
    this.updateKey = this.updateKey.bind(this);
  }

  updateKey(newKey) {
    sessionStorage.setItem("apiKey", newKey);
    this.setState({apiKey: newKey}, () => {
      console.log(this.state);
      this.loadStatus();
    });
  }

  loadStatus() {
    this.setState({gameStatus: 'Loading...'});
    fetch('/api/game_status', {
      headers: {
        "Authorization": "Bearer: " + this.state.apiKey
      }
    }).then(response => {
      if (response.status !== 200) {
        sessionStorage.removeItem("apiKey");
        this.setState({gameStatus: null, apiKey: null});
      } else {
        response.text().then(statusString => {
          this.setState({gameStatus: statusString});
        });
      }
    });
  }

  render() {
    if (this.state.apiKey === null) {
      return (
        <Login updateKey={this.updateKey}/>
      );
    } else {
      return (
        <p>{this.state.gameStatus}</p>
      );
    }
  }
}

export default App;
