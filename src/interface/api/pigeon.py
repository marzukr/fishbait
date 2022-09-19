# Copyright 2021 Marzuk Rashid

'''The messenger between the python API and the fishbait AI'''

from typing import (
  Callable, Concatenate, Dict, Generic, List, Literal, TypedDict, Optional,
  TypeVar, ParamSpec
)
from dataclasses import asdict, dataclass, field
from enum import Enum

from ctypes import cdll
from ctypes import (
  Structure,
  c_void_p,
  c_char_p,
  c_uint32,
  c_uint8,
  c_bool,
  c_double,
)

import settings
from ctypes_wrappers import (
  wrap_function_1,
  wrap_function_2,
  wrap_function_3,
  wrap_function_6,
)


lib = cdll.LoadLibrary(settings.RELAY_LIB_LOCATION)


Chips = c_uint32
PlayerId = c_uint8
CommanderPtr = c_void_p
ISOCard = c_uint8
RoundId = c_uint8
ActionCode = c_uint8


class Round(str, Enum):
  PREFLOP = 'Preflop'
  FLOP = 'Flop'
  TURN = 'Turn'
  RIVER = 'River'


class NodeSnapshot(Structure):
  '''A ctypes Structure for the C++ NodeSnapshot struct.'''
  _fields_ = [
    ('players', PlayerId),
    ('big_blind', Chips),
    ('small_blind', Chips),
    ('ante', Chips),
    ('big_blind_ante', c_bool),
    ('blind_before_ante', c_bool),
    ('rake', c_double),
    ('rake_cap', Chips),
    ('no_flop_no_drop', c_bool),
    ('button', PlayerId),
    ('in_progress', c_bool),
    ('round', RoundId),
    ('acting_player', PlayerId),
    ('folded', c_bool * settings.PLAYERS),
    ('players_left', PlayerId),
    ('players_all_in', PlayerId),
    ('pot', Chips),
    ('bets', Chips * settings.PLAYERS),
    ('stack', Chips * settings.PLAYERS),
    ('min_raise', Chips),
    ('needed_to_call', Chips),
    ('hands', (ISOCard * settings.HAND_CARDS) * settings.PLAYERS),
    ('board', ISOCard * settings.BOARD_CARDS)
  ]

  def round_name(self):
    mapping = {0: Round.PREFLOP, 1: Round.FLOP, 2: Round.TURN, 3: Round.RIVER}
    return mapping[self.round]


class Action(str, Enum):
  FOLD = 'Fold'
  CHECK = 'Check'
  CALL = 'Call'
  BET = 'Bet'
  ALL_IN = 'All In'

class ActionStruct(Structure):
  '''A ctypes Structure for the C++ ActionStruct struct in commander.cc'''
  _fields_ = [
    ('action', ActionCode),
    ('size', Chips),
    ('could_check', c_bool)
  ]

  class ActionDict(TypedDict):
    action: Action
    size: int

  def to_dict(self) -> ActionDict:
    mapping: Dict[int, Literal['Check/Call'] | Action] = {
      0: Action.FOLD, 1: 'Check/Call', 2: Action.BET, 3: Action.ALL_IN
    }
    action = mapping[self.action]
    size = self.size
    if action == 'Check/Call':
      if self.could_check:
        action = Action.CHECK
      else:
        action = Action.CALL
    return ActionStruct.ActionDict(action=action, size=size)


commander_new = wrap_function_1(
  lib, 'CommanderNew',
  restype=CommanderPtr,
  argtypes=(c_char_p,)
)
commander_delete = wrap_function_1(
  lib, 'CommanderDelete',
  restype=None,
  argtypes=(CommanderPtr,)
)
commander_reset = wrap_function_6(
  lib, 'CommanderReset',
  restype=None,
  argtypes=(
    CommanderPtr,
    Chips * settings.PLAYERS,
    PlayerId,
    Chips,
    Chips,
    PlayerId,
  ),
)
commander_set_hand = wrap_function_3(
  lib, 'CommanderSetHand',
  restype=None,
  argtypes=(
    CommanderPtr,
    PlayerId,
    ISOCard * settings.HAND_CARDS,
  ),
)
commander_proceed_play = wrap_function_1(
  lib, 'CommanderProceedPlay',
  restype=None,
  argtypes=(CommanderPtr,),
)
commander_state = wrap_function_1(
  lib, 'CommanderState',
  restype=NodeSnapshot,
  argtypes=(CommanderPtr,),
)
commander_fishbait_seat = wrap_function_1(
  lib, 'CommanderFishbaitSeat',
  restype=PlayerId,
  argtypes=(CommanderPtr,),
)
commander_query = wrap_function_1(
  lib, 'CommanderQuery',
  restype=ActionStruct,
  argtypes=(CommanderPtr,),
)
commander_apply = wrap_function_3(
  lib, 'CommanderApply',
  restype=None,
  argtypes=(
    CommanderPtr,
    ActionCode,
    Chips,
  ),
)
commander_set_board = wrap_function_2(
  lib, 'CommanderSetBoard',
  restype=None,
  argtypes=(
    CommanderPtr,
    ISOCard * settings.BOARD_CARDS,
  ),
)
commander_award_pot = wrap_function_1(
  lib, 'CommanderAwardPot',
  restype=None,
  argtypes=(CommanderPtr,),
)
commander_new_hand = wrap_function_1(
  lib, 'CommanderNewHand',
  restype=None,
  argtypes=(CommanderPtr,),
)


