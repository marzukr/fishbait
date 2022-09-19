// Copyright 2022 Marzuk Rashid

import { useState, useEffect, useCallback, useRef } from 'react';
import { isEqual, mapKeys, snakeCase } from 'lodash';

import {
  objectAgent, numberAgent, booleanAgent, enumAgent, arrayAgent, nullable,
  Stamped, stringAgent, unsnakeAgent, conversionAgent, Nullable
} from 'utils/customs';
import { isoToAsciiString } from 'utils/hands';

enum Round {
  PREFLOP =  'Preflop',
  FLOP = 'Flop',
  TURN = 'Turn',
  RIVER = 'River',
}

/** Interface to describe which board cards we need info for. */
const boardNeedsCardsAgent = objectAgent({
  /** The first board card we need info for */
  start: numberAgent,
  /** One past the last board card we need info for */
  end: numberAgent,
});
export type BoardNeedsCards = Stamped<typeof boardNeedsCardsAgent>;

export enum Action {
  FOLD = 'Fold',
  CHECK = 'Check',
  CALL = 'Call',
  BET = 'Bet',
  ALL_IN = 'All In'
}

const actionInterfaceAgent = objectAgent({
  action: enumAgent(Action),
  size: numberAgent,
});
export type ActionInterface = Stamped<typeof actionInterfaceAgent>;

/**
 * A class that mirrors PigeonState on the backend. It is received from the
 * backend and is used to store the game state on the frontend.
 */
const gameStateAgent = unsnakeAgent(objectAgent({
  /** The number of players in the game */
  players: numberAgent,
  /** The size of the big blind in chips */
  bigBlind: numberAgent,
  /** The size of the small blind in chips */
  smallBlind: numberAgent,
  /** The size of the ante in chips */
  ante: numberAgent,
  /** If the big blind pays the ante for all players */
  bigBlindAnte: booleanAgent,
  /** If blinds should be posted before the ante */
  blindBeforeAnte: booleanAgent,
  /** The size of the rake as a proportion of the pot */
  rake: numberAgent,
  /** The maximum size of the rake. 0 if no maximum */
  rakeCap: numberAgent,
  /** If hands that end preflop are not raked */
  noFlopNoDrop: booleanAgent,
  /** Player id of the button */
  button: numberAgent,
  /** If the game is currently in progress */
  inProgress: booleanAgent,
  /** What round the game is currently in */
  round: enumAgent(Round),
  /** Player id of the acting player */
  actingPlayer: numberAgent,
  /** If each player is folded or not */
  folded: arrayAgent(booleanAgent),
  /** How many players have not folded yet */
  playersLeft: numberAgent,
  /** How many players are all in */
  playersAllIn: numberAgent,
  /** The total size of the pot */
  pot: numberAgent,
  /** How much each player has contributed to the pot */
  bets: arrayAgent(numberAgent),
  /** The size of each player's stack */
  stack: arrayAgent(numberAgent),
  /** The minimum legal raise size for the acting player */
  minRaise: numberAgent,
  /** The amount the current player needs to call */
  neededToCall: numberAgent,
  /**
   * Each player's hand. If we don't know the player's hand, then it will be
   * null. If the player has mucked, then it will be an array filled with null.
   */
  hands: arrayAgent(conversionAgent(
    nullable(arrayAgent(nullable(numberAgent))),
    (val) => {
      if (val === null) return val;
      if (isEqual(val, [null, null])) return [null, null];
      return val.map(c => c ? isoToAsciiString[c] : null);
    },
    nullable(arrayAgent(nullable(stringAgent)))
  )),
  /** The cards on the public board. Null if we don't know the card yet. */
  board: arrayAgent(conversionAgent(
    nullable(numberAgent),
    (c) => c ? isoToAsciiString[c] : null,
    nullable(stringAgent)
  )),
  /** Which player number fishbait is */
  fishbaitSeat: numberAgent,
  /** Which player, if any, we need hand info for */
  playerNeedsHand: nullable(numberAgent),
  /** Which board cards, if any, we need info for */
  boardNeedsCards: nullable(boardNeedsCardsAgent),
  /** If the player we need hand info for can muck */
  canMuck: booleanAgent,
  /** Names of each player */
  playerNames: arrayAgent(stringAgent),
  /** If each player's hand is known or not */
  knownCards: arrayAgent(booleanAgent),
  /** What each player's last action was */
  lastAction: arrayAgent(nullable(actionInterfaceAgent)),
  /** If each board card is known or not */
  knownBoard: arrayAgent(booleanAgent),
}));
export type GameState = Stamped<typeof gameStateAgent>;

