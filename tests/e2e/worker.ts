/* eslint-disable @typescript-eslint/no-explicit-any */
import {
  EndToEndHandler,
  IEndToEndHandlerResult,
  IGameRunEndToEndParamsConfig,
} from '@revenge-games/logic-sharedlibs/dist/e2e/type';
import { BetType, ProfileVersion, ENGINE_TYPE } from '@revenge-games/logic-sharedlibs/dist/types';
import Decimal from 'decimal.js';
import { LogicService } from '../../src/logic.service';
import { GetGameConfigs } from '../../src/logic.config';
import { IPlayBetData } from '@revenge-games/logic-sharedlibs/dist/server/app.type';
import { IGameCustomBetData, IGamePlayRequest, IGamePlayResponse } from '../../src/logic.types';
import { IMathBetMode } from '../../src/math';

const LOGIC_SERVICE = LogicService.GetInstance({} as any);
const GAME_CONFIG = GetGameConfigs();

if (!GAME_CONFIG.maxWin || !GAME_CONFIG.baseBet) {
  throw new Error(
    `INVALID game configs - maxWin(${GAME_CONFIG.maxWin}) or baseBet(${GAME_CONFIG.baseBet})`,
  );
}

/**
 * Custom this one based on specific game bet feature/mode
 * config.betType will get from --m in e2e command
 * ex: `pnpm e2e --n=1M --m=circle` => configs.betType is circle
 */
// eslint-disable-next-line @typescript-eslint/no-unused-vars
const GAME_BET_DATA = (configs: IGameRunEndToEndParamsConfig): IPlayBetData<IGameCustomBetData> => {
  let betMode: IMathBetMode;

  switch (configs.betType) {
    case 'circle':
      betMode = IMathBetMode.CIRCLE;
      break;
    case 'triangle':
      betMode = IMathBetMode.TRIANGLE;
      break;
    case 'diamond':
      betMode = IMathBetMode.DIAMOND;
      break;
    default:
      betMode = IMathBetMode.TRIANGLE;
      break;
  }
  return {
    betMode,
  };
};
/**
 * Custom this one based on specific game
 * If no any specific validation, just return
 */
const GAME_CUSTOM_VALIDATOR = (
  // eslint-disable-next-line @typescript-eslint/no-unused-vars
  spinRequest: IGamePlayRequest,
  // eslint-disable-next-line @typescript-eslint/no-unused-vars
  playResponse: IGamePlayResponse,
): void => {
  // CUSTOM LOGIC VALIDATION - based on specific game
  if (spinRequest.bet.data?.betMode !== playResponse.result.selectionMode) {
    throw new Error(`INVALID betMode and response selectionMode: expect selectionMode to be ${spinRequest.bet.data?.betMode} but got ${playResponse.result.selectionMode}`)
  }
  return;
};

/**
 * @param configs is the configs from end-to-end.ts
 * @returns
 */
