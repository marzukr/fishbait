import { useState, useEffect, useCallback, useMemo, useRef } from 'react';
import { isEqual, mapKeys, snakeCase } from 'lodash';

import {
  objectAgent, numberAgent, booleanAgent, enumAgent, arrayAgent,
  nullableAgent, Stamped, stringAgent, unsnakeAgent, conversionAgent, Nullable
} from 'cbp';
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

const actionInterfaceAgent = unsnakeAgent(objectAgent({
  action: enumAgent(Action),
  size: numberAgent,
  actionIdx: nullableAgent(numberAgent),
}));
export type ActionInterface = Stamped<typeof actionInterfaceAgent>;
export type PartialAction = Nullable<ActionInterface>;

const availableActionAgent = unsnakeAgent(objectAgent({
  action: enumAgent(Action),
  size: numberAgent,
  policy: numberAgent,
  actionIdx: numberAgent,
}));
export type AvailableAction = Stamped<typeof availableActionAgent>;

const cardAgent = nullableAgent(stringAgent);
export type CardT = Stamped<typeof cardAgent>;

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
    nullableAgent(arrayAgent(nullableAgent(numberAgent))),
    (val) => {
      if (val === null) return val;
      if (isEqual(val, [null, null])) return [null, null];
      return val.map(c => c !== null ? isoToAsciiString[c] : null);
    },
    nullableAgent(arrayAgent(cardAgent))
  )),
  /** The cards on the public board. Null if we don't know the card yet. */
  board: arrayAgent(conversionAgent(
    nullableAgent(numberAgent),
    (c) => c !== null ? isoToAsciiString[c] : null,
    cardAgent
  )),
  /** Which player number fishbait is */
  fishbaitSeat: numberAgent,
  /** Which player, if any, we need hand info for */
  playerNeedsHand: nullableAgent(numberAgent),
  /** Which board cards, if any, we need info for */
  boardNeedsCards: nullableAgent(boardNeedsCardsAgent),
  /** If the player we need hand info for can muck */
  canMuck: booleanAgent,
  /** Names of each player */
  playerNames: arrayAgent(stringAgent),
  /** If each player's hand is known or not */
  knownCards: arrayAgent(booleanAgent),
  /** What each player's last action was */
  lastAction: arrayAgent(nullableAgent(actionInterfaceAgent)),
  /** If each board card is known or not */
  knownBoard: arrayAgent(booleanAgent),
  /** The actions available to fishbait during its last turn to act */
  availableActions: nullableAgent(arrayAgent(availableActionAgent)),
}));
export type GameState = Stamped<typeof gameStateAgent>;


enum ApiErrorCode {
  MISSING_SESSION_ID = 'MissingSessionIdError',
  UNKNOWN_SESSION_ID = 'UnknownSessionIdError',
  SERVER_OVERLOADED = 'ServerOverloadedError',
  INVALID_EMAIL = 'InvalidEmailError',
}

const apiErrorAgent = unsnakeAgent(objectAgent({
  errorMessage: stringAgent,
  errorCode: enumAgent(ApiErrorCode),
}));

class ApiError extends Error {
  constructor(rawObj: unknown) {
    const apiError = apiErrorAgent.stamp(rawObj);
    super(apiError.errorMessage);
    Object.setPrototypeOf(this, ApiError.prototype);
    this.name = apiError.errorCode;
  }
}

class NetworkError extends Error {
  constructor() {
    super("A network error occured when trying to access the API");
    Object.setPrototypeOf(this, NetworkError.prototype);
  }
}

enum RequestMethod {
  GET = 'GET',
  POST = 'POST',
}

interface ApiCallParams {
  route: string;
  method: RequestMethod;
  headers?: HeadersInit;
  body?: BodyInit;
}

const getStateParams = {
  route: 'state',
  method: RequestMethod.GET,
};

