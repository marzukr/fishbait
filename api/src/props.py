'''Definitions for the props of our api routes.'''

from dataclasses import dataclass
from enum import Enum
from typing import Any, Type, Generic, TypeVar, Callable
from ctypes import _SimpleCData, Array
from collections.abc import Iterable

import settings
from error import ValidationError
from utils import get_logger

log = get_logger(__name__)

# ------------------------------------------------------------------------------
# Type Vars --------------------------------------------------------------------
# ------------------------------------------------------------------------------

T = TypeVar('T')
U = TypeVar('U')
C = TypeVar('C', bound=_SimpleCData)

# ------------------------------------------------------------------------------
# Base Props and Validation Classes --------------------------------------------
# ------------------------------------------------------------------------------

class BaseProps:
  def __init__(self, candidate_obj) -> None:
    pass

def propclass(custom_props: Type[T]) -> Type[T]:
  '''
  Takes an arbitrary object and attempts to convert it to our type. We do this
  by checking that each property in our type is defined in the arbitrary object,
  and then copying the value over if so.

  This is also a dataclass. Note that with this pattern, all props must be typed
  as `Prop` so the dataclass picks them up.
  '''

  if not issubclass(custom_props, BaseProps):
    raise TypeError('propclasses must be derived from BaseProps')

  def init_fn(self, candidate_obj):
    vars_dict = vars(self) | vars(type(self))
    properties = {p for p in vars_dict if not p.startswith('_')}
    is_dict = isinstance(candidate_obj, dict)
    not_found_err = object()
    for prop in properties:
      candidate_val = getattr(candidate_obj, prop, not_found_err)
      if candidate_val == not_found_err and is_dict:
        candidate_val = candidate_obj.get(prop, not_found_err)
      if candidate_val == not_found_err:
        candidate_val = None
      setattr(self, prop, candidate_val)

  # Note: We need set the __init__ method manually here instead of putting it in
  # BaseProps so that we override the __init__ set by dataclass. We need
  # dataclass so that we can convert any props objects to dicts with asdict and
  # put props objects inside other dataclasses (such as PigeonState)
  dataclass_props: Any = dataclass(custom_props)
  dataclass_props.__init__ = init_fn
  return dataclass_props

class Prop(Generic[T]):
  '''
  A descriptor that takes an arbitrary object and attempts to convert it to a
  `T` by using `prop_type(obj)` and then assigning that value to the parent
  object if the conversion is successful.
  '''

  def __init__(self, prop_type: Callable[[Any], T]):
    self.prop_ctr = prop_type

  def __set_name__(self, owner, name):
    self.name = name

  def __get__(self, obj, obj_type=None) -> T:
    return obj.__dict__[self.name]

  def __set__(self, obj, value):
    stamped_value = self.prop_ctr(value)
    obj.__dict__[self.name] = stamped_value


class ValidationUnion(Generic[T, U]):
  '''Validates to a T or U'''

  def __init__(self, t_ctr: Callable[[Any], T], u_ctr: Callable[[Any], U]):
    self.t_ctr = t_ctr
    self.u_ctr = u_ctr

  def __call__(self, requested) -> T | U:
    try:
      return self.t_ctr(requested)
    except ValidationError:
      pass
    try:
      return self.u_ctr(requested)
    except ValidationError:
      pass
    raise ValidationError()

def validation_none(requested):
  if requested is not None:
    raise ValidationError()

class ValidationOptional(Generic[T], ValidationUnion[T, None]):
  def __init__(self, t_ctr: Callable[[Any], T]):
    super().__init__(t_ctr, validation_none)


class TypedList(Generic[T], list[T]):
  '''
  Represents a list of a specific type. Subclasses can also specify a specific
  length.
  '''

  LIST_LEN: int | None = None
  '''The required length of the list or None if any length is ok'''

  LIST_TYPE: Type[T]
  '''
  The type of this typed List. If undefined, the constructor will fail. This is
  because typed list should not be directly constructed. One should subclass it.
  '''

  def __init__(self, requested):
    if not isinstance(requested, Iterable):
      raise ValidationError()
    processed = [
      i if isinstance(i, self.LIST_TYPE) else self.LIST_TYPE(i)
      for i in requested
    ]
    super().__init__(processed)

    if self.LIST_LEN is None:
      return

    if not len(self) == self.LIST_LEN:
      raise ValidationError()

  def c_clean(self) -> list:
    return self

  def c_arr(self, c_type: Type[C]) -> Array[C]:
    cleaned = self.c_clean()
    if self.LIST_LEN is None:
      raise TypeError('attemped to use c_arr with a None LIST_LEN')
    if self.LIST_LEN == 0:
      raise TypeError('attemped to use c_arr with a 0 LIST_LEN')
    return (c_type * self.LIST_LEN)(*cleaned)

class ValidationInt(int):
  '''An integer such that MINIMUM <= self < MAXIMUM'''

  MINIMUM: int | None = None
  MAXIMUM: int | None = None

  def __new__(cls, requested):
    try:
      self = super().__new__(cls, requested)
    except (ValueError, TypeError) as err:
      raise ValidationError() from err
    if cls.MINIMUM is not None and self < cls.MINIMUM:
      raise ValidationError()
    if cls.MAXIMUM is not None and self >= cls.MAXIMUM:
      raise ValidationError()
    return self

