import React from 'react';
import './Login.css';

class Login extends React.Component {
  constructor(props) {
    super(props);
    this.state = {value: ''};
    this.handleChange = this.handleChange.bind(this);
    this.handleSubmit = this.handleSubmit.bind(this);
  }

  handleChange(event) {
    this.setState({value: event.target.value});
  }

  handleSubmit(event) {
    this.props.updateKey(this.state.value);
    event.preventDefault();
  }
  
  render() {
    return (
      <div className="boxSpace">
        <div className="passwordGroup">
          <p className="passwordLabel">Password:</p>
          <form className="inputButton" onSubmit={this.handleSubmit}>
            <input type="password" value={this.state.value}
                   onChange={this.handleChange}/>
            <button type="submit"><i className="fas fa-chevron-right"/></button>
          </form>
        </div>
      </div>
    );
  }
}

export default Login;
