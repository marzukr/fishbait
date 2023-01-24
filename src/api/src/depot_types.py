'''Types used by the depot'''

from abc import ABC, abstractmethod
import secrets
from datetime import datetime, timezone
from typing import Dict, List, Optional, Type, TypeVar, Generic, ParamSpec
from multiprocessing.managers import BaseManager
import logging

from pigeon import Pigeon, PigeonInterface
import settings

log = logging.getLogger(__name__)


class Depot(BaseManager):
  pass

depot_server = Depot(
  address=('', settings.DEPOT_PORT),
  authkey=settings.DEPOT_PASSWORD
)


T = TypeVar('T')
P = ParamSpec('P')
class DepotMessage(Generic[P, T], ABC):
  '''Base class for a message type that is sendable to the depot'''

  @staticmethod
  @abstractmethod
  def execute(*args: P.args, **kwargs: P.kwargs) -> T:
    raise NotImplementedError()

  @classmethod
  def send(cls, *args: P.args, **kwargs: P.kwargs):
    executor = getattr(depot_server, cls.__name__)
    proxy = executor(*args, **kwargs)
    proxy_val: T = getattr(proxy, '_getvalue')()
    return proxy_val

  @classmethod
  def register(cls):
    Depot.register(cls.__name__, cls.execute)


class Session:
  def __init__(self, updated: float, revere: PigeonInterface):
    self.updated = updated
    self.revere = revere

sessions: Dict[str, Session] = {}

class PigeonProxy(PigeonInterface):
  '''
  An object that behaves like a local Pigeon to an outside observer but modifies
  the Pigeons stored in depot.
  '''

  def __init__(self, session_id: str) -> None:
    super().__init__()
    self.session_id = session_id

  class PigeonMessage(DepotMessage):
    '''
    A descriptor that applies the given function on the appropriate Pigeon
    object
    '''

    def __set_name__(self, owner, name):
      self.name = name

    @staticmethod
    def execute(session_id: str, fn_name: str, *args, **kwargs):
      revere = sessions[session_id].revere
      fn = getattr(revere, fn_name)
      try:
        result = fn(*args, **kwargs)
      except Exception as e:
        # We should not get an error from Pigeon. If we do, something has gone
        # horribly wrong and we need to delete this session to preserve the
        # integrity of the server:
        sessions.pop(session_id)
        log.exception(e)
        raise
      return result

    def __get__(self, obj, objtype=None):
      def wrapped_send(*args, **kwargs):
        return self.send(obj.session_id, self.name, *args, **kwargs)
      return wrapped_send

  reset = PigeonMessage()
  set_hand = PigeonMessage()
  apply = PigeonMessage()
  set_board = PigeonMessage()
  state_dict = PigeonMessage()
  new_hand = PigeonMessage()

class SessionProxy(Session):
  '''An object that's just like a session but it uses a PigeonProxy'''

  revere: PigeonProxy

  def __init__(self, updated: float, revere: PigeonProxy):
    super().__init__(updated, revere)

  @classmethod
  def from_session_id(cls, session_id: str):
    session = sessions.get(session_id)
    if session is None:
      return None
    revere_prx = PigeonProxy(session_id)
    return cls(session.updated, revere_prx)

  def to_session(self):
    return Session(self.updated, sessions[self.revere.session_id].revere)


class GetSession(DepotMessage[[str], Optional[SessionProxy]]):
  @staticmethod
  def execute(session_id: str):
    return SessionProxy.from_session_id(session_id)

class SetSession(DepotMessage[[str, SessionProxy], None]):
  @staticmethod
  def execute(session_id: str, new_session: SessionProxy):
    sessions[session_id] = new_session.to_session()

class TryCreateNewSession(DepotMessage[[], Optional[str]]):
  '''Try to create a new session if we have capacity'''

  @staticmethod
  def create_new_session():
    token_candidate = secrets.token_hex(settings.SESSION_ID_BYTES)
    while token_candidate in sessions:
      token_candidate = secrets.token_hex(settings.SESSION_ID_BYTES)
    updated_time = datetime.now(timezone.utc).timestamp()
    new_revere = Pigeon()
    sessions[token_candidate] = Session(updated_time, new_revere)
    return token_candidate

  @staticmethod
  def execute():
    if len(sessions) >= settings.MAX_SESSIONS:
      current_time = datetime.now(timezone.utc).timestamp()
      to_remove = None
      for session_id, session in sessions.items():
        if current_time - session.updated >= settings.SESSION_TIMEOUT:
          to_remove = session_id
          break
      if to_remove is not None:
        sessions.pop(to_remove)
        return TryCreateNewSession.create_new_session()
      else:
        return None
    else:
      return TryCreateNewSession.create_new_session()


session_manager_messages: List[Type[DepotMessage]] = [
  GetSession, SetSession, TryCreateNewSession, PigeonProxy.PigeonMessage
]
for message in session_manager_messages:
  message.register()