@dataclass
class PigeonState:
  '''A representation of the game state that is JSON serializable for the FE'''

  class BoardNeedsCards(TypedDict):
    start: int
    '''The first board card we need info for'''
    end: int
    '''One past the last board card we need info for'''

  ### State Variables from NodeSnapshot ###
  players: int = settings.PLAYERS
  '''The number of players in the game'''
  big_blind: int = 0
  '''The size of the big blind in chips'''
  small_blind: int = 0
  '''The size of the small blind in chips'''
  ante: int = 0
  '''The size of the ante in chips'''
  big_blind_ante: bool = False
  '''If the big blind pays the ante for all players'''
  blind_before_ante: bool = False
  '''If blinds should be posted before the ante'''
  rake: float = False
  '''The size of the rake as proportion of the pot'''
  rake_cap: int = 0
  '''The maximum size of the rake. 0 if no maximum'''
  no_flop_no_drop: bool = True
  '''If hands that end preflop are not raked'''
  button: int = 0
  '''Player id of the button'''
  in_progress: bool = False
  '''If the game is currently in progress'''
  round: Round = Round.PREFLOP
  '''What round the game is currently in'''
  acting_player: int = 0
  '''Player id of the acting player'''
  folded: List[bool] = field(
    default_factory=lambda: [False] * settings.PLAYERS
  )
  '''If each player is folded or not'''
  players_left: int = 0
  '''How many players have not folded yet'''
  players_all_in: int = 0
  '''How many players are all in'''
  pot: int = 0
  '''The total size of the pot'''
  bets: List[int] = field(
    default_factory=lambda: [0] * settings.PLAYERS
  )
  '''How much each player has contributed to the pot'''
  stack: List[int] = field(
    default_factory=lambda: [0] * settings.PLAYERS
  )
  '''The size of each player's stack'''
  min_raise: int = 0
  '''The minimum legal raise size for the acting player'''
  needed_to_call: int = 0
  '''The amount the current player needs to call'''
  hands: List[Optional[List[Optional[int]]]] = field(
    default_factory=lambda: [None] * settings.PLAYERS
  )
  '''
  Each players hand. If we don't know the player's hand, then it will be None.
  If the player has mucked, then it will be a list filled with None.
  '''
  board: List[Optional[int]] = field(
    default_factory=lambda: [None] * settings.BOARD_CARDS
  )
  '''The cards on the public board. None if we don't know the card yet.'''

  ### From commander_fishbait_seat ###
  fishbait_seat: int = 0
  '''Which player number fishbait is'''

  ### Set with self._set_player_needs_hand ###
  player_needs_hand: Optional[int] = None
  '''Which player if any we need hand info for'''

  ### Set with self._set_board_needs_cards ###
  board_needs_cards: Optional[BoardNeedsCards] = None
  '''Which board cards if any we need info for'''

  ### Set with self._set_can_muck ###
  can_muck: bool = False
  '''If the player we need hand info for can muck'''

  ### Managed by Pigeon ###
  player_names: List[str] = field(default_factory=lambda: [
    'Biden', 'Harris', 'Pelosi', 'McCarthy', 'Schumer', 'McConnell'
  ])
  '''Names of each player'''
  known_cards: List[bool] = field(
    default_factory=lambda: [False] * settings.PLAYERS
  )
  '''If each player's hand is known or not'''
  last_action: List[Optional[ActionStruct.ActionDict]] = field(
    default_factory=lambda: [None] * settings.PLAYERS
  )
  '''What each player's last action was'''
  known_board: List[bool] = field(
    default_factory=lambda: [False] * settings.BOARD_CARDS
  )
  '''If each board card is known or not'''

  def _set_can_muck(self):
    """
    Sets `can_muck` properly after a pull from a commander.

    Should only be called by pull_from_commander to ensure a proper state is
    maintained.
    """
    if self.player_needs_hand == self.fishbait_seat:
      self.can_muck = False
      return
    cards_entered = 0
    cards_defined = 0
    for i in range(settings.PLAYERS):
      if self.folded[i]:
        continue
      if self.hands[i] is not None:
        cards_entered += 1
        if self.hands[i][0] != self.hands[i][1]:
          cards_defined += 1
    cards_left_to_enter = self.players_left - cards_entered
    # There must be at least one other player who has defined cards or has still
    # to enter their cards. cards_left_to_enter counts the player currently
    # entering their cards, so this sum can't be equal to 1, must be greater.
    self.can_muck = cards_defined + cards_left_to_enter > 1

  def _set_board_needs_cards(self):
    """
    Sets `board_needs_cards` properly after a pull from a commander.

    Should only be called by pull_from_commander to ensure a proper state is
    maintained.
    """
    if self.acting_player != settings.PLAYERS:
      self.board_needs_cards = None
      return
    elif self.round == Round.FLOP and None in self.board[:3]:
      self.board_needs_cards = PigeonState.BoardNeedsCards(start=0, end=3)
      return
    elif self.round == Round.TURN and self.board[3] is None:
      self.board_needs_cards = PigeonState.BoardNeedsCards(start=3, end=4)
      return
    elif self.round == Round.RIVER and self.board[4] is None:
      self.board_needs_cards = PigeonState.BoardNeedsCards(start=4, end=5)
      return
    self.board_needs_cards = None
    return

  def _set_player_needs_hand(self):
    """
    Sets `player_needs_hand` properly after a pull from a commander.

    Should only be called by pull_from_commander to ensure a proper state is
    maintained.
    """
    fishbait_needs_cards = (
      self.round == Round.PREFLOP
      and self.in_progress
      and self.acting_player == settings.PLAYERS
      and self.hands[self.fishbait_seat] is None
    )
    other_players_might_need_cards = (
      not self.in_progress
      and self.players_left > 1
      and self.round == Round.RIVER
    )
    if fishbait_needs_cards:
      self.player_needs_hand = self.fishbait_seat
      return
    elif other_players_might_need_cards:
      for i in range(settings.PLAYERS):
        player = (self.acting_player + i) % settings.PLAYERS
        if self.folded[player]:
          continue
        if self.hands[player] is not None:
          continue
        self.player_needs_hand = player
        return
    self.player_needs_hand = None

  def _cleanup_cards(self):
    """
    Sets `board` properly after a pull from a commander.

    Should only be called by pull_from_commander to ensure a proper state is
    maintained.
    """
    for i in range(settings.BOARD_CARDS):
      if not self.known_board[i]:
        self.board[i] = None
    for i in range(settings.PLAYERS):
      if not self.known_cards[i]:
        self.hands[i] = None
      elif self.hands[i] == [0] * settings.HAND_CARDS:
        self.hands[i] = [None] * settings.HAND_CARDS

  def pull_from_commander(self, commander: CommanderPtr):
    '''Pulls the latest state from the given Commander into this object.'''
    node_snapshot = commander_state(commander)

    direct_copy_fields = [
      'players', 'big_blind', 'small_blind', 'ante', 'big_blind_ante',
      'blind_before_ante', 'rake', 'rake_cap', 'no_flop_no_drop', 'button',
      'in_progress', 'acting_player', 'players_left', 'players_all_in', 'pot',
      'min_raise', 'needed_to_call'
    ]
    for copy_field in direct_copy_fields:
      node_value = getattr(node_snapshot, copy_field)
      setattr(self, copy_field, node_value)

    direct_array_copy_fields = ['folded', 'bets', 'stack', 'board']
    for copy_field in direct_array_copy_fields:
      node_array = getattr(node_snapshot, copy_field)
      list_of_array = list(node_array)
      setattr(self, copy_field, list_of_array)

    self.round = node_snapshot.round_name()

    hands_array = node_snapshot.hands
    self.hands = [list(hands_array[i]) for i in range(len(hands_array))]

    self.fishbait_seat = commander_fishbait_seat(commander)
    self._cleanup_cards()
    self._set_player_needs_hand()
    self._set_board_needs_cards()
    self._set_can_muck()


