import { ProfileVersion, GetProfileRTPPercent } from '@revenge-games/logic-sharedlibs/dist/types';
import { v3 } from '@revenge-games/logic-sharedlibs/dist/wintitles';
import packageJson from '../package.json';
import { IBetConfigItem, IGameConfig, IGameCustomBetData } from './logic.types';
import { GameMath, IMathBetMode } from './math';
import { IPlayBet } from '@revenge-games/logic-sharedlibs/dist/server/app.type';

// Return available profiles which are support by native math only
export const GetAvailableProfiles = () => [ProfileVersion.b3];

// DONT EXPOSE THIS VARIABLE, IT IS FOR CACHING AND REDUCING INVOKE TO CPP
const CONFIG_VAULTS: Record<number, IGameConfig> = {};
export const GetGameConfigs = (profile?: ProfileVersion): IGameConfig => {
  const validProfile = GetProfileRTPPercent(profile);
  if (!CONFIG_VAULTS[validProfile]) {
    const DEFAULT_PROFILE_CONFIGS = GameMath.GetConfigs({ profile: GetProfileRTPPercent(profile) });
    CONFIG_VAULTS[validProfile] = {
      name: packageJson.name,
      code: packageJson.name,
      version: packageJson.version,
      build: '1.0.0',
      profile: '',
      winBrackets: v3.Brackets,
      matrixDefault: [8, 9, 9, 3, 0, 3, 5, 6, 6],
      baseBet: DEFAULT_PROFILE_CONFIGS.baseBet,
      reelSize: DEFAULT_PROFILE_CONFIGS.reelWindowSize,
      maxWin: DEFAULT_PROFILE_CONFIGS.maxWinMultiplier,
      payTable: DEFAULT_PROFILE_CONFIGS.paytable,
      freeSpin: 0,
      circleIndex: DEFAULT_PROFILE_CONFIGS.circleIndex,
      triangleIndex: DEFAULT_PROFILE_CONFIGS.triangleIndex,
      diamondIndex: DEFAULT_PROFILE_CONFIGS.diamondIndex,
      payline: DEFAULT_PROFILE_CONFIGS.payline,
      selectedPayLine: DEFAULT_PROFILE_CONFIGS.selectedPayLine,
      symbols: [
        { id: 0, type: 'WILD' },
        { id: 1, type: 'SCATTER' },
        { id: 2, type: 'PICTURE_SYMBOL' },
        { id: 3, type: 'PICTURE_SYMBOL' },
        { id: 4, type: 'PICTURE_SYMBOL' },
        { id: 5, type: 'PICTURE_SYMBOL' },
        { id: 6, type: 'POKER_SYMBOL' },
        { id: 7, type: 'POKER_SYMBOL' },
        { id: 8, type: 'POKER_SYMBOL' },
        { id: 9, type: 'POKER_SYMBOL' },
        { id: 10, type: 'POKER_SYMBOL' },
      ],
    };
  }

  return CONFIG_VAULTS[validProfile];
};

export const BetConfigs: IBetConfigItem[] = [
  // Feature Buy Configs
  {
    betMode: IMathBetMode.CIRCLE,
    betCost: 25,
    canUseThisFeature: (reqBet: IPlayBet<IGameCustomBetData>) =>
      (reqBet.data?.betMode as IMathBetMode) === IMathBetMode.CIRCLE && !!reqBet.data?.isBuyFeature,
    isCalculateBracketWithBetCost: true,
  },
  {
    betMode: IMathBetMode.TRIANGLE,
    betCost: 40,
    canUseThisFeature: (reqBet: IPlayBet<IGameCustomBetData>) =>
      (reqBet.data?.betMode as IMathBetMode) === IMathBetMode.TRIANGLE &&
      !!reqBet.data?.isBuyFeature,
    isCalculateBracketWithBetCost: true,
  },
  {
    betMode: IMathBetMode.DIAMOND,
    betCost: 80,
    canUseThisFeature: (reqBet: IPlayBet<IGameCustomBetData>) =>
      (reqBet.data?.betMode as IMathBetMode) === IMathBetMode.DIAMOND &&
      !!reqBet.data?.isBuyFeature,
    isCalculateBracketWithBetCost: true,
  },
  // Normal Spin Configs
  {
    betMode: IMathBetMode.CIRCLE,
    betCost: 1,
    canUseThisFeature: (reqBet: IPlayBet<IGameCustomBetData>) =>
      !reqBet.data?.betMode ||
      ((reqBet.data.betMode as IMathBetMode) === IMathBetMode.CIRCLE && !reqBet.data?.isBuyFeature),
    isCalculateBracketWithBetCost: false,
  },
  {
    betMode: IMathBetMode.TRIANGLE,
    betCost: 1,
    canUseThisFeature: (reqBet: IPlayBet<IGameCustomBetData>) =>
      (reqBet.data?.betMode as IMathBetMode) === IMathBetMode.TRIANGLE &&
      !reqBet.data?.isBuyFeature,
    isCalculateBracketWithBetCost: false,
  },
  {
    betMode: IMathBetMode.DIAMOND,
    betCost: 1,
    canUseThisFeature: (reqBet: IPlayBet<IGameCustomBetData>) =>
      (reqBet.data?.betMode as IMathBetMode) === IMathBetMode.DIAMOND && !reqBet.data?.isBuyFeature,
    isCalculateBracketWithBetCost: false,
  },
];
