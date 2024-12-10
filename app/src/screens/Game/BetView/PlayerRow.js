import React from 'react';
import './PlayerRow.css';
import Card from '../Card';

/*
  This component is a single row in the BetView table. This represents the state
  of a single player.

  props:
    playerId: The id number of the player on this row.
    playerName: The name of the player on this row.
    isButton: If this player is on the button.
    isActingPlayer: If this player is the current acting player.
    bet: How much this player has bet.
    stack: How much this player has left in their stack.
    hand: This player's hand.
    folded: If this player is folded.
    action: The action to display on this player row. Either the last action of
        this player, or the action currently being entered by the user.
    modifying: The id of the card we are modifying if we are modifying this
        player's hand. Otherwise null.
    hideCards: Boolean. If this player's cards should be hidden.
*/
class PlayerRow extends React.Component {
  render() {
    let nameStr = this.props.action === null && !this.props.isActingPlayer
        ? this.props.playerName
        : this.props.playerName + ':';
    let cursorClass = this.props.isActingPlayer
        ? 'cursorClass active'
        : 'cursorClass';
    let actionString = (() => {
      if (this.props.action === null) {
        return '';
      } else if (this.props.action.action === 'Bet') {
        if (!('size' in this.props.action) || this.props.action.size === null) {
          return 'Bet';
        } else {
          return 'Bet ' + this.props.action.size;
        }
      } else {
        return this.props.action.action;
      }
    })();
    let generateCard = (id) => {
      let isModifying = this.props.modifying === id ? true : false;
      let card = this.props.hand === null ? null : this.props.hand[id];
      let key = `${id}${this.props.hideCards}`;
      return <Card card={card} isModifying={isModifying}
                   shouldHide={this.props.hideCards} key={key}/>
    };
    return (
      <div className={`
        ${this.props.playerId !== 0 ? 'border-t' : ''} border-neutral-200
        h-[72px]
      `}>
        <div className={`
          grid grid-cols-[35px_1fr_1fr_30%] grid-rows-2 px-1 h-full
          ${this.props.folded ? 'opacity-50' : ''}
        `}>
          <div className={`
            col-span-1 row-span-2 flex justify-center items-center text-xl
          `}>
            <div className={
              this.props.isButton ? `
                dealerButton w-[20pt] h-[20pt] bg-black rounded-2xl
                text-white text-center
              ` : ''
            }>
              {this.props.playerId + 1}.
            </div>
          </div>
          <div className='playerNameBox'>
            <div className='playerNameBoxName'>{nameStr}</div>
            <div className={cursorClass}>{actionString}</div>
          </div>
          <div className={`
            playerBetBox col-start-2 col-end-3 flex gap-1.5 pl-1.5 pt-1
          `}>
            <i className='fas fa-dice'/>
            <span className='leading-5'>{this.props.bet}</span>
          </div>
          <div className='playerStackBox'>
            <i className='fas fa-coins'/>
            <span className='leading-5'>{this.props.stack}</span>
          </div>
          <div className='playerCardBox'>
            {generateCard(0)}
            {generateCard(1)}
          </div>
        </div>
      </div>
    );
  }  // render()
}  // class PlayerRow

export default PlayerRow;
