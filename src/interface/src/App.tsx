// Copyright 2021 Marzuk Rashid

import React, { useState } from 'react';

import { Game } from 'screens/Game';
import { Settings } from 'screens/Settings';
import { Error } from 'screens/Error';
import { useApi } from 'utils/api';

enum AppError {
  CONNECTION_ISSUE = 'connectionIssue',
  SERVER_FULL = 'serverFull',
  UNEXPECTED_ERROR = 'unexpectedError',
}

/** Routes the user to the proper screen in our app. */
export const App: React.FC = () => {
  const [onSettingsPage, setOnSettingsPage] = useState(false);
  const api = useApi();

  const toggleSettings = () => {
    setOnSettingsPage(!onSettingsPage);
  }

  if (!api.canConnect) {
    const serverDownMessages = [
      `We are having trouble connecting to Fishbait. Your internet connection
      might be down.`,
      `If your internet is working, then Fishbait might have gotten tilted. If
      this issue persists, please contact fishbait@marzuk.io for support.`
    ];
    return (
      <Error
        api={api} initialMessages={serverDownMessages}
        displayEmailForm={false} key={AppError.CONNECTION_ISSUE} />
    );
  } else if (api.isServerFull) {
    const serverFullMessages = [
      `Hi there! I'm Fishbait—an AI agent designed to play poker :)`,
      `Although I'd love to play some hands with you, my CPUs are at maximum
      capacity.`,
      `Come back in a few hours. I should have a chance to rest before then.`,
      `I can also notify you when I upgrade my CPUs:`,
    ];
    return (
      <Error
        api={api} initialMessages={serverFullMessages}
        displayEmailForm={true} key={AppError.SERVER_FULL} />
    );
  } else if (api.unexpectedError) {
    const unexpectedErrorMessages = [
      `We have encountered an unexpected error. Refreshing the page or using a
      different browser will probably resolve this.`,
      `If this issue persists, please contact fishbait@marzuk.io for support.`
    ];
    return (
      <Error
        api={api} initialMessages={unexpectedErrorMessages}
        displayEmailForm={false} key={AppError.UNEXPECTED_ERROR} />
    );
  } else if (api.gameState === null /** This means we are still loading */) {
    return null;
  } else if (onSettingsPage) {
    return <Settings api={api} toggleSettings={toggleSettings} />;
  } else {
    return <Game api={api} toggleSettings={toggleSettings} />
  }
}
