"""Utils for the API."""

import logging
import ddtrace

logging.basicConfig(
  level=logging.DEBUG,
  format=(
    '%(asctime)s %(levelname)s [%(name)s] [%(filename)s:%(lineno)d] '
    '[dd.service=%(dd.service)s dd.env=%(dd.env)s dd.version=%(dd.version)s dd.trace_id=%(dd.trace_id)s dd.span_id=%(dd.span_id)s] '
    '- %(message)s'
  )
)

def get_logger(name: str):
  return logging.getLogger(name)

@ddtrace.tracer.wrap()
def hello():
    get_logger(__name__).info('Hello, World!')
