/* Returns if the given string is a suit. */
export function isSuit(entry: string) {
  return entry === 's' || entry === 'c' || entry === 'd' || entry === 'h';
}

/**
 * Constructs a card with the given inputs:
 *  - constructCard('T', 's') returns 'Ts'
 *  - constructCard('s', 'T') returns 'Ts'
 *  - constructCard('s', 'h') returns 'h'
 *  - constructCard('T', 'A') returns 'A'
 *  - constructCard('Ts', 'A') returns 'As'
 *  - etc.
 */
export function constructCard(previousEntry: string | null, newEntry: string) {
  if (previousEntry === null) return newEntry;
  let isNewSuit = isSuit(newEntry);
  if (previousEntry.length === 2) {
    if (isNewSuit) {
      return previousEntry[0] + newEntry;
    } else {
      return newEntry + previousEntry[1];
    }
  } else {
    if (isNewSuit) {
      if (isSuit(previousEntry)) {
        return newEntry;
      } else {
        return previousEntry + newEntry;
      }
    } else {
      if (isSuit(previousEntry)) {
        return newEntry + previousEntry;
      } else {
        return newEntry;
      }
    }
  }
}  // constructCard()

/** Mapping from ISO card ids to ascii strings. */
export const isoToAsciiString: Record<number, string> = {
  0: '2s', 1: '2h', 2: '2d', 3: '2c', 4: '3s', 5: '3h', 6: '3d', 7: '3c',
  8: '4s', 9: '4h', 10: '4d', 11: '4c', 12: '5s', 13: '5h', 14: '5d', 15: '5c',
  16: '6s', 17: '6h', 18: '6d', 19: '6c', 20: '7s', 21: '7h', 22: '7d',
  23: '7c', 24: '8s', 25: '8h', 26: '8d', 27: '8c', 28: '9s', 29: '9h',
  30: '9d', 31: '9c', 32: 'Ts', 33: 'Th', 34: 'Td', 35: 'Tc', 36: 'Js',
  37: 'Jh', 38: 'Jd', 39: 'Jc', 40: 'Qs', 41: 'Qh', 42: 'Qd', 43: 'Qc',
  44: 'Ks', 45: 'Kh', 46: 'Kd', 47: 'Kc', 48: 'As', 49: 'Ah', 50: 'Ad',
  51: 'Ac'
};

/** Mapping from ISO card ids to symbol strings. */
export const isoToSymbolString = {
  0: '2♠', 1: '2♥', 2: '2♦', 3: '2♣', 4: '3♠', 5: '3♥', 6: '3♦', 7: '3♣',
  8: '4♠', 9: '4♥', 10: '4♦', 11: '4♣', 12: '5♠', 13: '5♥', 14: '5♦', 15: '5♣',
  16: '6♠', 17: '6♥', 18: '6♦', 19: '6♣', 20: '7♠', 21: '7♥', 22: '7♦',
  23: '7♣', 24: '8♠', 25: '8♥', 26: '8♦', 27: '8♣', 28: '9♠', 29: '9♥',
  30: '9♦', 31: '9♣', 32: '10♠', 33: '10♥', 34: '10♦', 35: '10♣', 36: 'J♠',
  37: 'J♥', 38: 'J♦', 39: 'J♣', 40: 'Q♠', 41: 'Q♥', 42: 'Q♦', 43: 'Q♣',
  44: 'K♠', 45: 'K♥', 46: 'K♦', 47: 'K♣', 48: 'A♠', 49: 'A♥', 50: 'A♦',
  51: 'A♣'
};

export const asciiStringToIso: Record<string, number> = {
  '2s': 0, '2h': 1, '2d': 2, '2c': 3, '3s': 4, '3h': 5, '3d': 6, '3c': 7,
  '4s': 8, '4h': 9, '4d': 10, '4c': 11, '5s': 12, '5h': 13, '5d': 14, '5c': 15,
  '6s': 16, '6h': 17, '6d': 18, '6c': 19, '7s': 20, '7h': 21, '7d': 22,
  '7c': 23, '8s': 24, '8h': 25, '8d': 26, '8c': 27, '9s': 28, '9h': 29,
  '9d': 30, '9c': 31, 'Ts': 32, 'Th': 33, 'Td': 34, 'Tc': 35, 'Js': 36,
  'Jh': 37, 'Jd': 38, 'Jc': 39, 'Qs': 40, 'Qh': 41, 'Qd': 42, 'Qc': 43,
  'Ks': 44, 'Kh': 45, 'Kd': 46, 'Kc': 47, 'As': 48, 'Ah': 49, 'Ad': 50,
  'Ac': 51
};
