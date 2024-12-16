'''The messenger between the python API and the fishbait AI'''

from abc import abstractmethod, ABC
from typing import (
  Any, Callable, Concatenate, Dict, List, Literal, TypedDict, Optional, Type
)
from dataclasses import asdict, dataclass, field
from enum import Enum
from ctypes import (
  cdll,
  c_char_p,
  c_uint32,
  c_uint8,
  c_bool,
  c_double,
  c_float,
  c_size_t,
  c_int,
  c_char,
  POINTER,
  Structure,
  CFUNCTYPE,
)
from functools import cached_property

import settings
import props
from error import InvalidStateTransitionError, CPlusPlusError
from utils import get_logger


log = get_logger(__name__)
lib = cdll.LoadLibrary(settings.RELAY_LIB_LOCATION)


Chips = c_uint32
SequenceId = c_uint32
PlayerId = c_uint8
RoundId = c_uint8
ActionCode = c_uint8
ISOCard = c_uint8


class LibArg[A]:
  def __init__(self, arg: Type[A]):
    self.arg = arg

class LibRet[R]:
  def __init__(self, ret: Type[R]):
    self.ret = ret

class LibFn[**P, R]:
  '''Wraps ctypes functions with the appropriate types'''

  def __init__(self, func: Callable[P, R], argtypes: list, restype):
    self.func: Any = func

    self.argtypes = argtypes
    self.func.argtypes = self.argtypes

    self.restype = restype
    self.func.restype = self.restype

  @classmethod
  def name(cls, name: str):
    func = getattr(lib, name)
    return LibFn[[], None](func, [], None)

  @staticmethod
  def _clear_error():
    clearer = lib.ClearError
    clearer.argtypes = []
    clearer.restype = None
    clearer()

  @staticmethod
  def _check_error() -> bytes | None:
    checker = lib.CheckError
    checker.argtypes = []
    checker.restype = c_char_p
    check_result = checker()
    return check_result

  def __call__(self, *args: P.args, **kwargs: P.kwargs) -> R:
    self._clear_error()
    result = self.func(*args, **kwargs)
    error = self._check_error()
    if error is not None:
      error_msg = error.decode('utf-8')
      log.error('Error from C++: %s', error_msg)
      raise CPlusPlusError()
    return result

  def arg[A](self, arg: Type[A]):
    new_arg_fn: Callable[Concatenate[A, P], R] = self.func
    new_argtypes = [arg] + self.argtypes
    return LibFn[Concatenate[A, P], R](
      new_arg_fn, new_argtypes, self.restype  # type: ignore
    )

  def ret[S](self, ret: Type[S]):
    new_res_fn: Callable[P, S] = self.func
    return LibFn[P, S](new_res_fn, self.argtypes, ret)

commander_get_illegal_action_id = (
  LibFn.name('CommanderGetIllegalActionId').ret(SequenceId)
)
commander_get_k_actions = LibFn.name('CommanderGetKActions').ret(c_int)
ILLEGAL_SEQ_ID = commander_get_illegal_action_id()
NUM_ACTIONS = int(commander_get_k_actions())


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

class ActionStruct(Structure):
  '''A ctypes Structure for the C++ ActionStruct struct in commander.cc'''
  _fields_ = [
    ('action', ActionCode),
    ('size', Chips),
    ('action_idx', c_size_t),
  ]

  def to_props(self, could_check: bool) -> props.ApplyProps:
    mapping: Dict[int, Literal['Check/Call'] | props.ActionType] = {
      0: props.ActionType.FOLD,
      1: 'Check/Call',
      2: props.ActionType.BET,
      3: props.ActionType.ALL_IN
    }
    action = mapping[self.action]
    if action == 'Check/Call':
      if could_check:
        action = props.ActionType.CHECK
      else:
        action = props.ActionType.CALL
    action_idx = self.action_idx if self.action_idx != ILLEGAL_SEQ_ID else None
    return props.ApplyProps(
      {'action': action, 'size': self.size, 'action_idx': action_idx}
    )

class AvailableActionStruct(Structure):
  '''A ctypes Structure for the C++ AvailableAction struct in commander.h'''
  _fields_ = [
    ('action', ActionCode),
    ('size', c_double),
    ('policy', c_float),
    ('action_idx', c_size_t)
  ]

  def to_props(self, can_check: bool) -> props.AvailableActionProps:
    mapping: Dict[int, Literal['Check/Call'] | props.ActionType] = {
      0: props.ActionType.FOLD,
      1: 'Check/Call',
      2: props.ActionType.BET,
      3: props.ActionType.ALL_IN
    }
    action = mapping[self.action]
    if action == 'Check/Call':
      if can_check:
        action = props.ActionType.CHECK
      else:
        action = props.ActionType.CALL
    return props.AvailableActionProps({
      'action': action,
      'size': self.size,
      'policy': self.policy,
      'action_idx': self.action_idx,
    })


BinStr = POINTER(c_char)
CallbackFunc = CFUNCTYPE(None, BinStr, c_size_t)

