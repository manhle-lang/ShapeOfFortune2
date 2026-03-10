/* eslint-disable @typescript-eslint/no-require-imports */
import {
  ENGINE_TYPE,
  GetProfileRTPPercent,
  WIN_TITLE,
} from '@revenge-games/logic-sharedlibs/dist/types';
import { v3 } from '@revenge-games/logic-sharedlibs/dist/wintitles';
import { GetWinTitle } from '@revenge-games/logic-sharedlibs/dist/wintitles/utils';
import { LogicService } from '../logic.service';
import { GameMath, IMathBetMode, IMathGameConfig, IMathSpinResult } from '../math';
import { CheatBuilder } from './cheat.builder';
import { IGamePlayResponse, IGameResultRequest } from '../logic.types';
import { APP_LOGGER } from '@revenge-games/logic-sharedlibs';
import { GetCheatConfigs } from './cheat.config';
import { existsSync } from 'fs';
import path from 'path';
import { getBetConfigItem } from '../logic.helper';

const CHEAT_TIMEOUT = 1000; // timeout to find valid cheat record is 1s
const RESPONSE_BUILDER = new CheatBuilder();
export const CHEAT_JSON_FILENAME = 'cheat.json';

export type ICheatJsonData = {
  serverSeed: string;
  clientSeed: string;
  cheats: Record<string, string[]>;
};

export class CheatService extends LogicService {
  static parseTriggerSymbolsCheat(
    cheatPayout: Record<string, boolean>,
  ): { symbolId: number; lineId: number } | null {
    // format: ex: isTriggerSymbols_1_payline_1
    const key = Object.keys(cheatPayout).find(
      (k) => k.startsWith('isTriggerSymbols_') && cheatPayout[k],
    );
    if (key) {
      const splited = key.split('isTriggerSymbols_')[1]?.split('_payline_');
      if (splited.length === 2 && !isNaN(Number(splited[0])) && !isNaN(Number(splited[1]))) {
        return { symbolId: Number(splited[0]), lineId: Number(splited[1]) };
      }
    }
    return null;
  }

  doPlay(): IGamePlayResponse {
    return this._doPlay().result;
  }

