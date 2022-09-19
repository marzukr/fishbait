// Copyright 2021 Marzuk Rashid

import React, { useState } from 'react';

import { Game } from 'screens/Game';
import { Settings } from 'screens/Settings';
import Login from 'screens/Login';
import { useApi } from 'utils/api';

/** Routes the user to the proper screen in our app. */
export const App: React.FC = () => {
  const [onSettingsPage, setOnSettingsPage] = useState(false);
  const api = useApi();

  const toggleSettings = () => {
    setOnSettingsPage(!onSettingsPage);
  }

  if (!api.canConnect) {
    return <>server down error message with contact info</>;
  } else if (api.isServerFull) {
    return <Login />
  } else if (api.unexpectedError) {
    return <>unexpected error error message with contact info"</>;
  } else if (api.gameState === null) {
    return <>loading screen</>;
  } else if (onSettingsPage) {
    return <Settings api={api} toggleSettings={toggleSettings} />;
  } else {
    return <Game api={api} toggleSettings={toggleSettings} />
  }
}
