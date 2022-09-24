# Copyright 2021 Marzuk Rashid

"""Webserver for the FISHBAIT web interface."""

import secrets
from datetime import datetime, timezone
from typing import Dict

from flask import Flask, request, make_response

from pigeon import Pigeon, Action
import settings
from error import (
  ServerOverloadedError,
  MissingSessionIdError,
  UnknownSessionIdError,
  InvalidEmailError,
)

app = Flask(__name__)

class Session:
  def __init__(self, updated: float, revere: Pigeon):
    self.updated = updated
    self.revere = revere

sessions: Dict[str, Session] = {}

@app.route('/api', methods=['GET'])
def api_status():
  return 'FISHBAIT API Running'

def create_new_session():
  token_candidate = secrets.token_hex(settings.SESSION_ID_BYTES)
  while token_candidate in sessions:
    token_candidate = secrets.token_hex(settings.SESSION_ID_BYTES)
  updated_time = datetime.now(timezone.utc).timestamp()
  new_revere = Pigeon()
  sessions[token_candidate] = Session(updated_time, new_revere)
  resp = make_response()
  resp.set_cookie(settings.SESSION_ID_KEY, token_candidate)
  return resp

@app.route('/api/new-session', methods=['GET'])
def new_session():
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
      return ServerOverloadedError.flask_tuple()
  else:
    return create_new_session()

def session_guard(route):
  def guarded_route():
    session_id = request.cookies.get(settings.SESSION_ID_KEY)
    if session_id is None:
      return MissingSessionIdError.flask_tuple()
    elif session_id not in sessions:
      return UnknownSessionIdError.flask_tuple()
    sessions[session_id].updated = datetime.now(timezone.utc).timestamp()
    return route(sessions[session_id].revere)
  guarded_route.__name__ = route.__name__
  return guarded_route

@app.route('/api/state', methods=['GET'])
@session_guard
def state(revere: Pigeon):
  return revere.state_dict()

@app.route('/api/set-hand', methods=['POST'])
@session_guard
def set_hand(revere: Pigeon):
  data = request.get_json()
  revere.set_hand(data['hand'])
  return revere.state_dict()

@app.route('/api/apply', methods=['POST'])
@session_guard
def apply(revere: Pigeon):
  data = request.get_json()
  def parse_size(data):
    if 'size' in data:
      if data['size'] is not None:
        return data['size']
    return 0
  size = parse_size(data)
  revere.apply(Action(data['action']), size)
  return revere.state_dict()

@app.route('/api/set-board', methods=['POST'])
@session_guard
def set_board(revere: Pigeon):
  data = request.get_json()
  revere.set_board(data['board'])
  return revere.state_dict()

@app.route('/api/new-hand', methods=['POST'])
@session_guard
def new_hand(revere: Pigeon):
  revere.new_hand()
  return revere.state_dict()

@app.route('/api/reset', methods=['POST'])
@session_guard
def reset(revere: Pigeon):
  data = request.get_json()
  revere.reset(data['stack'], data['button'], data['big_blind'],
               data['small_blind'], data['fishbait_seat'], data['player_names'])
  return revere.state_dict()

@app.route('/api/join-email-list', methods=['POST'])
def join_email_list():
  data = request.get_json()
  email = data['email']
  if len(email) > 254 or '\n' in email:
    return InvalidEmailError.flask_tuple()
  with open(settings.EMAIL_LIST_LOCATION, 'a', encoding='utf-8') as email_list:
    email_list.write(f'{email}\n')
  return make_response()

if __name__ == '__main__':
  app.run(debug=True)
