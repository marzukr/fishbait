"""Utils for the API."""

import logging

logging.basicConfig(
  level=logging.DEBUG,
  format='%(levelname)s:%(name)s:%(lineno)d: %(message)s'
)

def get_logger(name: str):
  return logging.getLogger(name)
