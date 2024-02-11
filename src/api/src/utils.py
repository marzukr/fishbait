"""Utils for the API."""

import logging
from logging.handlers import RotatingFileHandler
import ddtrace

import settings

file_handler = RotatingFileHandler(
  settings.LOG_LOCATION,
  mode='a',
  maxBytes=5*1024*1024, 
  backupCount=2,
  encoding=None,
)

logging.basicConfig(
  level=logging.DEBUG,
  format=(
    '%(asctime)s %(levelname)s [%(name)s] [%(filename)s:%(lineno)d] '
    '[dd.service=%(dd.service)s dd.env=%(dd.env)s dd.version=%(dd.version)s dd.trace_id=%(dd.trace_id)s dd.span_id=%(dd.span_id)s] '
    '- %(message)s'
  ),
  handlers=[file_handler],
)

def get_logger(name: str):
  return logging.getLogger(name)

@ddtrace.tracer.wrap()
def hello():
    get_logger(__name__).info('Hello, World!')
