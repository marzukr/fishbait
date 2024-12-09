"""Error types for the Fishbait API"""

from typing import Optional

import settings

class ApiError(Exception):
  '''
  Base class and default fishbait API error.

  Can be constructed from a message string like `Exception` and also has the
  `flask_tuple` method to return a tuple that can be returned from a flask
  controller to send the error properly to the client.
  '''

  message: str = 'The fishbait API encountered an error.'
  status_code: int = 500
  response_headers: dict = {}

  def __init__(self, message: Optional[str] = None) -> None:
    # We don't override self.message or __class__.message to ensure the default
    # error message is always preserved and we never expose confidential
    # internal details externally
    exc_message = message or self.__class__.message
    super().__init__(exc_message)

  @classmethod
  def flask_tuple(cls):
    # Using cls ensures we are always returning the generic default error
    # message to flask to avoid exposing confidential internal details to
    # potential attackers:
    return ({
      'error_message': cls.message,
      'error_code': cls.__name__,
    }, cls.status_code, cls.response_headers)

class ServerOverloadedError(ApiError):
  message = (
    'Our servers are currently overloaded because we are in beta and our '
    'capacity is limited. Please try again in 30 minutes.'
  )
  status_code = 503
  response_headers = {
    'Retry-After': settings.SESSION_TIMEOUT,
  }

class MissingSessionIdError(ApiError):
  message = 'A session ID was not provided.'
  status_code = 400

class UnknownSessionIdError(ApiError):
  message = 'The provided session id was not found.'
  status_code = 404

class ValidationError(ApiError):
  message = 'The given input was not valid'
  status_code = 400

class InvalidStateTransitionError(ApiError):
  message = 'The requested operation is invalid in the current game state'
  status_code = 400

class CPlusPlusError(Exception):
  pass
