"""Webserver for the FISHBAIT web interface."""

from typing import Any
import os

from flask import Flask, request, make_response
from werkzeug.exceptions import BadRequest
import datadog
from datadog import statsd
import redis

from pigeon import PigeonInterface
import settings
from error import (
  ApiError, ServerOverloadedError, MissingSessionIdError, UnknownSessionIdError,
  ValidationError
)
from props import (
  SetHandProps, ApplyProps, SetBoardProps, ResetProps
)
from utils import get_logger
import tasks
import sessions

app = Flask(__name__)
log = get_logger(__name__)
if os.getenv('DD_SERVICE'):
  datadog.initialize()

def handle_api_error(e: ApiError):
  return e.flask_tuple()

def handle_bad_request(e: BadRequest):
  raise ValidationError() from e

@app.errorhandler(Exception)
def handle_exceptions(e: Exception):
  error_handler: Any = None
  if isinstance(e, ApiError):
    error_handler = handle_api_error
  elif isinstance(e, BadRequest):
    error_handler = handle_bad_request
  else:
    log.exception(e)
    return ApiError().flask_tuple()

  try:
    result = error_handler(e)
    log.exception(e)
    return result
  except Exception as new_exc:  # pylint: disable=broad-except
    log.exception('Exception while handling exception: %s', new_exc)
    return ApiError().flask_tuple()

@app.before_request
def record_api_metric():
  statsd.increment('api.requests', 1, [request.base_url])

# ------------------------------------------------------------------------------
# Session Management -----------------------------------------------------------
# ------------------------------------------------------------------------------

def session_guard(route):
  def guarded_route():
    session_id = request.cookies.get(settings.SESSION_ID_KEY)
    if session_id is None:
      raise MissingSessionIdError()

    if not sessions.does_session_exist(session_id):
      raise UnknownSessionIdError()

    revere = tasks.PigeonProxy(session_id)
    try:
      return route(revere)
    except ApiError:
      # These errors are anticipated and should be passed through
      raise
    except Exception:
      # We should not get any other type of error. If we do, something may have
      # gone horribly wrong and we need to delete this session to preserve the
      # integrity of the server:
      sessions.delete_session(session_id)
      raise

  guarded_route.__name__ = route.__name__
  return guarded_route

# ------------------------------------------------------------------------------
# Routes -----------------------------------------------------------------------
# ------------------------------------------------------------------------------

@app.route('/api', methods=['GET'])
def api_status():
  return 'FISHBAIT API Running'

@app.route('/api/new-session', methods=['GET'])
def new_session():
  try:
    new_session_id = tasks.create_new_session.delay().get(
      timeout=settings.PIGEON_EXECUTION_TIMEOUT
    )
  except redis.exceptions.OutOfMemoryError as e:
    raise ServerOverloadedError() from e

  resp = make_response()
  resp.set_cookie(settings.SESSION_ID_KEY, new_session_id)
  return resp

@app.route('/api/state', methods=['GET'])
@session_guard
def state(revere: PigeonInterface):
  return revere.state_dict()

@app.route('/api/set-hand', methods=['POST'])
@session_guard
def set_hand(revere: PigeonInterface):
  hand = SetHandProps(request.get_json()).hand
  revere.set_hand(hand)
  return revere.state_dict()

@app.route('/api/apply', methods=['POST'])
@session_guard
def apply(revere: PigeonInterface):
  action = ApplyProps(request.get_json())
  revere.apply(action.action, action.size)
  return revere.state_dict()

@app.route('/api/set-board', methods=['POST'])
@session_guard
def set_board(revere: PigeonInterface):
  board = SetBoardProps(request.get_json()).board
  revere.set_board(board)
  return revere.state_dict()

@app.route('/api/new-hand', methods=['POST'])
@session_guard
def new_hand(revere: PigeonInterface):
  revere.new_hand()
  return revere.state_dict()

@app.route('/api/reset', methods=['POST'])
@session_guard
def reset(revere: PigeonInterface):
  reset_props = ResetProps(request.get_json())
  revere.reset(
    reset_props.stack, reset_props.button, reset_props.big_blind,
    reset_props.small_blind, reset_props.fishbait_seat,
    reset_props.player_names
  )
  return revere.state_dict()

@app.route('/api/join-email-list', methods=['POST'])
def join_email_list():
  return make_response()
