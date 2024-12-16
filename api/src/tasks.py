'''Defines celery tasks for the api.'''

import secrets

from celery import Celery

import settings
from pigeon import Pigeon, PigeonInterface
from sessions import get_pigeon, set_pigeon

app = Celery(
  'tasks',
  broker=f'redis://redis:6379/{settings.REDIS_CELERY_DB}',
  backend=f'redis://redis:6379/{settings.REDIS_CELERY_DB}'
)
app.conf.update(
  task_serializer='pickle',
  result_serializer='pickle',
  accept_content=['pickle'],
)

@app.task
def create_new_session():
  session_id = secrets.token_hex(settings.SESSION_ID_BYTES)
  revere = Pigeon()
  set_pigeon(session_id, revere)
  return session_id

@app.task
def run_pigeon(session_id: str, message: str, *args, **kwargs):
  revere = get_pigeon(session_id)
  fn = getattr(revere, message)
  result = fn(*args, **kwargs)
  set_pigeon(session_id, revere)
  return result

class PigeonProxy(PigeonInterface):
  '''
  An object that behaves like a local Pigeon to an outside observer but runs the
  Pigeon's methods in celery tasks and saves the Pigeon's state in redis.
  '''

  def __init__(self, session_id: str) -> None:
    super().__init__()
    self.session_id = session_id

  class PigeonMessage():
    '''
    A descriptor that applies the given function on the managed Pigeon
    '''

    def __set_name__(self, owner, name):
      self.name = name

    def __get__(self, obj, obj_type):
      def wrapped_fn(*args, **kwargs):
        return run_pigeon.delay(
          obj.session_id, self.name, *args, **kwargs
        ).get(timeout=settings.PIGEON_EXECUTION_TIMEOUT)
      return wrapped_fn

  reset = PigeonMessage()
  set_hand = PigeonMessage()
  apply = PigeonMessage()
  set_board = PigeonMessage()
  state_dict = PigeonMessage()
  new_hand = PigeonMessage()
