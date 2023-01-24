import React, { useState } from 'react';

import { Api, useSafeAsync } from 'utils/api';

interface ErrorInterface {
  api: Api,
  initialMessages: JSX.Element[],
  displayEmailForm: boolean,
}

/** This screen is displayed when we encounter an error from the API. */
export const Error: React.FC<ErrorInterface> = (
  { api, initialMessages, displayEmailForm }
) => {
  const [email, setEmail] = useState('');
  const [submitEnabled, setSubmitEnabled] = useState(true);
  const [displayForm, setDisplayForm] = useState(displayEmailForm);
  const [messages, setMessages] = useState(initialMessages);

  const safeAsync = useSafeAsync();

  const updateEmail = (e: React.ChangeEvent<HTMLInputElement>) => {
    setEmail(e.target.value);
  };

  const submitEmail = (e: React.FormEvent<HTMLFormElement>) => {
    e.preventDefault();
    setSubmitEnabled(false);
    safeAsync(api.joinEmailList(email)).then(() => {
      setDisplayForm(false);
      setMessages([<>Thank you! I hope to get back to you shortly.</>]);
    });
  }

  const messageElements = messages.map((v, i) => (
    <p className='text-justify mt-2.5' key={i}>{v}</p>
  ));

  const renderForm = () => {
    if (!displayForm) return null;
    return (
      <form className='flex h-9' onSubmit={submitEmail}>
        <input
          className={
            `h-9 border-y border-l border-black rounded-l-md box-border
              align-top text-base border-r-0 font-sans grow rounded-r-none`
          }
          type='email' value={email} placeholder='Email address'
          onChange={updateEmail}
        />
        <button
          className={
            `flex h-9 w-9 text-base rounded-r-md border border-black
              box-border align-top text-white justify-center items-center
              bg-green-550`
          } type='submit' disabled={!submitEnabled}
        >
          <i className='fas fa-chevron-right'/>
        </button>
      </form>
    );
  }

  return (
    <div className='absolute inset-x-0 max-w-sm mx-auto h-screen'>
      <div className='absolute top-1/2 inset-x-11 -translate-y-2/4'>
        {messageElements}
        {renderForm()}
      </div>
    </div>
  );
}