BinStrType = type[BinStr]
SizeTypeType = type[c_size_t]
def handle_commander_buffer[**P, R](
    commander_fn: Callable[Concatenate[BinStrType, SizeTypeType, P], R]
):
  def wrapped(commander: bytes, *args: P.args, **kwargs: P.kwargs) -> R:
    return commander_fn(commander, len(commander), *args, **kwargs)
  return wrapped


# Note: arguments here are in reverse order
commander_create = LibFn.name('CommanderCreate').arg(CallbackFunc).arg(c_char_p)
commander_reset = handle_commander_buffer(
  LibFn.name('CommanderReset')
  .arg(CallbackFunc)
  .arg(PlayerId)
  .arg(Chips)
  .arg(Chips)
  .arg(PlayerId)
  .arg(Chips * settings.PLAYERS)
  .arg(c_size_t)
  .arg(BinStr)
)
commander_set_hand = handle_commander_buffer(
  LibFn.name('CommanderSetHand')
  .arg(CallbackFunc)
  .arg(ISOCard * settings.HAND_CARDS)
  .arg(PlayerId)
  .arg(c_size_t)
  .arg(BinStr)
)

commander_proceed_play = handle_commander_buffer(
  LibFn.name('CommanderProceedPlay')
    .arg(CallbackFunc).arg(c_size_t).arg(BinStr)
)

commander_state = handle_commander_buffer(
  LibFn.name('CommanderState').ret(NodeSnapshot).arg(c_size_t).arg(BinStr)
)
commander_fishbait_seat = handle_commander_buffer(
  LibFn.name('CommanderFishbaitSeat').ret(PlayerId).arg(c_size_t).arg(BinStr)
)
commander_get_available_actions = handle_commander_buffer(
  LibFn.name('CommanderGetAvailableActions')
  .arg(POINTER(AvailableActionStruct))
  .arg(c_size_t)
  .arg(BinStr)
)
commander_query = handle_commander_buffer(
  LibFn.name('CommanderQuery')
    .ret(ActionStruct).arg(CallbackFunc).arg(c_size_t).arg(BinStr)
)
commander_apply = handle_commander_buffer(
  LibFn.name('CommanderApply')
  .arg(CallbackFunc)
  .arg(Chips)
  .arg(ActionCode)
  .arg(c_size_t)
  .arg(BinStr)
)

commander_set_board = handle_commander_buffer(
  LibFn.name('CommanderSetBoard')
  .arg(CallbackFunc)
  .arg(ISOCard * settings.BOARD_CARDS)
  .arg(c_size_t)
  .arg(BinStr)
)

commander_award_pot = handle_commander_buffer(
  LibFn.name('CommanderAwardPot').arg(CallbackFunc).arg(c_size_t).arg(BinStr)
)

