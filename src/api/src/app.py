"""Webserver for the FISHBAIT web interface."""

from datetime import datetime, timezone
from multiprocessing.managers import RemoteError
import re
from typing import Any
import secrets

from flask import Flask, request, make_response
from werkzeug.exceptions import BadRequest
import datadog
from datadog import statsd

from pigeon import PigeonProxy, PigeonInterface
import settings
import error
from error import (
  ApiError, ServerOverloadedError, MissingSessionIdError, UnknownSessionIdError,
  ValidationError
)
from props import (
  SetHandProps, ApplyProps, SetBoardProps, ResetProps, JoinEmailListProps
)
from utils import get_logger

app = Flask(__name__)
log = get_logger(__name__)
datadog.initialize()

def handle_api_error(e: ApiError):
  log.exception(e)
  return e.flask_tuple()

def handle_remote_error(e: RemoteError):
  match = re.search(r'error\.([A-z]*?): ((.|\n)*)\n-{75}', f'{e}')
  if match is None:
    log.error('Could not parse PigeonProxy error')
    raise ApiError() from e
  error_name, error_msg = match.group(1, 2)
  error_type = getattr(error, error_name)
  parsed_error = error_type(error_msg)
  raise parsed_error from e

def handle_bad_request(e: BadRequest):
  raise ValidationError() from e

@app.errorhandler(Exception)
def handle_exceptions(e: Exception):
  error_handler: Any = None
  if isinstance(e, ApiError):
    error_handler = handle_api_error
  elif isinstance(e, RemoteError):
    error_handler = handle_remote_error
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
    if type(e) is type(new_exc):
      log.exception(new_exc)
      rec_er = RecursionError('Could not reduce the encountered error')
      raise rec_er from new_exc
    return handle_exceptions(new_exc)

@app.before_request
def record_api_metric():
  statsd.increment('api.requests', 1, [request.base_url])

# ------------------------------------------------------------------------------
# Session Management -----------------------------------------------------------
# ------------------------------------------------------------------------------

class Session:
  def __init__(self):
    self.updated = datetime.now(timezone.utc).timestamp()
    self.revere = PigeonProxy()

sessions: dict[str, Session] = {}

def session_guard(route):
  def guarded_route():
    session_id = request.cookies.get(settings.SESSION_ID_KEY)
    if session_id is None:
      raise MissingSessionIdError()

    session = sessions.get(session_id)
    if session is None:
      raise UnknownSessionIdError()

    session.updated = datetime.now(timezone.utc).timestamp()
    try:
      return route(session.revere)
    except ApiError:
      # These errors are anticipated and should be passed through
      raise
    except Exception:
      # We should not get any other type of error. If we do, something may have
      # gone horribly wrong and we need to delete this session to preserve the
      # integrity of the server:
      sessions.pop(session_id)
      raise

  guarded_route.__name__ = route.__name__
  return guarded_route

def create_new_session():
  token_candidate = secrets.token_hex(settings.SESSION_ID_BYTES)
  while token_candidate in sessions:
    token_candidate = secrets.token_hex(settings.SESSION_ID_BYTES)
  sessions[token_candidate] = Session()
  return token_candidate

def try_create_new_session():
  if len(sessions) >= settings.MAX_SESSIONS:
    current_time = datetime.now(timezone.utc).timestamp()
    to_remove = None
    for session_id, session in sessions.items():
      if current_time - session.updated >= settings.SESSION_TIMEOUT:
        to_remove = session_id
        break
    if to_remove is not None:
      sessions.pop(to_remove)
      return create_new_session()
    else:
      return None
  else:
    return create_new_session()

# ------------------------------------------------------------------------------
# Routes -----------------------------------------------------------------------
# ------------------------------------------------------------------------------

@app.route('/api', methods=['GET'])
def api_status():
  return 'FISHBAIT API Running'

@app.route('/api/new-session', methods=['GET'])
def new_session():
  new_session_id = try_create_new_session()
  if new_session_id is None:
    raise ServerOverloadedError()
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
  props = JoinEmailListProps(request.get_json())
  with open(settings.EMAIL_LIST_LOCATION, 'a', encoding='utf-8') as email_list:
    email_list.write(f'{props.email}\n')
  return make_response()
