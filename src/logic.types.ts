/* eslint-disable @typescript-eslint/no-empty-object-type */
import {
  ICheatPayout,
  IDescriptionResponse,
  IPlayBet,
  IPlayRequest,
  IPlayResponse,
  IPlayResponseBet,
  IPlayResponseResult,
  IPlayResponseState,
  IResultRequest,
  ISymbols,
} from '@revenge-games/logic-sharedlibs/dist/server/app.type';
import { IMathBetMode, IMathSpinResult, IMathWinLine } from './math';

/**
 * =============================================
 * DO NOT CHANGE THIS ONE
 */
export type IGameDescriptionResponse = IDescriptionResponse<IGameCustomDescriptionResponse>;
export type IGameResultRequest = IResultRequest<
  IGameCustomBetData,
  IMathSpinResult,
  IGameCustomStateData,
  IGameCustomCheatPayout
>;
export type IGamePlayRequest = IPlayRequest<
  IGameCustomBetData,
  IMathSpinResult,
  IGameCustomStateData,
  IGameCustomCheatPayout
>;
export type IGamePlayResponse = IPlayResponse<
  IGameCustomBetData,
  IGameCustomSpinResult,
  IMathSpinResult,
  IGameCustomStateData
>;
export type IGameCheatPayout = ICheatPayout<IGameCustomCheatPayout>;
export type IGamePlayResponseState = IPlayResponseState<
  IMathSpinResult,
  IGameCustomStateData,
  IGameCustomCheatPayout
>;
export type IGamePlayResponseBet = IPlayResponseBet<IGameCustomBetData>;
export type IGamePlayResponseResult = IPlayResponseResult<IGameCustomSpinResult>;
/**
 * =============================================
 * DO NOT CHANGE THIS ONE
 */

export interface IBetConfigItem<> {
  betMode: IMathBetMode;
  betCost: number;
  canUseThisFeature: (reqBet: IPlayBet<IGameCustomBetData>) => boolean;
  /**
   * false: in case buy for example
   * true: in case sure win for example
   */
  isCalculateBracketWithBetCost?: boolean;
}

// GAME CONFIG
export interface IGameConfig extends IDescriptionResponse, IGameCustomDescriptionResponse {
  // custom based on game specific
}

export interface IGameCustomDescriptionResponse {
  // custom based on game specific
  payline: number[][];
  selectedPayLine: number;
  circleIndex: number;
  triangleIndex: number;
  diamondIndex: number;
  symbols: ISymbols; // For docs only
}

// eslint-disable-next-line @typescript-eslint/no-empty-object-type
export interface IGameCustomBetData {
  // custom based on game specific
  betMode: IMathBetMode;
}

// eslint-disable-next-line @typescript-eslint/no-empty-object-type
export interface IGameCustomSpinResult {
  // custom based on game specific
  matrix: number[];
  result: IMathWinLine[];
  betSize: number;
  betLevel: number;
  selectionMode: number;
  reelMode: number;
  multiplier: number;
  multiplierReelVector: number[];
}

// eslint-disable-next-line @typescript-eslint/no-empty-object-type
export interface IGameCustomStateData {
  // custom based on game specific
}

export interface IGameCustomCheatPayout {
  // custom based on game specific
}
