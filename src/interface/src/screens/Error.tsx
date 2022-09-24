// Copyright 2021 Marzuk Rashid

import React, { useState } from 'react';

import { Api } from 'utils/api';

interface ErrorInterface {
  /** Our Api object */
  api: Api,
}

/** This screen is displayed when the server is at capacity. */
export const Error: React.FC<ErrorInterface> = ({ api }) => {
  const [email, setEmail] = useState('');

  const updateEmail = (e: React.ChangeEvent<HTMLInputElement>) => {
    setEmail(e.target.value);
  };

  const submitEmail = (e: React.FormEvent<HTMLFormElement>) => {
    e.preventDefault();
    // TODO: disable submit button
    api.joinEmailList(email).then(() => {
      // TODO: success message screen
    });
  }

  const messages = [
    `Hi there! I'm Fishbait—an AI agent designed to play poker :)`,
    `Although I'd love to play some hands with you, my CPUs are at maximum
    capacity.`,
    `Come back in a few hours. I should have a chance to rest before then.`,
    `I can also notify you when I upgrade myself with more CPUs:`,
  ];

  const messageElements = messages.map((v, i) => (
    <p className='text-justify' key={i}>{v}</p>
  ));

  return (
    <div className='absolute inset-x-0 max-w-sm mx-auto h-screen'>
      <div className='absolute top-1/2 inset-x-11 -translate-y-2/4'>
        {messageElements}
        <form className='flex h-9' onSubmit={submitEmail}>
          <input
            className={
              `h-9 border-y border-l border-black rounded-l-md box-border
               align-top text-base border-r-0 font-sans grow`
            }
            type='email' value={email} placeholder='Email address'
            onChange={updateEmail}
          />
          <button
            className={
              `flex h-9 w-9 text-base rounded-r-md border border-black
               box-border align-top text-white justify-center items-center
               bg-green-550`
            } type='submit'
          >
            <i className='fas fa-chevron-right'/>
          </button>
        </form>
      </div>
    </div>
  );
}
