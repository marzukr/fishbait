# Copyright 2021 Marzuk Rashid

"""Webserver for the FISHBAIT web interface."""

from datetime import datetime, timezone

from flask import Flask, request, make_response

from pigeon import PigeonInterface, Action
import settings
from depot_types import (
  depot_server, TryCreateNewSession, GetSession, SetSession
)
from error import (
  ServerOverloadedError,
  MissingSessionIdError,
  UnknownSessionIdError,
  InvalidEmailError,
)

app = Flask(__name__)
depot_server.connect()

@app.route('/api', methods=['GET'])
def api_status():
  return 'FISHBAIT API Running'

@app.route('/api/new-session', methods=['GET'])
def new_session():
  new_session_id = TryCreateNewSession.send()
  if new_session_id is None:
    return ServerOverloadedError.flask_tuple()
  resp = make_response()
  resp.set_cookie(settings.SESSION_ID_KEY, new_session_id)
  return resp

def session_guard(route):
  def guarded_route():
    session_id = request.cookies.get(settings.SESSION_ID_KEY)
    if session_id is None:
      return MissingSessionIdError.flask_tuple()

    session = GetSession.send(session_id)
    if session is None:
      return UnknownSessionIdError.flask_tuple()

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
  data = request.get_json()
  revere.set_hand(data['hand'])
  return revere.state_dict()

@app.route('/api/apply', methods=['POST'])
@session_guard
def apply(revere: PigeonInterface):
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
def set_board(revere: PigeonInterface):
  data = request.get_json()
  revere.set_board(data['board'])
  return revere.state_dict()

@app.route('/api/new-hand', methods=['POST'])
@session_guard
def new_hand(revere: PigeonInterface):
  revere.new_hand()
  return revere.state_dict()

@app.route('/api/reset', methods=['POST'])
@session_guard
def reset(revere: PigeonInterface):
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
