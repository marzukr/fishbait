import { useEffect } from 'react';

/**
 * A hook to register a given callback when a key is pressed. Note that the old
 * event listener is cleared and a new one is added anytime the callback
 * function changes. This includes if the callback function changes to a
 * different anonymous function that is identical in behavior. If releaseFn is
 * given, that function is called when the keypress is released.
 */
export const useKey = (
  pressFn: (key: string) => void,
  releaseFn?: (key: string) => void
) => {
  useEffect(() => {
    const makeListener = (fn: (key: string) => void) => (
      (event: KeyboardEvent) => {
        event.preventDefault();
        fn(event.code);
      }
    );

    const downListener = makeListener(pressFn);
    window.addEventListener('keydown', downListener);
    const cleanDownListener = () => (
      window.removeEventListener('keydown', downListener)
    );
    if (releaseFn === undefined) return cleanDownListener;

    const upListener = makeListener(releaseFn);
    window.addEventListener('keyup', upListener);
    return () => {
      cleanDownListener();
      window.removeEventListener('keyup', upListener);
    }
  }, [pressFn, releaseFn]);
};