commander_new_hand = handle_commander_buffer(
  LibFn.name('CommanderNewHand').arg(CallbackFunc).arg(c_size_t).arg(BinStr)
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
  last_action: List[Optional[props.ApplyProps]] = field(
    default_factory=lambda: [None] * settings.PLAYERS
  )
  '''What each player's last action was'''
  known_board: List[bool] = field(
    default_factory=lambda: [False] * settings.BOARD_CARDS
  )
  '''If each board card is known or not'''
  available_actions: Optional[List[props.AvailableActionProps]] = None
  '''The actions available to fishbait during its last turn to act'''

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

  def pull_from_commander(self, commander: bytes):
    '''Pulls the latest state from the given Commander into this object.'''
    node_snapshot = commander_state(commander, len(commander))

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

    self.fishbait_seat = commander_fishbait_seat(commander, len(commander))
    self._cleanup_cards()
    self._set_player_needs_hand()
    self._set_board_needs_cards()
    self._set_can_muck()

class PigeonInterface(ABC):
  '''Interface for a Pigeon's public functions'''

  @abstractmethod
  def reset(
    self, stacks: props.ChipPlayerList, button: props.PlayerNumber,
    big_blind: props.ChipCount, small_blind: props.ChipCount,
    fishbait_seat: props.PlayerNumber, player_names: props.StrPlayerList
  ) -> None:
    '''Resets fishbait to a state with the given parameters.

    Args:
      stacks: A list of the stack size of each player.
      button: Seat number of the button.
      big_blind: Size of the big blind in number of chips.
      small_blind: Size of the small blind in number of chips.
      fishbait_seat: Seat number of fishbait.
      player_names: A list of names for each player.
    '''
    raise NotImplementedError()

  @abstractmethod
  def set_hand(self, hand: props.Hand) -> None:
    '''Sets the hand of the player who currently needs a card.

    Args:
      hand: List of cards to set the given player's hand to. Passing 0 for
          both cards mucks the player.
    '''
    raise NotImplementedError()

  @abstractmethod
  def apply(self, action: props.ActionType, size: props.ActionSize) -> None:
    '''Applies the given action to the current acting player.'''
    raise NotImplementedError()

  @abstractmethod
  def set_board(self, board: props.Board) -> None:
    raise NotImplementedError()

  @abstractmethod
  def state_dict(self) -> Dict[str, Any]:
    '''Returns the current state of the game as a dictionary'''
    raise NotImplementedError()

  @abstractmethod
  def new_hand(self) -> None:
    raise NotImplementedError()

class Pigeon(PigeonInterface):
  '''Sends messages between python clients and the fishbait C++ AI.'''

  def __init__(self):
    self._commander = b''

    strategy_loc = settings.STRATEGY_LOCATION
    commander_create(bytes(strategy_loc, 'utf-8'), self._commander_callback)

    self._state: PigeonState = PigeonState()
    self._update_state()

  @cached_property
  def _commander_callback(self):
    def callback(data, size):
      self._commander = bytes(data[:size])
    return CallbackFunc(callback)

  def __getstate__(self) -> object:
    return (self._commander, self._state)

  def __setstate__(self, state: tuple[bytes, PigeonState]):
    self._commander, self._state = state

  @staticmethod
  def _auto_advance[**P, T](fn: Callable[Concatenate['Pigeon', P], T]):
    # pylint: disable=protected-access
    def auto_advancer(pigeon_obj: 'Pigeon', *args: P.args, **kwargs: P.kwargs):
      result = fn(pigeon_obj, *args, **kwargs)
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
    return auto_advancer

  def reset(
    self, stacks: props.ChipPlayerList, button: props.PlayerNumber,
    big_blind: props.ChipCount, small_blind: props.ChipCount,
    fishbait_seat: props.PlayerNumber, player_names: props.StrPlayerList
  ):
    c_stacks = stacks.c_arr(Chips)
    commander_reset(
      self._commander, c_stacks, button, big_blind, small_blind, fishbait_seat,
      self._commander_callback
    )
    self._state = PigeonState(player_names=player_names)
    self._update_state()

  @_auto_advance
  def set_hand(self, hand: props.Hand):
    if self._state.player_needs_hand is None:
      log.error('No player needs a hand right now')
      raise InvalidStateTransitionError()
    iso_hand = hand.c_arr(ISOCard)
    commander_set_hand(
      self._commander, self._state.player_needs_hand, iso_hand,
      self._commander_callback
    )
    self._state.known_cards[self._state.player_needs_hand] = True

  @_auto_advance
  def apply(self, action: props.ActionType, size: props.ActionSize):
    can_check = self._state.needed_to_call == 0
    if action == props.ActionType.CHECK and not can_check:
      log.error('The currently acting player cannot check.')
      raise InvalidStateTransitionError()
    action_map = {
      props.ActionType.FOLD: 0,
      props.ActionType.CHECK: 1,
      props.ActionType.CALL: 1,
      props.ActionType.BET: 2,
      props.ActionType.ALL_IN: 3,
    }
    action_code = action_map[action]
    commander_apply(
      self._commander, action_code, size, self._commander_callback
    )
    self._state.last_action[self._state.acting_player] = (
      ActionStruct(action_code, size, ILLEGAL_SEQ_ID).to_props(can_check)
    )

  @_auto_advance
  def set_board(self, board: props.Board):
    self._state.known_board = [card is not None for card in board]
    commander_set_board(
      self._commander, board.c_arr(ISOCard), self._commander_callback
    )

  def state_dict(self):
    return asdict(self._state)

  def new_hand(self):
    commander_new_hand(self._commander, self._commander_callback)
    self._state.known_cards = [False] * settings.PLAYERS
    self._state.last_action = [None] * settings.PLAYERS
    self._state.known_board = [False] * settings.BOARD_CARDS
    self._state.available_actions = None
    self._update_state()

  @_auto_advance
  def _proceed_play(self):
    '''Proceeds play to the next round.'''
    commander_proceed_play(self._commander, self._commander_callback)
    self._state.last_action = [None] * settings.PLAYERS
    self._state.available_actions = None

  def _update_state(self):
    '''
    Modifies all other variables in our state after we make a change to one of
    them.
    '''
    self._state.pull_from_commander(self._commander)

  @_auto_advance
  def _query(self):
    '''Asks fishbait to decide on an action and take it.'''
    can_check = self._state.needed_to_call == 0
    available_action_arr = (AvailableActionStruct * NUM_ACTIONS)()
    commander_get_available_actions(self._commander, available_action_arr)
    available_action_list: List[AvailableActionStruct] = [
      act for act in available_action_arr
      if act.action_idx != ILLEGAL_SEQ_ID and act.policy != 0
    ]
    self._state.available_actions = [
      act.to_props(can_check) for act in available_action_list
    ]

    action_taken = (
      commander_query(self._commander, self._commander_callback)
        .to_props(can_check)
    )
    fishbait_seat = commander_fishbait_seat(self._commander)
    self._state.last_action[fishbait_seat] = action_taken

  def _award_pot(self):
    commander_award_pot(
      self._commander, self._commander_callback
    )
    self._update_state()
