import { IWinBracket } from '@revenge-games/logic-sharedlibs/dist/types';
import { IMathWinLine } from './math';

export enum SYMBOL_TYPE {
  WILD = 'WILD',
  SCATTER = 'SCATTER',
  PICTURE_SYMBOL = 'PICTURE_SYMBOL',
  POKER_SYMBOL = 'POKER_SYMBOL',
  BLOCK_SYMBOL = 'BLOCK_SYMBOL',
}
// GAME CONFIG
export interface IGameConfig {
  profile: string;
  profileRTP: number; // example 97
  name: string;
  code: string;
  version: string;
  build: string;
  baseBet: number;
  reelSize: number[];
  maxWin: number;
  matrixDefault: number[];
  paytable: number[][];
  freeSpin: number;
  winBrackets: IWinBracket[];
  symbols: { id: number; code: string; type: SYMBOL_TYPE }[];
}

// eslint-disable-next-line @typescript-eslint/no-empty-object-type
export interface IGameCustomStateData {
  // state data based on specific game
}

export interface IGameCustomSpinResult {
  // spin result data based on specific game
  result: IMathWinLine[];
  betSize: number;
  betLevel: number;
  smallMultiplier: number;
  bigMultiplier: number;
  blockerCount: number;
  collapseCount: number;
  isSmallMultiplierIsApplied: boolean;
  isBigMultiplierIsApplied: boolean;
}

// eslint-disable-next-line @typescript-eslint/no-empty-object-type
export interface IGameCustomCheatPayout extends Record<string, undefined | boolean> {}