export const runEndToEnd: EndToEndHandler = (
  configs: IGameRunEndToEndParamsConfig,
): IEndToEndHandlerResult => {
  let isFG = false;
  let isMaxWin = false;
  let roundCount = 1;
  let totalBetAmount: Decimal = new Decimal(0);
  let totalWinAmount: Decimal = new Decimal(0);
  let normalTotalWin: Decimal = new Decimal(0);
  let freeTotalWin: Decimal = new Decimal(0);

  const spinRequest: IGamePlayRequest = {
    bet: {
      betId: '',
      betLevel: configs.betLevel,
      betSize: configs.betSize,
      type: configs.betType as BetType,
      data: GAME_BET_DATA(configs),
    },
    state: configs.playerInfo?.state || {
      profile: ProfileVersion.b3,
      freeSpins: 0,
      freeSpinsWinAmount: 0,
      data: {
        parentBetId: undefined,
        parentSpinResult: undefined,
        pastSpinResult: undefined,
      },
      v: 0,
      engine: ENGINE_TYPE.ANYTHING,
    },
    profile: configs.spinConfigs.profile || ProfileVersion.b3,
  };

  let playResponse: IGamePlayResponse = LOGIC_SERVICE.refreshInstance({
    body: spinRequest,
  } as any).doPlay();
  totalBetAmount = totalBetAmount.add(playResponse.bet.betAmount);
  totalWinAmount = totalWinAmount.add(playResponse.bet.winAmount);
  normalTotalWin = normalTotalWin.add(playResponse.bet.winAmount);

  if (playResponse.bet.type === BetType.WIN_FREESPIN && playResponse.bet.winFreeSpins > 0) {
    isFG = true;
  }

  if ((!playResponse.bet.isEndRound && !isFG) || (playResponse.bet.isEndRound && isFG)) {
    throw new Error(`WinFreespin handling failure!`);
  }

  GAME_CUSTOM_VALIDATOR(spinRequest, playResponse);

  spinRequest.state = playResponse.state;
  const parentBet = structuredClone(spinRequest);
  while (!playResponse.bet.isEndRound) {
    // make sure maxWin must end game
    if (playResponse.result.isMaxWin) {
      throw new Error(`MaxWin but not end game!`);
    }

    // NEED PARENT for experiencing production scenario on betting over sgc
    spinRequest.parent = parentBet.bet;
    playResponse = LOGIC_SERVICE.refreshInstance({ body: spinRequest } as any).doPlay();
    roundCount++;

    if (playResponse.bet.betAmount) {
      throw new Error(`Bet Amount have to be 0 in case of free game!`);
    }

    totalWinAmount = totalWinAmount.add(playResponse.bet.winAmount);
    freeTotalWin = freeTotalWin.add(playResponse.bet.winAmount);
    if (playResponse.bet.type === BetType.WIN_FREESPIN) {
      isFG = true;
    }

    spinRequest.state = playResponse.state;
  }

  if (!totalBetAmount.toNumber()) {
    throw new Error(`Bet Amount have to be bigger than 0!`);
  }

  /**
   * MAX WIN VALIDATION, DO NOT WASTE TIME OF SERVER, STOP IMMEDIATELY FOR BUG FIXING
   */
  const invalidMaxWin = totalWinAmount.greaterThan(
    new Decimal(GAME_CONFIG.maxWin).mul(
      spinRequest.bet.betSize * spinRequest.bet.betLevel * GAME_CONFIG.baseBet,
    ),
  );

  if (invalidMaxWin) {
    throw new Error(`MaxWin handling failure!`);
  }
  isMaxWin = playResponse.result.isMaxWin || totalWinAmount.equals(
    new Decimal(GAME_CONFIG.maxWin).mul(
      spinRequest.bet.betSize * spinRequest.bet.betLevel * GAME_CONFIG.baseBet,
    ),
  );

  const result: IEndToEndHandlerResult = {
    betDetails: {
      OVERALL: {
        winAmount: totalWinAmount.toNumber(),
        betAmount: totalBetAmount.toNumber(),
      },
      [configs.betType || BetType.NORMAL]: {
        winAmount: normalTotalWin.toNumber(),
        betAmount: totalBetAmount.toNumber(),
      },
      ...(isFG
        ? {
            [BetType.FREE_SPIN]: {
              winAmount: freeTotalWin.toNumber(),
              betAmount: totalBetAmount.toNumber(),
            },
          }
        : {}),
    },
    resultState: playResponse.state,
    resultStats: {
      FG_COUNT: {
        value: isFG ? 1 : 0,
        group: 'N/A',
        sortVal: 2,
        metricType: 'FREQUENCY',
      },
      MAX_WIN_COUNT: {
        value: isMaxWin ? 1 : 0,
        group: 'N/A',
        sortVal: 3,
        metricType: 'FREQUENCY',
      },
      MAX_ROUND_COUNT: {
        value: roundCount,
        group: 'N/A',
        sortVal: 4,
        metricType: 'MAX',
      },
      PAYOUT_DISTRIBUTION_MAIN_GAME: {
        value: normalTotalWin.toNumber() / totalBetAmount.toNumber(),
        group: 'N/A',
        sortVal: 5,
        metricType: 'PAYOUT_DISTRIBUTION',
      },
      ...(isFG
        ? {
            PAYOUT_DISTRIBUTION_FREE_GAME: {
              value: freeTotalWin.toNumber() / totalBetAmount.toNumber(),
              group: 'N/A',
              sortVal: 6,
              metricType: 'PAYOUT_DISTRIBUTION',
            },
          }
        : {}),
    },
  };

  return result;
};