  /**
   * for internal usage and for initial cheats seeding
   */
  _doPlay(isSeeding?: boolean) {
    const playReq = this.REQUEST.body as IGameResultRequest;
    const { state, bet, cheatPayout } = playReq;

    if (state?.engine === ENGINE_TYPE.ENGINE) {
      throw new Error('Incompatible Engine');
    }

    let targetWinBracket: WIN_TITLE | undefined;

    if (cheatPayout.isTriggerWin) {
      targetWinBracket = WIN_TITLE.NORMAL_WIN;
    } else if (cheatPayout.isTriggerBigWin) {
      targetWinBracket = WIN_TITLE.BIG_WIN;
    } else if (cheatPayout.isTriggerMegaWin) {
      targetWinBracket = WIN_TITLE.MEGA_WIN;
    } else if (cheatPayout.isTriggerSuperMegaWin) {
      targetWinBracket = WIN_TITLE.SUPER_MEGA_WIN;
    }

    let mathSpinResult: IMathSpinResult | undefined;
    let isGotRecord = false;
    const startTS = new Date().getTime();
    const isTimeoutExceed = () =>
      isSeeding ? false : new Date().getTime() - startTS >= CHEAT_TIMEOUT;
    let runCycles = 0;

    // remove default key and use the first key only
    const cheatPayoutKey = Object.keys(cheatPayout).filter((k) => k !== 'triggerWinMode')[0];
    const gameConfig: Partial<IMathGameConfig> = {
      ...GetCheatConfigs(),
    };
    const betMode = getBetConfigItem(playReq).betMode;

    if (targetWinBracket) {
      let curWinBracket: WIN_TITLE | undefined;
      let debugMaxPayout = 0;
      let debugMaxBetAmount = 0;

      do {
        mathSpinResult = GameMath.DoCheat({
          betLevel: bet.betLevel,
          betMode,
          betSize: bet.betSize,
          profile: GetProfileRTPPercent(playReq.profile),
          parentSpinResult: playReq.state?.data?.parentSpinResult,
          pastSpinResult: playReq.state?.data?.pastSpinResult,
          gameConfig,
          _rngCheatSeed: this._getCheatSeed({ isSeeding, cheatPayoutKey, betMode }),
        });
        runCycles++;
        if (mathSpinResult.payout > debugMaxPayout) debugMaxPayout = mathSpinResult.payout;
        if (mathSpinResult.betAmount > debugMaxBetAmount)
          debugMaxBetAmount = mathSpinResult.betAmount;

        const multiplier = mathSpinResult.payout / mathSpinResult.betAmount;
        if (multiplier) curWinBracket = GetWinTitle(v3.Brackets, multiplier);
        isGotRecord = curWinBracket === targetWinBracket;
      } while (!isGotRecord && !isTimeoutExceed());
      APP_LOGGER.info({ debugMaxPayout, debugMaxBetAmount });
    } else if (cheatPayout.isTriggerFreeSpins) {
      do {
        mathSpinResult = GameMath.DoCheat({
          betLevel: bet.betLevel,
          betMode,
          betSize: bet.betSize,
          profile: GetProfileRTPPercent(playReq.profile),
          parentSpinResult: playReq.state?.data?.parentSpinResult,
          pastSpinResult: playReq.state?.data?.pastSpinResult,
          gameConfig,
          _rngCheatSeed: this._getCheatSeed({ isSeeding, cheatPayoutKey, betMode }),
        });
        runCycles++;
        isGotRecord = !!mathSpinResult.freeSpins;
      } while (!isGotRecord && !isTimeoutExceed());
    } else if (cheatPayout.isTriggerMaxWin) {
      do {
        gameConfig.mgReelWeightage = [
          // Circle Mode
          [1, 0, 0, 0, 0, 0, 0, 0],

          // Triangle Mode
          [1, 0, 0, 0, 0, 0, 0, 0, 0],

          // Diamond Mode
          [1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        ];

        mathSpinResult = GameMath.DoCheat({
          betLevel: bet.betLevel,
          betMode,
          betSize: bet.betSize,
          profile: GetProfileRTPPercent(playReq.profile),
          parentSpinResult: playReq.state?.data?.parentSpinResult,
          pastSpinResult: playReq.state?.data?.pastSpinResult,
          gameConfig,
          _rngCheatSeed: this._getCheatSeed({ isSeeding, cheatPayoutKey, betMode }),
        });
        runCycles++;

        isGotRecord = true;
      } while (!isGotRecord && !isTimeoutExceed());
    } else if (Object.keys(cheatPayout).some((k) => k.startsWith('isTriggerSymbols_'))) {
      const meta = CheatService.parseTriggerSymbolsCheat(cheatPayout as Record<string, boolean>);
      if (!meta) {
        throw new Error(`INVALID isTriggerSymbols_ cheat format`);
      }
      const { symbolId, lineId } = meta;
      do {
        mathSpinResult = GameMath.DoCheat({
          betLevel: bet.betLevel,
          betMode,
          betSize: bet.betSize,
          profile: GetProfileRTPPercent(playReq.profile),
          parentSpinResult: playReq.state?.data?.parentSpinResult,
          pastSpinResult: playReq.state?.data?.pastSpinResult,
          gameConfig,
          _rngCheatSeed: this._getCheatSeed({ isSeeding, cheatPayoutKey, betMode }),
        });
        runCycles++;
        // if (mathSpinResult.winLines.length) {
        //   console.log('mathSpinResult.winLines', mathSpinResult.winLines);
        // }

        isGotRecord = mathSpinResult.winLines.some((line) => {
          return line.lineId === lineId && line.symbolId === symbolId;
        });
      } while (!isGotRecord && !isTimeoutExceed());
    }

    APP_LOGGER.info({ runCycles, _rngCheatSeed: mathSpinResult?._rngCheatSeed });

    if (!isGotRecord || !mathSpinResult || !cheatPayoutKey || !mathSpinResult?._rngCheatSeed) {
      const msg = runCycles ? `CHEAT TIMEOUT - ${runCycles} cycles` : `INVALID CHEATS`;
      throw new Error(`${msg} - ${JSON.stringify(playReq, null)}`);
    }

    // DO BUILDING RESULTS
    const result = RESPONSE_BUILDER.build({
      mathSpinResult,
      playReq,
      cheatPayout: cheatPayout.isTriggerMaxWin ? cheatPayout : undefined,
    });

    return { result, cheatPayoutKey, rngCheatSeed: mathSpinResult._rngCheatSeed, betMode };
  }

  _genCheatJsonDataKey({
    cheatPayoutKey,
    betMode,
  }: {
    cheatPayoutKey: string;
    betMode: IMathBetMode;
  }) {
    return `${cheatPayoutKey}_${betMode}`;
  }

  _parseRNGCheatSeed(rngCheatSeed: string) {
    const splited = rngCheatSeed.split(':');
    return {
      serverSeed: splited[0] || '',
      clientSeed: splited[1] || '',
      rest: rngCheatSeed.replace(`${splited[0]}:${splited[1]}:`, '') || '',
    };
  }

  _mergeRNGCheatSeed({
    serverSeed,
    clientSeed,
    rest,
  }: {
    serverSeed: string;
    clientSeed: string;
    rest: string;
  }) {
    return `${serverSeed}:${clientSeed}:${rest}`;
  }

  private _getCheatSeed({
    isSeeding,
    cheatPayoutKey,
    betMode,
  }: {
    isSeeding?: boolean;
    cheatPayoutKey: string;
    betMode: IMathBetMode;
  }): string {
    let rngCheatSeed = '';
    const cheatJsonPath = path.resolve(__dirname, CHEAT_JSON_FILENAME);
    if (isSeeding || !cheatPayoutKey || !existsSync(cheatJsonPath)) {
      return rngCheatSeed;
    }

    const { serverSeed, clientSeed, cheats } = require(cheatJsonPath) as ICheatJsonData;
    const rests = cheats[this._genCheatJsonDataKey({ cheatPayoutKey, betMode })] || [];

    if (rests.length) {
      const rest = rests[Math.floor(Math.random() * rests.length)];
      rngCheatSeed = this._mergeRNGCheatSeed({ serverSeed, clientSeed, rest });
      if (!rngCheatSeed) {
        const msg = `INVALID cheats - cheatPayoutKey: ${cheatPayoutKey}, betMode: ${betMode}, seeds: ${JSON.stringify(rests)}`;
        APP_LOGGER.error({ msg });
        throw new Error(msg);
      }
    }

    return rngCheatSeed;
  }
}
