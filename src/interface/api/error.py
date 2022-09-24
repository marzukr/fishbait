# Copyright 2021 Marzuk Rashid

"""Error types for the Fishbait API"""

import settings

class ApiError(Exception):
  message: str = 'The fishbait API encountered an error.'
  status_code: int = 500
  response_headers: dict = {}

  @classmethod
  def flask_tuple(cls):
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

class InvalidEmailError(ApiError):
  message = 'The provided email is invalid.'
  status_code = 400
