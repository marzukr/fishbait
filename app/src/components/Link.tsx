import React from 'react';

interface LinkProps {
  url: string;
  displayText?: string;
  newTab?: boolean;
}

export const Link: React.FC<LinkProps> = (props) => {
  const newTab = props.newTab === true || props.newTab === undefined;
  const target = newTab ? '_blank' : undefined;
  const rel = newTab ? 'noopener noreferrer' : undefined;
  return (
    <a href={props.url} className='underline' target={target} rel={rel}>
      {props.displayText || props.url}
    </a>
  );
}
