# Copyright 2022 Marzuk Rashid

'''Definitions for the props of our api routes.'''

from dataclasses import dataclass
from enum import Enum
from typing import Any, Type, Generic, TypeVar
from ctypes import c_uint8

import settings
from error import ValidationError

# ------------------------------------------------------------------------------
# Base Props and Validation Classes --------------------------------------------
# ------------------------------------------------------------------------------

class BaseProps:
  def __init__(self, candidate_obj) -> None:
    pass

T = TypeVar('T')
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
        raise ValidationError(f'{prop} not found in {candidate_obj}')
      try:
        setattr(self, prop, candidate_val)
      except ValidationError as err:
        raise err
      except Exception as exc:
        new_err = ValidationError(
          f'validation failed on {prop} from {candidate_obj}'
        )
        raise new_err from exc

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
  `T` by using `T(obj)` and then assigning that value to the parent object if
  the conversion is successful.
  '''

  def __init__(self, prop_type: Type[T]):
    self.prop_type = prop_type

  def __set_name__(self, owner, name):
    self.name = name

  def __get__(self, obj, obj_type=None) -> T:
    return obj.__dict__.get(self.name)

  def __set__(self, obj, value):
    stamped_value = self.prop_type(value)
    obj.__dict__[self.name] = stamped_value

# ------------------------------------------------------------------------------
# API specific props and types -------------------------------------------------
# ------------------------------------------------------------------------------

ISOCard = c_uint8

class CardSet(list[int | None]):
  '''
  This class represents a set of cards and validates a given object to be a set
  of cards.
  '''

  SET_SIZE: int | None = None

  def __init__(self, requested):
    if not isinstance(requested, list):
      raise ValidationError(f'{requested} is not a list')

    if self.SET_SIZE is None:
      raise TypeError('attempted to create a CardSet with a None SET_SIZE')

    if not len(requested) == self.SET_SIZE:
      raise ValidationError(
        f'{requested} is not the proper length of a {self.__class__.__name__}'
      )

    if not all(isinstance(item, int | None) for item in requested):
      raise ValidationError(f'{requested} is not only ints or null')

    defined_cards = [card for card in requested if isinstance(card, int)]

    if any(
      card < 0 or card >= settings.CARDS_IN_DECK
      for card in defined_cards
    ):
      raise ValidationError(f'{requested} has an invalid card number')

    if len(set(defined_cards)) != len(defined_cards):
      raise ValidationError(f'{requested} has duplicate cards')

    super().__init__(requested)

  def c_arr(self):
    cleaned_list = [c if isinstance(c, int) else 0 for c in self]
    if self.SET_SIZE is None:
      raise TypeError('attemped to use a CardSet with a None SET_SIZE')
    return (ISOCard * self.SET_SIZE)(*cleaned_list)

class Hand(CardSet):
  '''This class represents a Hand and validates a given object to be a Hand.'''

  SET_SIZE: int | None = settings.HAND_CARDS

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
      raise ValidationError(f'{requested} is not only ints or only null')

@propclass
class SetHandProps(BaseProps):
  hand: Prop[Hand] = Prop(Hand)


class ActionType(str, Enum):
  FOLD = 'Fold'
  CHECK = 'Check'
  CALL = 'Call'
  BET = 'Bet'
  ALL_IN = 'All In'

  @classmethod
  def _missing_(cls, value):
    raise ValidationError(f'\'{value}\' is not a valid {cls.__name__}')

class ActionSize(int):
  def __new__(cls, requested):
    if requested is None:
      return super().__new__(cls, 0)
    try:
      return super().__new__(cls, requested)
    except ValueError as err:
      raise ValidationError(f'{err}') from err

@propclass
class ApplyProps(BaseProps):
  action: Prop[ActionType] = Prop(ActionType)
  size: Prop[ActionSize] = Prop(ActionSize)


class Board(CardSet):
  '''This class is a Board and validates a given object to be a Board.'''
  SET_SIZE: int | None = settings.BOARD_CARDS

@propclass
class SetBoardProps(BaseProps):
  board: Prop[Board] = Prop(Board)
