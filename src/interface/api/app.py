# Copyright 2021 Marzuk Rashid

"""Webserver for the FISHBAIT web interface."""

from flask import Flask, request, abort
import configparser

config = configparser.ConfigParser()
config.read('api/config.ini')

app = Flask(__name__)

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
  return authenticator

@app.route('/api', methods=['GET'])
def api_status():
  return 'FISHBAIT API Running'

@app.route('/api/game_status', methods=['GET'])
@authenticate
def game_status():
  return 'No game in progress'

if __name__ == '__main__':
  app.run(debug=True)
