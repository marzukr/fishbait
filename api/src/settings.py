'''Global config for the fishbait API'''

import os

STRATEGY_LOCATION: str = os.getenv("STRATEGY_LOCATION", "")
RELAY_LIB_LOCATION: str = os.getenv("RELAY_LIB_LOCATION", "")

MAX_SESSIONS: int = int(os.getenv("MAX_SESSIONS", "1000"))
SESSION_ID_BYTES: int = int(os.getenv("SESSION_ID_BYTES", "8"))
SESSION_TIMEOUT: int = int(os.getenv("SESSION_TIMEOUT", "1800"))  # in seconds
SESSION_ID_KEY = os.getenv("SESSION_ID_KEY", "session_id")
PIGEON_EXECUTION_TIMEOUT: int = (
    int(os.getenv("PIGEON_EXECUTION_TIMEOUT", "5"))  # in seconds
)

PLAYERS: int = 6
HAND_CARDS: int = 2
BOARD_CARDS: int = 5
CARDS_IN_DECK: int = 52

EMAIL_LIST_LOCATION: str = "/tmp/emails.txt"
