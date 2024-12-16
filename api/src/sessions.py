'''Manages sessions in redis.'''

import pickle

import redis

import settings
from pigeon import Pigeon

session_manager = redis.Redis(
  host='redis', port=6379, db=settings.REDIS_SESSION_DB
)

def get_pigeon(session_id: str) -> Pigeon:
  serialized_revere = session_manager.get(session_id)
  return pickle.loads(serialized_revere)

def set_pigeon(session_id: str, revere: Pigeon):
  serialized_revere = pickle.dumps(revere)
  session_manager.set(
    session_id, serialized_revere, ex=settings.SESSION_TIMEOUT
  )

def does_session_exist(session_id: str) -> bool:
  return bool(session_manager.exists(session_id))

def delete_session(session_id: str):
  session_manager.delete(session_id)