class ValidationFloat(float):
  def __new__(cls, requested):
    try:
      self = super().__new__(cls, requested)
    except (ValueError, TypeError) as err:
      raise ValidationError() from err
    return self

# ------------------------------------------------------------------------------
# API specific types -----------------------------------------------------------
# ------------------------------------------------------------------------------

class CardSet(TypedList[int | None]):
  '''
  This class represents a set of cards and validates a given object to be a set
  of cards.
  '''

  LIST_LEN: int | None = None
  '''
  Default to None but if we try to construct with this set to None we will
  throw. This is because CardSets should not be constructed directly. They
  should be subclassed with LIST_LEN defined.
  '''

  LIST_TYPE = int | None

  def __init__(self, requested):
    super().__init__(requested)

    # If the super init doesn't throw, we know requested is a valid list of int
    # or None of unknown length. We now want to validate that it is a valid Card
    # set:
    if self.LIST_LEN is None:
      raise TypeError('attempted to create a CardSet with a None LIST_LEN')

    # LIST_LEN is not None and super init didn't throw implies requested has
    # length LIST_LEN where LIST_LEN is an int >= 0

    defined_cards = [card for card in requested if isinstance(card, int)]

    if any(
      card < 0 or card >= settings.CARDS_IN_DECK
      for card in defined_cards
    ):
      raise ValidationError()

    if len(set(defined_cards)) != len(defined_cards):
      raise ValidationError()

  def c_clean(self) -> list:
    return [c if isinstance(c, int) else 0 for c in self]

class Hand(CardSet):
  '''This class represents a Hand and validates a given object to be a Hand.'''

  LIST_LEN: int | None = settings.HAND_CARDS

  def __init__(self, requested):
    super().__init__(requested)

    # If the super init doesn't throw, we know we have a list of 2 unique valid
    # cards where some or all elements may also be None. We want to narrow this
    # to confirm that either all the elements are None or all the elements are
    # ints to verify that this is a valid hand:
    if not (
      all(it is None for it in requested)
      or all(isinstance(it, int) for it in requested)
    ):
      raise ValidationError()

class Board(CardSet):
  '''This class is a Board and validates a given object to be a Board.'''
  LIST_LEN: int | None = settings.BOARD_CARDS


class ChipCount(ValidationInt):
  MINIMUM: int | None = 0

class PlayerNumber(ValidationInt):
  MINIMUM: int | None = 0
  MAXIMUM: int | None = settings.PLAYERS


class ActionType(str, Enum):
  FOLD = 'Fold'
  CHECK = 'Check'
  CALL = 'Call'
  BET = 'Bet'
  ALL_IN = 'All In'

  @classmethod
  def _missing_(cls, value):
    raise ValidationError()

class ActionSize(ChipCount):
  def __new__(cls, requested):
    if requested is None:
      return super().__new__(cls, 0)
    return super().__new__(cls, requested)

class ValidationSequenceId(ValidationInt):
  MINIMUM: int | None = 0


class ChipPlayerList(TypedList[ChipCount]):
  LIST_LEN = settings.PLAYERS
  LIST_TYPE = ChipCount

class StrPlayerList(TypedList[str]):
  LIST_LEN = settings.PLAYERS
  LIST_TYPE = str


class Email(str):
  MAX_EMAIL_LEN = 254

  def __new__(cls, requested):
    self = super().__new__(cls, requested)
    if len(self) > self.MAX_EMAIL_LEN or '\n' in self:
      raise ValidationError()
    return self

# ------------------------------------------------------------------------------
# API Props --------------------------------------------------------------------
# ------------------------------------------------------------------------------

@propclass
class SetHandProps(BaseProps):
  hand: Prop[Hand] = Prop(Hand)

@propclass
class ApplyProps(BaseProps):
  action: Prop[ActionType] = Prop(ActionType)
  size: Prop[ActionSize] = Prop(ActionSize)
  action_idx: Prop[ValidationSequenceId | None] = (
    Prop(ValidationOptional(ValidationSequenceId))
  )

@propclass
class AvailableActionProps(BaseProps):
  action: Prop[ActionType] = Prop(ActionType)
  size: Prop[ValidationFloat] = Prop(ValidationFloat)
  policy: Prop[ValidationFloat] = Prop(ValidationFloat)
  action_idx: Prop[ValidationSequenceId] = Prop(ValidationSequenceId)

@propclass
class SetBoardProps(BaseProps):
  board: Prop[Board] = Prop(Board)

@propclass
class ResetProps(BaseProps):
  stack: Prop[ChipPlayerList] = Prop(ChipPlayerList)
  button: Prop[PlayerNumber] = Prop(PlayerNumber)
  big_blind: Prop[ChipCount] = Prop(ChipCount)
  small_blind: Prop[ChipCount] = Prop(ChipCount)
  fishbait_seat: Prop[PlayerNumber] = Prop(PlayerNumber)
  player_names: Prop[StrPlayerList] = Prop(StrPlayerList)
