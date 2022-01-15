// Copyright 2021 Marzuk Rashid

import React from 'react';
import 'screens/Login.css';

/*
  This screen password protects the interface.

  props:
    updateKey: Function to call when the user has submitted a password. Takes
        one parameter which is the password the user entered.

  state:
    value: The value of the password entry text box.
*/
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
      <div className='boxSpace'>
        <div className='passwordGroup'>
          <p className='passwordLabel'>Password:</p>
          <form className='inputButton' onSubmit={this.handleSubmit}>
            <input type='password' value={this.state.value}
                   onChange={this.handleChange}/>
            <button type='submit'><i className='fas fa-chevron-right'/></button>
          </form>
        </div>
      </div>
    );
  }  // render()
}  // class Login

export default Login;
