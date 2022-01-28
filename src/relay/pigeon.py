# Copyright 2021 Marzuk Rashid

'''Python interface for the Fishbait AI'''

import os
try:
  from ctypes import windll
except ImportError:
  from ctypes import cdll
from ctypes import Structure, c_void_p, c_char_p, c_uint32, c_uint8, c_bool
from ctypes import c_double, Array

pigeon_script_dir = os.path.dirname(__file__)
fishbait_lib_dir = os.path.join(pigeon_script_dir, '../../build/lib')
possible_lib_exts = ['.so', '.dylib', '.dll']
lib = None
for ext in possible_lib_exts:
  lib_path = os.path.join(fishbait_lib_dir, 'librelay' + ext)
  if ext == '.dll':
    try:
      lib = windll.LoadLibrary(lib_path)
    except OSError:
      pass
    else:
      break
  else:
    try:
      lib = cdll.LoadLibrary(lib_path)
    except OSError:
      pass
    else:
      break
if lib is None:
  raise Exception('Fishbait shared library not found.')

PLAYERS = 6
HAND_CARDS = 2
BOARD_CARDS = 5

Chips = c_uint32
PlayerId = c_uint8
CommanderPtr = c_void_p
ISOCard = c_uint8
RoundId = c_uint8
Action = c_uint8

class NodeSnapshot(Structure):
  '''A ctypes Structure for the C++ NodeSnapshot struct.'''
  _fields_ = [('players', PlayerId), ('big_blind', Chips),
              ('small_blind', Chips), ('ante', Chips),
              ('big_blind_ante', c_bool), ('blind_before_ante', c_bool),
              ('rake', c_double), ('rake_cap', Chips),
              ('no_flop_no_drop', c_bool), ('button', PlayerId),
              ('in_progress', c_bool), ('round', RoundId),
              ('acting_player', PlayerId), ('folded', c_bool * PLAYERS),
              ('players_left', PlayerId), ('players_all_in', PlayerId),
              ('pot', Chips), ('bets', Chips * PLAYERS),
              ('stack', Chips * PLAYERS), ('min_raise', Chips),
              ('needed_to_call', Chips),
              ('hands', (ISOCard * HAND_CARDS) * PLAYERS),
              ('board', ISOCard * BOARD_CARDS)]

  def round_str(self, round_n):
    mapping = {0: 'Preflop', 1: 'Flop', 2: 'Turn', 3: 'River'}
    return mapping[round_n]

  def to_dict(self):
    props_dict = {}
    for (field_name, _) in self._fields_:
      field_value = getattr(self, field_name)
      if isinstance(field_value, Array) and field_name != 'hands':
        field_value = list(field_value)
      elif field_name == 'hands':
        field_value = [list(field_value[i]) for i in range(len(field_value))]
      elif field_name == 'round':
        field_value = self.round_str(field_value)
      props_dict[field_name] = field_value
    return props_dict

class ActionStruct(Structure):
  '''A ctypes Structure for the C++ ActionStruct struct in commander.cc'''
  _fields_ = [('action', Action), ('size', Chips), ('could_check', c_bool)]

  def to_dict(self):
    mapping = {0: 'Fold', 1: 'Check/Call', 2: 'Bet', 3: 'All In'}
    ret_dict = { 'action': mapping[self.action] }
    if self.action == 1:
      if self.could_check:
        ret_dict['action'] = 'Check'
      else:
        ret_dict['action'] = 'Call'
    elif self.action == 2:
      ret_dict['size'] = self.size
    return ret_dict

def wrap_function(clib, funcname, restype, argtypes):
  '''Simplify wrapping ctypes functions'''
  func = clib.__getattr__(funcname)
  func.restype = restype
  func.argtypes = argtypes
  return func

commander_new = wrap_function(lib, 'CommanderNew', CommanderPtr, [c_char_p])
commander_delete = wrap_function(lib, 'CommanderDelete', None, [CommanderPtr])
commander_reset = wrap_function(lib, 'CommanderReset', None,
    [CommanderPtr, Chips * PLAYERS, PlayerId, Chips, Chips, PlayerId])
