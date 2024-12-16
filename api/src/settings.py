'''Global config for the fishbait API'''

import os

STRATEGY_LOCATION: str = os.getenv("STRATEGY_LOCATION", "")
RELAY_LIB_LOCATION: str = os.getenv("RELAY_LIB_LOCATION", "")

SESSION_ID_BYTES: int = 16
SESSION_TIMEOUT: int = int(os.getenv("SESSION_TIMEOUT", "86400"))  # in seconds
SESSION_ID_KEY = os.getenv("SESSION_ID_KEY", "session_id")
PIGEON_EXECUTION_TIMEOUT: int = (
    int(os.getenv("PIGEON_EXECUTION_TIMEOUT", "5"))  # in seconds
)

PLAYERS: int = 6
HAND_CARDS: int = 2
BOARD_CARDS: int = 5
CARDS_IN_DECK: int = 52

REDIS_SESSION_DB: int = 0
REDIS_CELERY_DB: int = 1