const apiErrorAgent = unsnakeAgent(objectAgent({
  errorMessage: stringAgent,
  errorCode: stringAgent,
}));
class ApiError extends Error {
  constructor(rawObj: unknown) {
    const apiError = apiErrorAgent.stamp(rawObj);
    super(apiError.errorMessage);
    Object.setPrototypeOf(this, ApiError.prototype);
    this.name = apiError.errorCode;
  }
}
enum ApiErrorCode {
  MISSING_SESSION_ID = 'MissingSessionIdError',
  UNKNOWN_SESSION_ID = 'UnknownSessionIdError',
  SERVER_OVERLOADED = 'ServerOverloadedError',
}

class AlreadyRequestingApiError extends Error {
  constructor() {
    super('Already making an api request.');
    Object.setPrototypeOf(this, ApiError.prototype);
  }
}

enum RequestMethod {
  GET = 'GET',
  POST = 'POST',
}

interface ApiCallParams {
  route: string,
  method: RequestMethod,
  headers?: HeadersInit,
  body?: BodyInit,
  fallbackParams?: ApiCallParams,
}

const newSessionParams: ApiCallParams = {
  route: 'new_session',
  method: RequestMethod.GET,
};

const getStateParams: ApiCallParams = {
  route: 'state',
  method: RequestMethod.GET,
};

