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
          <p style={{ textAlign: 'justify' }}>
            Aw, snap! We've got a lot of people visiting us, so we don't have
            room for you right now.
          </p>
          
          <p style={{ textAlign: 'justify' }}>
            We're sorry about that! You might have better luck in a few hours.
            We're working hard to build more space for you.
          </p>

          <p style={{ textAlign: 'justify' }}>
            In the meantime, if you give us your email, we can let you
            know when we've got more capacity:
          </p>
          <form className='inputButton' onSubmit={this.handleSubmit}>
            <input type='email' value={this.state.value} placeholder='mail@example.com'
                   onChange={this.handleChange}/>
            <button type='submit'><i className='fas fa-chevron-right'/></button>
          </form>
        </div>
      </div>
    );
  }  // render()
}  // class Login

export default Login;
