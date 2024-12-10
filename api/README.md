# 🐟 Fishbait API Guide
This guide will describe how to use Fishbait via its API. This is the same API
that is used by the web app at [fishbait.marzuk.io](http://fishbait.marzuk.io)

The API gives you access to a version of Fishbait that is hosted in the cloud
and that has been trained using the ["big" hyperparameters](/src/ai/mccfr/hyperparameters.h.big).


## Getting Started
The Fishbait API is hosted at [fishbait.marzuk.io/api](http://fishbait.marzuk.io/api).
If you navigate to this page, you should get a response that says `FISHBAIT API Running`.
This indicates that the API is up and ready to go.

To start using the Fishbait API, you’ll need a session ID. To get a session ID,
send a `GET` request to the `/new-session` endpoint (so this will be at
`fishbait.marzuk.io/api/new-session`).

If the request succeeds, you will get a blank response and HTTP status code
`200`. The response will also set the `session_id` cookie to your new session
id. This indicates you're good to go! You'll need to provide this cookie upon
subsequent requests.

All new sessions start with the default game data (10,000 chips per player,
100/200 blinds). If you wish to change this, you can [reset](#post-reset) your
game.


## Using Routes
Once you have a session id cookie, you can begin to use the [routes](#routes) to
interact with your game and the AI.

### Game State Object
If no errors are raised, all of the routes will return the state of the game
after the request is completed. The game state is represented by a JSON object
with the following properties (all properties will always be returned):
- **`players`**
  - **Type**: `integer`
  - **Description**: The number of players in the game
- **`big_blind`**
  - **Type**: `integer`
  - **Description**: The size of the big blind in chips
- **`small_blind`**
  - **Type**: `integer`
  - **Description**: The size of the small blind in chips
- **`ante`**
  - **Type**: `integer`
  - **Description**: The size of the ante in chips
- **`big_blind_ante`**
  - **Type**: `boolean`
  - **Description**: If the big blind pays the ante for all players
- **`blind_before_ante`**
  - **Type**: `boolean`
  - **Description**: If blinds should be posted before the ante
- **`rake`**
  - **Type**: `number`
  - **Description**: The size of the rake as a proportion of the pot
- **`rake_cap`**
  - **Type**: `integer`
  - **Description**: The maximum size of the rake. 0 if no maximum
- **`no_flop_no_drop`**
  - **Type**: `boolean`
  - **Description**: If hands that end pre-flop are not raked
- **`button`**
  - **Type**: `integer`
  - **Description**: Player id of the button
- **`in_progress`**
  - **Type**: `boolean`
  - **Description**: If the game is currently in progress
- **`round`**
  - **Type**: `string`
  - **Enum**: `Preflop`, `Flop`, `Turn`, `River`
  - **Description**: What round the game is currently in
- **`acting_player`**
  - **Type**: `integer`
  - **Description**: Player id of the acting player
- **`folded`**
  - **Type**: `[boolean]`
  - **Description**: If each player is folded or not
- **`players_left`**
  - **Type**: `integer`
  - **Description**: How many players have not folded yet
- **`players_all_in`**
  - **Type**: `integer`
  - **Description**: How many players are all in
- **`pot`**
  - **Type**: `integer`
  - **Description**: The total size of the pot
- **`bets`**
  - **Type**: `[integer]`
  - **Description**: How much each player has contributed to the pot
- **`stack`**
  - **Type**: `[integer]`
  - **Description**: The size of each player's stack
- **`min_raise`**
  - **Type**: `integer`
  - **Description**: The minimum legal raise size for the acting player
- **`needed_to_call`**
  - **Type**: `integer`
  - **Description**: The amount the current player needs to call
- **`hands`**
  - **Type**: `[null or [integer or null]]`
  - **Description**: Each player's hand. Null if unknown, or a list filled with
    null if the player has mucked. Numbered as described in the [hand numbering section](#hand-numbering--card-indexing)
- **`board`**
  - **Type**: `[integer or null]`
  - **Description**: The cards on the public board. Null if unknown. Numbered
    as described in the [hand numbering section](#hand-numbering--card-indexing)
- **`fishbait_seat`**
  - **Type**: `integer`
  - **Description**: Which player number fishbait is
- **`player_needs_hand`**
  - **Type**: `integer or null`
  - **Description**: Which player, if any, we need hand info for
- **`board_needs_cards`**
  - **Type**: `object or null`
  - **Description**: The range of board cards for which we need info
  - **Properties**:
      - **`start`**
        - **Type**: `integer`
        - **Description**: The first index of `board` we need a card for (inclusive)
      - **`end`**
        - **Type**: `integer`
        - **Description**: The last index of `board` we need a card for (not inclusive)
- **`can_muck`**
  - **Type**: `boolean`
  - **Description**: If the player we need hand info for can muck
- **`player_names`**
  - **Type**: `[string]`
  - **Description**: Names of each player
- **`known_cards`**
  - **Type**: `[boolean]`
  - **Description**: If each player's hand is known or not
- **`last_action`**
  - **Type**: `[object or null]`
  - **Description**: What each player's last action was
  - **Properties**:
    - **`action`**
      - **Type**: `string`
      - **Enum**: `Fold`, `Check`, `Call`, `Bet`, `All In`
    - **`size`**
      - **Type**: `integer`
      - **Description**: If the `action` is `Bet`, how many chips the player bet.
    - **`action_idx`**
      - **Type**: `integer or null`
      - **Description**: If the player is Fishbait, the `available_actions` index of its action. Otherwise null.
- **`known_board`**
  - **Type**: `array` of `boolean`
  - **Description**: If each board card is known or not
- **`available_actions`**
  - **Type**: `[object or null]`
  - **Description**: The actions available to fishbait during its last turn to act
  - **Properties**:
    - **`action`**
      - **Type**: `string`
      - **Enum**: `Fold`, `Check`, `Call`, `Bet`, `All In`
    - **`size`**
      - **Type**: `number`
      - **Description**: If the `action` is `Bet`, the size of the bet as a proportion of the pot.
    - **`policy`**
      - **Type**: `number`
      - **Description**: The probability Fishbait would take this action
    - **`action_idx`**
      - **Type**: `integer`
      - **Description**: The index number of this action

### Hand Numbering / Card Indexing
The following numbering scheme is used anywhere the API uses an integer to
represent a card in the 52 card deck (for ex. the `hands` or `boards` properties
in the [game state object](#game-state-object)):
- **`0`** - `2♠`
- **`1`** - `2♥`
- **`2`** - `2♦`
- **`3`** - `2♣`
- **`4`** - `3♠`
- **`5`** - `3♥`
- **`6`** - `3♦`
- **`7`** - `3♣`
- **`8`** - `4♠`
- ...
- **`12`** - `5♠`
- ...
- **`32`** - `T♠`
- ...
- **`36`** - `J♠`
- ...
- **`48`** - `A♠`
- ...
- **`51`** - `A♣`


## Routes

### GET `/state`
- **Description**: Fetch the current state of the game.
- **Returns**: The current [game state object](#game-state-object).

### POST `/set-hand`
- **Description**: Sets the hand of `player_needs_hand`
- **Returns**: The [game state object](#game-state-object) after the hand has
  been set.
- **Expected Body**: JSON
  - **`hand`**
    - **Type**: `[int or null]`
    - **Description**: An array of 2 [hand numbers](#hand-numbering--card-indexing)
      that represent the hand of the `player_needs_hand`. Or an array of 2 nulls
      if `player_needs_hand` is mucking their hand.

### POST `/apply`
- **Description**: Plays the given action for `acting_player`
- **Returns**: The [game state object](#game-state-object) after the action has
  been played.
- **Expected Body**: JSON
  - **`action`**
    - **Type**: `string`
    - **Enum**: `Fold`, `Check`, `Call`, `Bet`, `All In`
  - **`size`**
    - **Type**: `integer or null`
    - **Description**: The amount of chips that is being bet if the `action` is
      `Bet`. Otherwise any arbitrary value or `null` is fine.

### POST `/set-board`
- **Description**: Sets the `board` to the given cards.
- **Returns**: The [game state object](#game-state-object) after the board has
  been set.
- **Expected Body**: JSON
  - **`board`**
    - **Type**: `[int or null]`
    - **Description**: An array of 5 [hand numbers](#hand-numbering--card-indexing)
      that represent the cards on the `board`. Board cards that have yet to be
      revealed can be set to `null`.

      Note that Fishbait will only consider cards legally available during the
      current round when making its decision—i.e. if you define all 5 board
      cards on the flop, Fishbait will only use the first 3 to calculate its
      strategy. So it is safe to define all 5 board cards at the beginning of
      the game if you'd like.

### POST `/new-hand`
- **Description**: At the end of a hand, the winner(s) are automatically awarded
  the appropriate reward from the pot and the hand concludes. This is indicated
  by `pot=0` and `in_progress=false`.

  You can call this route at that point to set up the game for the next hand.
  The cards will be cleared, the button will be rotated one position, and blinds
  will be posted.

  **NOTE**: If you call this endpoint in the middle of a hand before the pot is
  awarded, behavior is undefined. You will likely need to [reset](#post-reset)
  your game.
- **Returns**: The [game state object](#game-state-object) at the start of the
  new hand.

### POST `/reset`
- **Description**: Hard reset of the game state. Wipes all hands, cards, bets
  and actions and starts a new game from the given parameters.
- **Returns**: The [game state object](#game-state-object) at the start of the
  new game.
- **Expected Body**: JSON
  - **`stack`**
    - **Type**: `[int]`
    - **Description**: An array of stack sizes for each of the 6 players.
  - **`button`**
    - **Type**: `int`
    - **Description**: The index of the player on the button.
  - **`big_blind`**
    - **Type**: `int`
    - **Description**: The size of the big blind in the new game.
  - **`small_blind`**
    - **Type**: `int`
    - **Description**: The size of the small blind in the new game.
  - **`fishbait_seat`**
    - **Type**: `int`
    - **Description**: The player index of Fishbait.
  - **`player_names`**
    - **Type**: `[str]`
    - **Description**: The name of each player in the game.


## Troubleshooting

### ServerOverloadedError
When requesting a session, if the server is at capacity, you will get the
following JSON response along with status code `503`:
```json
{
  "error_message": "Our servers are currently overloaded because we are in beta and our capacity is limited. Please try again in 30 minutes.",
  "error_code": "ServerOverloadedError"
}
```

If this happens, try again later when the server may have free capacity.
Fishbait currently supports sessions of at least 30 minutes for up to 1000 users
at a time. This can be scaled up if needed.

### MissingSessionIdError
If you fail to provide a session id cookie on a route that requires it, you'll
get the following JSON response along with status code `400`:
```json
{
  "error_message": "A session ID was not provided.",
  "error_code": "MissingSessionIdError"
}
```

### UnknownSessionIdError
If the server is at capacity, your session may be deleted after 30 minutes to
make space for other users. If this happens, your requests will be responded to
with the following JSON along with status code `404`:
```json
{
  "error_message": "The provided session id was not found.",
  "error_code": "UnknownSessionIdError"
}
```

In this case, try requesting a new session id. Note that if your session is
deleted, your game data will also be deleted.

### ValidationError
If the JSON object you supply to a route is in an unexpected format, you will be
given the following error along with status code `400`:
```json
{
  "error_message": "The given input was not valid",
  "error_code": "ValidationError"
}
```

### InvalidStateTransitionError
If an invalid or illegal operation is requested—for example, raising an amount
below the minimum raise—you will be given the following error along with status
code `400`:
```json
{
  "error_message": "The requested operation is invalid in the current game state",
  "error_code": "InvalidStateTransitionError"
}
```
