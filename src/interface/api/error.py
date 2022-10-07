# Copyright 2021 Marzuk Rashid

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
    self.message = message or self.__class__.message
    super().__init__(self.message)

  def flask_tuple(self):
    return ({
      'error_message': self.message,
      'error_code': self.__class__.__name__,
    }, self.__class__.status_code, self.response_headers)

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
