# Copyright 2022 Marzuk Rashid

'''Definitions for the props of our api routes.'''

from dataclasses import dataclass
from enum import Enum
from typing import Type, Generic, TypeVar

import settings
from error import ValidationError

# ------------------------------------------------------------------------------
# Base Props and Validation Classes --------------------------------------------
# ------------------------------------------------------------------------------

class BaseProps:
  def __init__(self, candidate_obj) -> None:
    pass

def propclass(custom_props):
  '''
  Takes an arbitrary object and attempts to convert it to our type. We do this
  by checking that each property in our type is defined in the arbitrary object,
  and then copying the value over if so.
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

  dataclass_props = dataclass(custom_props)
  dataclass_props.__init__ = init_fn
  return dataclass_props


T = TypeVar('T')
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

class Hand(list[int]):
  '''This class represents a Hand and validates a given object to be a Hand.'''

  def __init__(self, requested):
    if not isinstance(requested, list):
      raise ValidationError(f'{requested} is not a list')

    if not len(requested) == settings.HAND_CARDS:
      raise ValidationError(f'{requested} is not the proper length of a hand')

    if all(it is None for it in requested):
      # If all cards are None, we consider this hand mucked. We then init a Hand
      # with all cards set to 0 because this is how the C++ code represents
      # mucked cards.
      super().__init__([0] * settings.HAND_CARDS)
      return

    if any(not isinstance(item, int) for item in requested):
      raise ValidationError(f'{requested} is not only ints or only null')

    if any(
      item < 0 or item >= settings.CARDS_IN_DECK
      for item in requested
    ):
      raise ValidationError(f'{requested} has an invalid card number')

    if len(set(requested)) != len(requested):
      raise ValidationError(f'{requested} has duplicate cards')

    super().__init__(requested)

# Note: with this pattern, all props must be typed as `Prop` so the dataclass
# picks them up:
@propclass
class SetHandProps(BaseProps):
  hand: Prop = Prop(Hand)


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
class ActionProps(BaseProps):
  action: Prop = Prop(ActionType)
  size: Prop = Prop(ActionSize)