/** This hook returns an object that can make API requests. */
export const useApi = () => {
  const isMakingRequest = useRef(false);
  const [canConnect, setCanConnect] = useState(true);
  const [isServerFull, setIsServerFull] = useState(false);
  const [unexpectedError, setUnexpectedError] = useState(false);
  const [gameState, setGameState] = useState<GameState | null>(null);

  /**
   * This makes an API call without checking if we are already making an API
   * call. Don't use this unless you know what you're doing.
   * 
   * Returns either a Response object, null if there was an error we handled. Or
   * throws an ApiError if we don't know what to do with it.
   */
  const apiCall = useCallback(
    async (
      { route, method, headers, body, fallbackParams }: ApiCallParams,
    ): Promise<Response | null> => {
      // Try to make the fetch request. If the server or network is down, we set
      // the appropriate state and return null.
      const tryFetch = async () => {
        try {
          const response = await fetch(`/api/${route}`, {
            method: method,
            headers: headers,
            body: body,
          });
          setCanConnect(true);
          return response;
        } catch {
          setCanConnect(false);
          return null;
        }
      }
      const response = await tryFetch();
      if (response === null) return null;
  
      // Check if the reponse we got is an ApiError. If it isn't we return the
      // response.
      const responseToApiError = async (localResponse: Response) => {
        try {
          const jsonResp = localResponse.clone();
          const responseJson = await jsonResp.json();
          return new ApiError(responseJson);
        } catch {
          setIsServerFull(false);
          return null;
        }
      };
      const apiError = await responseToApiError(response);
      if (apiError === null) return response;
      
      // If we got here, then we have received an ApiError from the server.
      // First we check if we have a missing or unknown session id:
      if (
        apiError.name === ApiErrorCode.MISSING_SESSION_ID
        || apiError.name === ApiErrorCode.UNKNOWN_SESSION_ID
      ) {
        // If we have fallbackParams, then try to get a new session id. If that
        // succeeds then return the result of the fallbackParams.
        if (fallbackParams) {
          await apiCall(newSessionParams);
          return apiCall(fallbackParams);
        }
        // If we don't have fallbackParams, return null.
        return null;
      // If the server is currently overloaded, set the appropriate state and
      // then return null.
      } else if (apiError.name === ApiErrorCode.SERVER_OVERLOADED) {
        setIsServerFull(true);
        return null;
      }
      // If we got an ApiError that we don't know what to do with, throw it.
      throw apiError;
    },
    []
  );

  const apiCallWithGameStateImpl = useCallback(
    async (apiCallParams: ApiCallParams) => {
      const tryToMakeApiCall = async () => {
        try {
          // This will throw if we are already making a request or if we
          // encountered an unexpected ApiError
          const result = await apiCall({
            ...apiCallParams,
            fallbackParams: getStateParams,
          });
          setUnexpectedError(false);
          return result;
        } catch (error) {
          if (!(error instanceof AlreadyRequestingApiError)) {
            setUnexpectedError(true);
          } else {
            setUnexpectedError(false);
          }
          return null;
        }
      }

      // This will be null if we encountered an already handled error:
      const response = await tryToMakeApiCall();
      if (response === null) return;

      // If we get here, we should have a valid GameState object:
      const tryToMakeGameState = async () => {
        try {
          const rawResult = await response.json();
          const stamped = gameStateAgent.stamp(rawResult);
          setGameState(stamped);
          setUnexpectedError(false);
          return;
        } catch {
          setUnexpectedError(true);
          return;
        }
      };
      return tryToMakeGameState();
    },
    [apiCall]
  );

  /**
   * Checks if we are already making an API request. If not, makes an API
   * request to the server. Otherwise throws.
   */
  const apiCallWithGameState = useCallback(
    async (params: ApiCallParams) => {
      if (isMakingRequest.current) {
        throw new AlreadyRequestingApiError();
      }
      isMakingRequest.current = true;
      try {
        const response = await apiCallWithGameStateImpl(params);
        isMakingRequest.current = false;
        return response;
      } catch(e) {
        isMakingRequest.current = false;
        throw e;
      }
    },
    [apiCallWithGameStateImpl]
  );

  const state = useCallback(
    () => apiCallWithGameState(getStateParams),
    [apiCallWithGameState]
  );

  const apiPostWithGameState = async (route: string, bodyContent?: object) => {
    return apiCallWithGameState({
      route: route,
      method: RequestMethod.POST,
      headers: bodyContent ? {
        'Content-Type': 'application/json'
      } : undefined,
      body: bodyContent
        ? JSON.stringify(mapKeys(bodyContent, (_, key) => snakeCase(key)))
        : undefined
    });
  };

  const apply = (action: Nullable<ActionInterface>) => (
    apiPostWithGameState('apply', action)
  );

  const setHand = (hand: Array<number | null>) => (
    apiPostWithGameState('set_hand', { hand })
  );

  const setBoard = (board: Array<number | null>) => (
    apiPostWithGameState('set_board', { board })
  );

  const newHand = () => apiPostWithGameState('new_hand');

  const reset = (
    params: {
      stack: number[], button: number, fishbaitSeat: number,
      playerNames: string[], bigBlind: number, smallBlind: number,
    }
  ) => apiPostWithGameState('reset', params);

  useEffect(() => { state() }, [state]);

  return { 
    /** Endpoints *************************************************************/

    /** Fetches the current GameState object. */
    state,
    /** Apply the given action to the state. */
    apply,
    /** Sets the hand for the current acting player */
    setHand,
    /** Sets the public cards */
    setBoard,
    /** Starts a new hand */
    newHand,
    /** Resets the game */
    reset,

    /** Variables *************************************************************/

    /** The last fetched GameState object. */
    gameState,
    /** If we can connect to the server. */
    canConnect,
    /** If the server is currently out of capacity. */
    isServerFull,
    /** If we encountered an unexpected error when trying to make an API call */
    unexpectedError 
  };
};
export type Api = ReturnType<typeof useApi>;
