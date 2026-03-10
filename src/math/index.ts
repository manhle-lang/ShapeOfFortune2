// eslint-disable-next-line @typescript-eslint/no-require-imports
const addon = require('bindings')('shape_of_fortune.node');

export enum IMathBetMode {
  CIRCLE = 0,
  TRIANGLE = 1,
  DIAMOND = 2,
}

export interface IMathGameConfig {
  reelStripCircle: number[][][];
  reelStripTriangle: number[][][];
  reelStripDiamond: number[][][];
  paytable: number[][];
  payline: number[][];
  circleIndex: number;
  triangleIndex: number;
  diamondIndex: number;
  reelWindowSize: number[];
  triangleMultiplierWeightage: number[][];
  diamondMultiplierWeightage: number[][];
  mgReelWeightage: number[][];
  multiplierWeightage: number[][][];
  freeGameChance: number[];
  teaserChance: number[];
  fgTriangleMultiplierWeightage: number[][];
  fgDiamondMultiplierWeightage: number[][];
  fgReelWeightage: number[][];
  fbReelWeightage: number[][];
  fgMultiplierWeightage: number[][][];
  featureBuyCost: number[];
  baseBet: number;
  selectedPayLine: number;
  maxWinMultiplier: number;
}

export interface IMathWinLine {
  winPosition: number[];
  symbolId: number;
  lineId: number;
  winAmount: number;
  multiplier: number;
  winAmountWithoutMultiplier: number;
}

interface IMathSpinResultOri {
  baseBetAmount: number;
  betAmount: number;
  checkPayout: number;
  payout: number;
  maxWinQuota: number;

  resultVector: number[];
  multiplierReelVector: number[];
  winningVector: boolean[];
  winLines: IMathWinLine[];

  selectionMode: number;
  reelMode: number;
  multiplier: number;

  triggerMaxWin: boolean;
  teaser: boolean;
  featureBuy: boolean;
  totalFreeSpinsCount: number;
  totalFreeGameWin: number;
}

export interface IMathSpinResult extends IMathSpinResultOri {
  winFreeSpins: number;
  freeSpins: number;
  betSize: number;
  betLevel: number;
  isEndRound: boolean;
  isMaxWin: boolean;
  _rngCheatSeed?: string;
}

export interface IMathConfigsRequest {
  profile?: number;
}

export interface IMathSpinRequest {
  betSize: number;
  betLevel: number;
  profile: number;
  gameConfig?: Partial<IMathGameConfig>;
  betMode: IMathBetMode;
  pastSpinResult?: IMathSpinResult;
  parentSpinResult?: IMathSpinResult;
  _rngCheatSeed?: string;
  featureBuy?: boolean;
}

export const GameMath = {
  DoSpin: (request: IMathSpinRequest): IMathSpinResult => {
    return addon.DoSpin(request);
  },

  DoCheat: (request: IMathSpinRequest): IMathSpinResult => {
    return addon.DoCheat(request);
  },

  GetConfigs: (request: IMathConfigsRequest): IMathGameConfig => {
    return addon.GetConfigs(request);
  },
};
