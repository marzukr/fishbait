# Copyright 2021 Marzuk Rashid

"""Webserver for the FISHBAIT web interface."""

from flask import Flask, request
import configparser
import os
import sys
import secrets
from datetime import datetime, timezone
from typing import Dict
api_dir = os.path.dirname(__file__)
relay_dir = os.path.join(api_dir, '../../relay')
sys.path.append(relay_dir)
from pigeon import Pigeon  # pylint: disable=wrong-import-position

config = configparser.ConfigParser()
config.read('api/config.ini')

app = Flask(__name__)

strategy_loc = config.get('DEFAULT', 'STRATEGY_LOCATION', fallback=None)
if strategy_loc is None:
  raise KeyError('STRATEGY_LOCATION not defined in config.ini')

class Session:
  def __init__(self, updated: float, revere: Pigeon):
    self.updated = updated
    self.revere = revere

sessions: Dict[str, Session] = {}
MAX_SESSIONS = 1000
SESSION_ID_BYTES = 8
SESSION_TIMEOUT = 1800  # in seconds

@app.route('/api', methods=['GET'])
def api_status():
  return 'FISHBAIT API Running'

def create_new_session() -> Dict[str, str]:
  token_candidate = secrets.token_hex(SESSION_ID_BYTES)
  while token_candidate in sessions:
    token_candidate = secrets.token_hex(SESSION_ID_BYTES)
  updated_time = datetime.now(timezone.utc).timestamp()
  new_revere = Pigeon(strategy_loc)
  sessions[token_candidate] = Session(updated_time, new_revere)
  return {
    'new_session_id': token_candidate
  }

@app.route('/api/new_session', methods=['GET'])
def new_session():
  if len(sessions) >= MAX_SESSIONS:
    current_time = datetime.now(timezone.utc).timestamp()
    to_remove = None
    for session_id, session in sessions.items():
      if current_time - session.updated >= SESSION_TIMEOUT:
        to_remove = session_id
        break
    if to_remove is not None:
      sessions.pop(to_remove)
      return create_new_session()
    else:
      return ({
        'error_message': ('Our servers are currently overloaded because we are '
                          'in beta and our capacity is limited. Please try '
                          'again in 30 minutes.'),
        'forseen_error_code': 1,
      }, 503, {
        'Retry-After': SESSION_TIMEOUT,
      })
  else:
    return create_new_session()

def session_guard(route):
  def guarded_route():
    session_id = request.args.get('session_id')
    if session_id is None:
      return ({
        'error_message': 'A session ID was not provided.',
        'forseen_error_code': 2,
      }, 400)
    elif session_id not in sessions:
      return ({
        'error_message': 'The provided session id was not found.',
        'forseen_error_code': 3,
      }, 404)
    sessions[session_id].updated = datetime.now(timezone.utc).timestamp()
    return route(sessions[session_id].revere)
  guarded_route.__name__ = route.__name__
  return guarded_route

@app.route('/api/state', methods=['GET'])
@session_guard
def state(revere: Pigeon):
  return revere.state()

@app.route('/api/set_hand', methods=['POST'])
@session_guard
def set_hand(revere: Pigeon):
  data = request.get_json()
  revere.set_hand(data['hand'])
  return revere.state()

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
  revere.apply(data['action'], size)
  return revere.state()

@app.route('/api/set_board', methods=['POST'])
@session_guard
def set_board(revere: Pigeon):
  data = request.get_json()
  revere.set_board(data['board'])
  return revere.state()

@app.route('/api/new_hand', methods=['POST'])
@session_guard
def new_hand(revere: Pigeon):
  revere.new_hand()
  return revere.state()

@app.route('/api/reset', methods=['POST'])
@session_guard
def reset(revere: Pigeon):
  data = request.get_json()
  revere.reset(data['stack'], data['button'], data['big_blind'],
               data['small_blind'], data['fishbait_seat'], data['player_names'])
  return revere.state()

if __name__ == '__main__':
  app.run(debug=True)