/** This hook returns an object that can make API requests. */
export const useApi = () => {
  const [canConnect, setCanConnect] = useState(true);
  const [isServerFull, setIsServerFull] = useState(false);
  const [unexpectedError, setUnexpectedError] = useState(false);
  const [gameState, setGameState] = useState<GameState | null>(null);

  const fetchFromApi = async (
    { route, method, headers, body }: ApiCallParams,
  ) => {
    let response;
    try {
      response = await fetch(`/api/${route}`, {
        method: method,
        headers: headers,
        body: body,
      });
    } catch {
      throw new NetworkError();
    }

    // Check if the reponse we got is an ApiError. If it isn't we return the
    // response.
    const responseToApiError = async (localResponse: Response) => {
      try {
        const jsonResp = localResponse.clone();
        const responseJson = await jsonResp.json();
        return new ApiError(responseJson);
      } catch {
        return;
      }
    };
    const apiError = await responseToApiError(response);
    if (apiError === undefined) return response;

    // If we got an ApiError, it's not our job to handle it. So we throw it.
    throw apiError;
  };

  const apiCallFactory = useCallback(
    (
      processor?: (r: Response) => Promise<void>,
      handler?: (e: ApiError) => Promise<boolean>
    ) => {
      const retFn = async (params: ApiCallParams) => {
        const initialPromise = fetchFromApi(params);
        const processed = initialPromise.then(processor);
        const successProm = processed.then(() => {
          setCanConnect(true);
          setUnexpectedError(false);
          return true;
        });
        const promCatch = successProm.catch((error: unknown) => {
          if (error instanceof NetworkError) {
            setCanConnect(false);
            return false;
          } else if (error instanceof ApiError && handler) {
            return handler(error);
          } else {
            throw error;
          }
        });
        const catchallCatch = promCatch.catch((error) => {
          console.error(error);
          setUnexpectedError(true);
          return false;
        });
        return catchallCatch;
      }
      return retFn;
    },
    []
  );

  const getNewSession = useCallback(
    () => {
      const newSessionHandler = async (error: ApiError) => {
        if (error.name === ApiErrorCode.SERVER_OVERLOADED) {
          setIsServerFull(true);
          return false;
        }
        throw error;
      }
      const newSessionProcessor = async () => setIsServerFull(false);
      const apiCaller = apiCallFactory(newSessionProcessor, newSessionHandler);
      return apiCaller({
        route: 'new-session',
        method: RequestMethod.GET,
      });
    },
    [apiCallFactory]
  );

  const gameStateProcesssor = async (response: Response) => {
    const rawResult = await response.json();
    const stamped = gameStateAgent.stamp(rawResult);
    setGameState(stamped);
    setIsServerFull(false);
  };

  const getState = useCallback(
    () => {
      const gameStateApiCall = apiCallFactory(gameStateProcesssor);
      return gameStateApiCall(getStateParams);
    },
    [apiCallFactory]
  );

  const gameStateHandler = useCallback(
    async (error: ApiError) => {
      if (
        error.name === ApiErrorCode.MISSING_SESSION_ID
        || error.name === ApiErrorCode.UNKNOWN_SESSION_ID
      ) {
        const hasNewSession = await getNewSession();
        if (hasNewSession) {
          return await getState();
        }
      }
      return false;
    },
    [getNewSession, getState]
  );

  const gameStateApiCallWithRetry = useMemo(
    () => apiCallFactory(gameStateProcesssor, gameStateHandler),
    [apiCallFactory, gameStateHandler]
  );

  const apiPostParams = (
    route: string, bodyContent?: object
  ): ApiCallParams => {
    return {
      route: route,
      method: RequestMethod.POST,
      headers: bodyContent ? {
        'Content-Type': 'application/json'
      } : undefined,
      body: bodyContent
        ? JSON.stringify(mapKeys(bodyContent, (_, key) => snakeCase(key)))
        : undefined
    }
  };

  const gameStateApiPostWithRetry = (route: string, bodyContent?: object) => {
    const params = apiPostParams(route, bodyContent);
    return gameStateApiCallWithRetry(params);
  };

  const state = useCallback(
    () => gameStateApiCallWithRetry(getStateParams),
    [gameStateApiCallWithRetry]
  );

  const apply = (action: Nullable<ActionInterface>) => (
    gameStateApiPostWithRetry('apply', action)
  );

  const setHand = (hand: Array<number | null>) => (
    gameStateApiPostWithRetry('set-hand', { hand })
  );

  const setBoard = (board: Array<number | null>) => (
    gameStateApiPostWithRetry('set-board', { board })
  );

  const newHand = () => gameStateApiPostWithRetry('new-hand');

  const reset = (
    params: {
      stack: number[], button: number, fishbaitSeat: number,
      playerNames: string[], bigBlind: number, smallBlind: number,
    }
  ) => gameStateApiPostWithRetry('reset', params);

  const joinEmailList = (email: string) => {
    const emailHandler = async (error: ApiError) => {
      if (error.name !== ApiErrorCode.INVALID_EMAIL) throw error;
      return false;
    }
    const apiCaller = apiCallFactory(undefined, emailHandler);
    return apiCaller(apiPostParams('join-email-list', { email }));
  };

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
    /** Adds the user to our email list */
    joinEmailList,

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

/** Hook to keep track of if a React component is mounted */
const useMountedState = () => {
  const mountedRef = useRef(false);
  const isMounted = useCallback(() => mountedRef.current, []);
  useEffect(
    () => {
      mountedRef.current = true;
      return () => {
        mountedRef.current = false;
      };
    },
    []
  );
  return isMounted
};

/**
 * This hook will return a promise that only resolves if the component in which
 * it's contained is mounted. Needed for API callbacks that modify component
 * state.
 */
export const useSafeAsync = () => {
  const isMounted = useMountedState();
  const safeAsync = useCallback(
    async <T>(promise: Promise<T>) => {
      return new Promise<T>(
        (resolve) => {
          promise.then(
            (value) => {
              if (isMounted()) resolve(value);
            }
          );
        }
      );
    },
    [isMounted]
  );
  return safeAsync
}
