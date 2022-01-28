# Copyright 2021 Marzuk Rashid

"""Webserver for the FISHBAIT web interface."""

from flask import Flask, request, abort
import configparser
import os
import sys
import json
api_dir = os.path.dirname(__file__)
relay_dir = os.path.join(api_dir, '../../relay')
sys.path.append(relay_dir)
from pigeon import Pigeon  # pylint: disable=wrong-import-position

config = configparser.ConfigParser()
config.read('api/config.ini')

app = Flask(__name__)

strategy_loc = config.get('DEFAULT', 'STRATEGY_LOCATION', fallback=None)
if strategy_loc is not None:
  revere = Pigeon(strategy_loc)
else:
  raise KeyError('STRATEGY_LOCATION not defined in config.ini')

def authenticate(route):
  def authenticator():
    auth_token = request.headers.get('Authorization')
    if not auth_token:
      abort(401)
    if auth_token[:8] != 'Bearer: ':
      abort(401)
    if auth_token[8:] != config.get('DEFAULT', 'SECRET_KEY', fallback=None):
      abort(401)
    return route()
  authenticator.__name__ = route.__name__
  return authenticator

@app.route('/api', methods=['GET'])
def api_status():
  return 'FISHBAIT API Running'

@app.route('/api/state', methods=['GET'])
@authenticate
def state():
  return json.dumps(revere.state())

@app.route('/api/set_hand', methods=['POST'])
@authenticate
def set_hand():
  data = request.get_json()
  revere.set_hand(data['hand'])
  return json.dumps(revere.state())

@app.route('/api/apply', methods=['POST'])
@authenticate
def apply():
  data = request.get_json()
  def parse_size(data):
    if 'size' in data:
      if data['size'] is not None:
        return data['size']
    return 0
  size = parse_size(data)
  revere.apply(data['action'], size)
  return json.dumps(revere.state())

@app.route('/api/set_board', methods=['POST'])
@authenticate
def set_board():
  data = request.get_json()
  revere.set_board(data['board'])
  return json.dumps(revere.state())

@app.route('/api/new_hand', methods=['POST'])
@authenticate
def new_hand():
  revere.new_hand()
  return json.dumps(revere.state())

@app.route('/api/reset', methods=['POST'])
@authenticate
def reset():
  data = request.get_json()
  revere.reset(data['stack'], data['button'], data['big_blind'],
               data['small_blind'], data['fishbait_seat'], data['player_names'])
  return json.dumps(revere.state())

if __name__ == '__main__':
  app.run(debug=True)