commander_set_hand = wrap_function(lib, 'CommanderSetHand', None,
    [CommanderPtr, PlayerId, ISOCard * HAND_CARDS])
commander_proceed_play = wrap_function(lib, 'CommanderProceedPlay', None,
    [CommanderPtr])
commander_state = wrap_function(lib, 'CommanderState', NodeSnapshot,
    [CommanderPtr])
commander_fishbait_seat = wrap_function(lib, 'CommanderFishbaitSeat', PlayerId,
    [CommanderPtr])
commander_query = wrap_function(lib, 'CommanderQuery', ActionStruct,
    [CommanderPtr])
commander_apply = wrap_function(lib, 'CommanderApply', None,
    [CommanderPtr, Action, Chips])
commander_set_board = wrap_function(lib, 'CommanderSetBoard', None,
    [CommanderPtr, (ISOCard * BOARD_CARDS)])
commander_award_pot = wrap_function(lib, 'CommanderAwardPot', None,
    [CommanderPtr])
commander_new_hand = wrap_function(lib, 'CommanderNewHand', None,
    [CommanderPtr])

class Pigeon:
  '''Sends messages between python clients and the fishbait C++ AI.'''

  def __init__(self, strategy_loc):
    self.commander = commander_new(bytes(strategy_loc, 'utf-8'))
    self.last_state = {
      'player_names': ['Biden', 'Harris', 'Pelosi', 'McCarthy', 'Schumer',
                       'McConnell'],
      'known_cards': [False] * PLAYERS,
      'last_action': [None] * PLAYERS,
      'known_board': [False] * BOARD_CARDS
    }
    self.__update_state()

  def __del__(self):
    commander_delete(self.commander)

  def __auto_advance(f):  # pylint: disable=no-self-argument, unused-private-member
    # pylint: disable=protected-access
    def auto_advancer(*args, **kwargs):
      pigeon_obj = args[0]
      f(*args, **kwargs)  # pylint: disable=not-callable
      pigeon_obj.__update_state()

      should_proceed_play = (
        pigeon_obj.last_state['player_needs_hand'] is None and
        pigeon_obj.last_state['acting_player'] is PLAYERS and
        pigeon_obj.last_state['board_needs_cards'] is None
      )
      if should_proceed_play:
        pigeon_obj.__proceed_play()

      should_query = ((
          pigeon_obj.last_state['acting_player'] ==
          pigeon_obj.last_state['fishbait_seat']
        ) and
        pigeon_obj.last_state['in_progress']
      )
      if should_query:
        pigeon_obj.__query()

      should_award_pot = (
        pigeon_obj.last_state['pot'] > 0 and
        not pigeon_obj.last_state['in_progress'] and
        pigeon_obj.last_state['player_needs_hand'] is None
      )
      if should_award_pot:
        pigeon_obj.__award_pot()

    auto_advancer.__name__ = f.__name__
    return auto_advancer

  def reset(self, stacks, button, big_blind, small_blind, fishbait_seat,
            player_names):
    '''Resets fishbait to a state with the given parameters.

    Args:
      stacks: A list of the stack size of each player.
      button: Seat number of the button.
      big_blind: Size of the big blind in number of chips.
      small_blind: Size of the small blind in number of chips.
      fishbait_seat: Seat number of fishbait.
      player_names: A list of names for each player.
    '''
    stacks = (Chips * PLAYERS)(*stacks)
    commander_reset(self.commander, stacks, button, big_blind, small_blind,
                    fishbait_seat)
    self.last_state = {
      'player_names': player_names,
      'known_cards': [False] * PLAYERS,
      'last_action': [None] * PLAYERS,
      'known_board': [False] * BOARD_CARDS
    }
    self.__update_state()

  @__auto_advance
  def set_hand(self, hand):
    '''Sets the hand of the player who currently needs a card.

    Args:
      hand: List of cards to set the given player's hand to.
    '''
    if hand == [None] * HAND_CARDS:
      hand = [0] * HAND_CARDS
    hand = (ISOCard * HAND_CARDS)(*hand)
    commander_set_hand(self.commander, self.last_state['player_needs_hand'],
                       hand)
    self.last_state['known_cards'][self.last_state['player_needs_hand']] = True

  @__auto_advance
  def apply(self, action, size = 0):
    '''Applies the given action to the current acting player.'''
    current_state = self.state()
    can_check = current_state['needed_to_call'] == 0
    if action == 'Check' and not can_check:
      raise ValueError('The currently acting player cannot check.')
    action_map = { 'Fold': 0, 'Check': 1, 'Call': 1, 'Bet': 2, 'All In': 3 }
    action_code = action_map[action]
    commander_apply(self.commander, action_code, size)
    self.last_state['last_action'][current_state['acting_player']] = (
      ActionStruct(action_code, size, can_check).to_dict()
    )

  @__auto_advance
  def set_board(self, board):
    self.last_state['known_board'] = [card is not None for card in board]
    board = [0 if card is None else card for card in board]
    board = (ISOCard * BOARD_CARDS)(*board)
    commander_set_board(self.commander, board)

  def state(self):
    '''Returns the current state of the game

    Return:
      Dictionary with the following keys:
        'players': Int. The number of players in the game.
        'big_blind': Int. The size of the big blind in chips.
        'small_blind': Int. The size of the small blind in chips.
        'ante': Int. The size of the ante in chips.
        'big_blind_ante': Bool. If the big blind pays the ante for all players.
        'blind_before_ante': Bool. If blinds should be posted before the ante.
        'rake': Double. The size of the rake as proportion of the pot.
        'rake_cap': Int. The maximum size of the rake. 0 if no maximum.
        'no_flop_no_drop': Bool. If hands that end preflop are not raked.
        'button': Int. Player id of the button.
        'in_progress': Bool. If the game is currently in progress.
        'round': String. What round the game is currently in.
        'acting_player': Int. Player id of the acting player.
        'folded': List of Bool. If each player is folded or not.
        'players_left': Int. How many players have not folded yet.
        'players_all_in': Int. How many players are all in.
        'pot': Int. The total size of the pot.
        'bets': List of Int. How much each player has contributed to the pot.
        'stack': List of Int. The size of each player's stack.
        'min_raise': Int. The minimum legal raise size for the acting player.
        'needed_to_call': Int. The amount the current player needs to call.
        'hands': List of List of Int. Each players hand.
        'board': List of Int. The cards on the public board.
        'player_names': List of String. Names of each player.
        'known_cards': List of Bool. If each player's hand is known or not.
        'last_action': List of String. What each player's last action was.
        'known_board': List of Bool. If each board card is known or not.
        'player_needs_hand': Int. Which player if any we need hand info for.
        'board_needs_cards': Dict. Which board cards if any we need info for.
          'start': Int. The first board card we need info for.
          'end': Int. One past the last board card we need info for.
        'can_muck': Bool. If the player we need hand info for can muck.
    '''
    return self.last_state

  def new_hand(self):
    commander_new_hand(self.commander)
    self.last_state = {
      'player_names': self.last_state['player_names'],
      'known_cards': [False] * PLAYERS,
      'last_action': [None] * PLAYERS,
      'known_board': [False] * BOARD_CARDS
    }
    self.__update_state()

  @__auto_advance
  def __proceed_play(self):  # pylint: disable=unused-private-member
    '''Proceeds play to the next round.'''
    commander_proceed_play(self.commander)
    self.last_state['last_action'] = [None] * PLAYERS

  def __can_muck(part_state):  # pylint: disable=no-self-argument
    # pylint: disable=unsubscriptable-object, unsupported-assignment-operation
    if part_state['player_needs_hand'] == part_state['fishbait_seat']:
      part_state['can_muck'] = False
      return
    cards_entered = 0
    cards_defined = 0
    for i in range(PLAYERS):
      if part_state['folded'][i]:
        continue
      if part_state['hands'][i] is not None:
        cards_entered += 1
        if part_state['hands'][i][0] != part_state['hands'][i][1]:
          cards_defined += 1
    cards_left_to_enter = part_state['players_left'] - cards_entered
    # There must be at least one other player who has defined cards or has still
    # to enter their cards. cardsLeftToEnter counts the player currently
    # entering their cards, so this sum can't be equal to 1, must be greater.
    part_state['can_muck'] = cards_defined + cards_left_to_enter > 1

  def __board_needs_cards(part_state):  # pylint: disable=no-self-argument
    # pylint: disable=unsubscriptable-object, unsupported-assignment-operation
    if part_state['acting_player'] != PLAYERS:
      part_state['board_needs_cards'] = None
      return
    flop_needs_cards = (
      part_state['round'] == 'Flop' and (
        part_state['board'][0] is None or
        part_state['board'][1] is None or
        part_state['board'][2] is None
      )
    )
    if flop_needs_cards:
      part_state['board_needs_cards'] = { 'start': 0, 'end': 3 }
      return
    elif part_state['round'] == 'Turn' and part_state['board'][3] is None:
      part_state['board_needs_cards'] = { 'start': 3, 'end': 4 }
      return
    elif part_state['round'] == 'River' and part_state['board'][4] is None:
      part_state['board_needs_cards'] = { 'start': 4, 'end': 5 }
      return
    part_state['board_needs_cards'] = None
    return

  def __player_needs_hand(part_state):  # pylint: disable=no-self-argument
    # pylint: disable=unsubscriptable-object, unsupported-assignment-operation
    fishbait_needs_cards = (
      part_state['round'] == 'Preflop' and
      part_state['in_progress'] and
      part_state['acting_player'] == PLAYERS and
      part_state['hands'][part_state['fishbait_seat']] is None
    )
    other_players_might_need_cards = (
      not part_state['in_progress'] and
      part_state['players_left'] > 1 and
      part_state['round'] == 'River'
    )
    if fishbait_needs_cards:
      part_state['player_needs_hand'] = part_state['fishbait_seat']
      return
    elif other_players_might_need_cards:
      for i in range(PLAYERS):
        player = (part_state['acting_player'] + i) % PLAYERS
        if part_state['folded'][player]:
          continue
        if part_state['hands'][player] is not None:
          continue
        part_state['player_needs_hand'] = player
        return
    part_state['player_needs_hand'] = None

  def __cleanup_cards(part_state):  # pylint: disable=no-self-argument
    # pylint: disable=unsubscriptable-object
    for i in range(BOARD_CARDS):
      if not part_state['known_board'][i]:
        part_state['board'][i] = None
    for i in range(PLAYERS):
      if not part_state['known_cards'][i]:
        part_state['hands'][i] = None
      elif part_state['hands'][i] == [0] * HAND_CARDS:
        part_state['hands'][i] = [None] * HAND_CARDS

  def __update_state(self):
    state_dict = commander_state(self.commander).to_dict()
    fishbait_seat = commander_fishbait_seat(self.commander)
    state_dict['fishbait_seat'] = fishbait_seat
    state_dict['player_names'] = self.last_state['player_names'][:PLAYERS]
    state_dict['last_action'] = self.last_state['last_action']
    state_dict['known_cards'] = self.last_state['known_cards']
    state_dict['known_board'] = self.last_state['known_board']
    Pigeon.__cleanup_cards(state_dict)
    Pigeon.__player_needs_hand(state_dict)
    Pigeon.__board_needs_cards(state_dict)
    Pigeon.__can_muck(state_dict)
    self.last_state = state_dict

  @__auto_advance
  def __query(self):  # pylint: disable=unused-private-member
    '''Asks fishbait to decide on an action and take it.'''
    action_taken = commander_query(self.commander).to_dict()
    fishbait_seat = commander_fishbait_seat(self.commander)
    self.last_state['last_action'][fishbait_seat] = action_taken

  def __award_pot(self):  # pylint: disable=unused-private-member
    commander_award_pot(self.commander)
    self.__update_state()
