// Copyright 2021 Marzuk Rashid

import React from 'react';

const DISCORD_PURPLE = '#5865F2';
const DISCORD_INVITE_LINK = 'https://discord.gg/r8dXHVSqe6';

/** A link to the Fishbait Discord server */
export const DiscordButton: React.FC = () => {
  return (
    <a
      className={`
        bg-[${DISCORD_PURPLE}] text-white no-underline px-0.5 rounded
        hover:shadow-md transition-shadow
      `} href={DISCORD_INVITE_LINK} target='_blank' rel='noopener noreferrer'
    >
      Discord
    </a>
  );
}
