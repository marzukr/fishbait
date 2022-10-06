# Copyright 2021 Marzuk Rashid

"""Webserver for the FISHBAIT web interface."""

from datetime import datetime, timezone
from multiprocessing.managers import RemoteError
import re
from typing import Any
import logging

from flask import Flask, request, make_response
from werkzeug.exceptions import BadRequest

from pigeon import PigeonInterface
import settings
from depot_types import (
  depot_server, TryCreateNewSession, GetSession, SetSession
)
import error
from error import (
  ApiError,
  ServerOverloadedError,
  MissingSessionIdError,
  UnknownSessionIdError,
  InvalidEmailError,
  ValidationError,
)
from props import SetHandProps, ApplyProps, SetBoardProps, ResetProps

app = Flask(__name__)
depot_server.connect()
log = logging.getLogger(__name__)

def handle_api_error(e: ApiError):
  log.exception(e)
  return e.flask_tuple()

def handle_remote_error(e: RemoteError):
  match = re.search(r'error\.([A-z]*?): ((.|\n)*)\n-{75}', f'{e}')
  if match is None:
    raise ApiError('Could not parse depot error') from e
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

@app.route('/api', methods=['GET'])
def api_status():
  return 'FISHBAIT API Running'

@app.route('/api/new-session', methods=['GET'])
def new_session():
  new_session_id = TryCreateNewSession.send()
  if new_session_id is None:
    raise ServerOverloadedError()
  resp = make_response()
  resp.set_cookie(settings.SESSION_ID_KEY, new_session_id)
  return resp

def session_guard(route):
  def guarded_route():
    session_id = request.cookies.get(settings.SESSION_ID_KEY)
    if session_id is None:
      raise MissingSessionIdError()

    session = GetSession.send(session_id)
    if session is None:
      raise UnknownSessionIdError()

    session.updated = datetime.now(timezone.utc).timestamp()
    route_result = route(session.revere)
    SetSession.send(session_id, session)
    return route_result

  guarded_route.__name__ = route.__name__
  return guarded_route

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
  data = request.get_json()
  email = data['email']
  if len(email) > 254 or '\n' in email:
    raise InvalidEmailError()
  with open(settings.EMAIL_LIST_LOCATION, 'a', encoding='utf-8') as email_list:
    email_list.write(f'{email}\n')
  return make_response()
