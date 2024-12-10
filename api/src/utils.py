"""Utils for the API."""

import os

import logging
import ddtrace

if os.getenv('DD_SERVICE'):
  logging.basicConfig(
    level=logging.INFO,
    format=(
      '%(asctime)s %(levelname)s [%(name)s] [%(filename)s:%(lineno)d] '
      '[dd.service=%(dd.service)s dd.env=%(dd.env)s dd.version=%(dd.version)s dd.trace_id=%(dd.trace_id)s dd.span_id=%(dd.span_id)s] '  # pylint: disable=line-too-long
      '- %(message)s'
    ),
    handlers=[logging.StreamHandler()],
  )
else:
  logging.basicConfig(
    level=logging.INFO,
    format=(
      '%(asctime)s %(levelname)s [%(name)s] [%(filename)s:%(lineno)d] '
      '- %(message)s'
    ),
    handlers=[logging.StreamHandler()],
  )

def get_logger(name: str):
  return logging.getLogger(name)

@ddtrace.tracer.wrap()
def hello():
  get_logger(__name__).info('Hello, World!')