T = TypeVar('T')
P = ParamSpec('P')

class Pigeon:
  '''Sends messages between python clients and the fishbait C++ AI.'''

  def __init__(self):
    strategy_loc = settings.STRATEGY_LOCATION
    self._commander: CommanderPtr = commander_new(bytes(strategy_loc, 'utf-8'))
    self._state: PigeonState = PigeonState()
    self._update_state()

  def __del__(self):
    commander_delete(self._commander)

  class _AutoAdvance(Generic[P, T]):
    '''Decorator to advance the game after calling the given function'''

    def __init__(self, fn: Callable[Concatenate['Pigeon', P], T]):
      self._fn = fn

    def __call__(
      self, pigeon_obj: 'Pigeon', *args: P.args, **kwargs: P.kwargs
    ) -> T:
      result = self._fn(pigeon_obj, *args, **kwargs)
      pigeon_obj._update_state()

      state = pigeon_obj._state

      should_proceed_play = (
        state.player_needs_hand is None
        and state.acting_player == settings.PLAYERS
        and state.board_needs_cards is None
      )
      if should_proceed_play:
        pigeon_obj._proceed_play()

      should_query = (
        state.acting_player == state.fishbait_seat
        and state.in_progress
      )
      if should_query:
        pigeon_obj._query()

      should_award_pot = (
        state.pot > 0
        and not state.in_progress
        and state.player_needs_hand is None
      )
      if should_award_pot:
        pigeon_obj._award_pot()

      return result

    def __get__(self, pigeon_obj: 'Pigeon', _):
      def partial(*args: P.args, **kwargs: P.kwargs) -> T:
        return self(pigeon_obj, *args, **kwargs)
      return partial

  def reset(
    self, stacks: List[int], button: int, big_blind: int, small_blind: int,
    fishbait_seat: int, player_names: List[str]
  ):
    '''Resets fishbait to a state with the given parameters.

    Args:
      stacks: A list of the stack size of each player.
      button: Seat number of the button.
      big_blind: Size of the big blind in number of chips.
      small_blind: Size of the small blind in number of chips.
      fishbait_seat: Seat number of fishbait.
      player_names: A list of names for each player.
    '''
    stacks = (Chips * settings.PLAYERS)(*stacks)
    commander_reset(self._commander, stacks, button, big_blind, small_blind,
                    fishbait_seat)
    self._state = PigeonState(player_names=player_names)
    self._update_state()

  @_AutoAdvance
  def set_hand(self, hand: List[Optional[int]]):
    '''Sets the hand of the player who currently needs a card.

    Args:
      hand: List of cards to set the given player's hand to. Passing None for
          both cards mucks the player.
    '''
    if hand == [None] * settings.HAND_CARDS:
      hand = [0] * settings.HAND_CARDS
    hand = (ISOCard * settings.HAND_CARDS)(*hand)
    commander_set_hand(self._commander, self._state.player_needs_hand, hand)
    self._state.known_cards[self._state.player_needs_hand] = True

  @_AutoAdvance
  def apply(self, action: Action, size: int = 0):
    '''Applies the given action to the current acting player.'''
    can_check = self._state.needed_to_call == 0
    if action == Action.CHECK and not can_check:
      raise ValueError('The currently acting player cannot check.')
    action_map = {
      Action.FOLD: 0, Action.CHECK: 1, Action.CALL: 1, Action.BET: 2,
      Action.ALL_IN: 3
    }
    action_code = action_map[action]
    commander_apply(self._commander, action_code, size)
    self._state.last_action[self._state.acting_player] = (
      ActionStruct(action_code, size, can_check).to_dict()
    )

  @_AutoAdvance
  def set_board(self, board: List[int]):
    self._state.known_board = [card is not None for card in board]
    board = [0 if card is None else card for card in board]
    board = (ISOCard * settings.BOARD_CARDS)(*board)
    commander_set_board(self._commander, board)

  def state_dict(self):
    '''Returns the current state of the game as a dictionary'''
    return asdict(self._state)

  def new_hand(self):
    commander_new_hand(self._commander)
    self._state.known_cards = [False] * settings.PLAYERS
    self._state.last_action = [None] * settings.PLAYERS
    self._state.known_board = [False] * settings.BOARD_CARDS
    self._update_state()

  @_AutoAdvance
  def _proceed_play(self):
    '''Proceeds play to the next round.'''
    commander_proceed_play(self._commander)
    self._state.last_action = [None] * settings.PLAYERS

  def _update_state(self):
    '''
    Modifies all other variables in our state after we make a change to one of
    them.
    '''
    self._state.pull_from_commander(self._commander)

  @_AutoAdvance
  def _query(self):
    '''Asks fishbait to decide on an action and take it.'''
    action_taken = commander_query(self._commander).to_dict()
    fishbait_seat = commander_fishbait_seat(self._commander)
    self._state.last_action[fishbait_seat] = action_taken

  def _award_pot(self):
    commander_award_pot(self._commander)
    self._update_state()
