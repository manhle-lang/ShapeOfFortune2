import { BetType, ENGINE_TYPE } from '@revenge-games/logic-sharedlibs/dist/types';
import { GetWinTitle } from '@revenge-games/logic-sharedlibs/dist/wintitles/utils';
import Decimal from 'decimal.js';
import { GetGameConfigs } from './logic.config';
import { IMathSpinResult } from './math';
import {
  IGameCheatPayout,
  IGamePlayRequest,
  IGamePlayResponse,
  IGamePlayResponseBet,
  IGamePlayResponseResult,
  IGamePlayResponseState,
} from './logic.types';
import { getBetAmountForCalculateBracket } from './logic.helper';

/**
 * NOTICE: NO CLASS SCOPED VARIABLE HERE, ALL FUNCTION INVOKE COME WITH PARAMETTERS
 */
export class LogicBuilder {
  build({
    mathSpinResult,
    playReq,
    cheatPayout,
  }: {
    mathSpinResult: IMathSpinResult;
    playReq: IGamePlayRequest;
    cheatPayout?: IGameCheatPayout;
  }): IGamePlayResponse {
    const stateResult = this.buildStateResult({ mathSpinResult, playReq, cheatPayout });
    const betResult = this.buildBetResult({
      playReq,
      mathSpinResult,
      stateResult,
    });
    const spinResult = this.buildSpinResult({
      playReq,
      mathSpinResult,
      stateResult,
      betResult,
    });

    if (betResult.isEndRound) {
      stateResult.data = {}
      stateResult.engine = ENGINE_TYPE.ANYTHING;
      stateResult.freeSpins = 0;
    }

    return {
      profile: playReq.profile || '',
      state: stateResult,
      bet: betResult,
      result: spinResult,
    };
  }

  buildStateResult({
    playReq,
    mathSpinResult,
  }: {
    playReq: IGamePlayRequest;
    mathSpinResult: IMathSpinResult;
    cheatPayout?: IGameCheatPayout;
  }): IGamePlayResponseState {
    const newState: IGamePlayResponseState = Object.keys(playReq.state || {}).length
      ? (structuredClone(playReq.state) as IGamePlayResponseState)
      : {
          data: {},
          engine: ENGINE_TYPE.ANYTHING,
          freeSpins: mathSpinResult.freeSpins,
          freeSpinsWinAmount: 0,
          profile: playReq.profile || '',
          // v: 0, // TODO: check it
        };
    if (!newState.data) newState.data = {};
    newState.freeSpins = mathSpinResult.freeSpins;

    const winAmount = +mathSpinResult.payout.toFixed(2);
    if (!newState.data?.parentSpinResult && mathSpinResult.freeSpins) {
      newState.engine = ENGINE_TYPE.RANDOM;
      newState.data.parentSpinResult = mathSpinResult;
      newState.data.parentBetId = playReq.bet.betId;
    }
    if (mathSpinResult.freeSpins) {
      newState.data.pastSpinResult = mathSpinResult;
    }
    if (mathSpinResult.freeSpins || playReq.state?.freeSpins) {
      if (playReq.state?.data?.parentBetId) {
        newState.data.parentBetId = playReq.state.data.parentBetId;
      }
      newState.freeSpinsWinAmount = new Decimal(newState.freeSpinsWinAmount)
        .add(winAmount)
        .toNumber();
    }

    return newState;
  }

  buildBetResult({
    playReq,
    mathSpinResult,
    stateResult,
  }: {
    playReq: IGamePlayRequest;
    mathSpinResult: IMathSpinResult;
    stateResult: IGamePlayResponseState;
  }): IGamePlayResponseBet {
    const winAmount = +mathSpinResult.payout.toFixed(2);
    const resultBetType =
      !playReq.state?.data?.parentSpinResult?.freeSpins &&
      stateResult.data?.parentSpinResult?.freeSpins
        ? BetType.WIN_FREESPIN
        : stateResult.data?.parentSpinResult?.freeSpins
          ? BetType.FREE_SPIN
          : BetType.NORMAL;

    const result: IGamePlayResponseBet = {
      data: playReq.bet.data,
      betId: playReq.bet.betId,
      betLevel: playReq.bet.betLevel,
      betSize: playReq.bet.betSize,
      betAmount: +mathSpinResult.betAmount.toFixed(2),
      isEndRound: mathSpinResult.isEndRound,
      type: resultBetType,
      winAmount: winAmount,
      winFreeSpins: mathSpinResult.winFreeSpins,
      parentBetId: stateResult.data?.parentBetId,
    };
    return result;
  }

  buildSpinResult({
    playReq,
    mathSpinResult,
    stateResult,
    betResult,
  }: {
    playReq: IGamePlayRequest;
    mathSpinResult: IMathSpinResult;
    stateResult: IGamePlayResponseState;
    betResult: IGamePlayResponseBet;
  }): IGamePlayResponseResult {
    const winMultiplier =
      betResult.winAmount /
      getBetAmountForCalculateBracket(
        playReq,
        betResult.betAmount || stateResult.data?.parentSpinResult?.betAmount || 1,
      );
    const gameConfigs = GetGameConfigs();
    const result: IGamePlayResponseResult = {
      baseBet: gameConfigs.baseBet,
      betAmount: betResult.betAmount,
      bracket: GetWinTitle(gameConfigs.winBrackets, winMultiplier),
      isEndRound: betResult.isEndRound,
      isFreeSpin: betResult.type === BetType.FREE_SPIN,
      matrix: mathSpinResult.resultVector,
      result: mathSpinResult.winLines,
      type: betResult.type,
      winAmount: betResult.winAmount,
      freeSpins: stateResult.freeSpins,
      winFreeSpins: betResult.winFreeSpins,
      winFreeSpinsAmount: stateResult.freeSpinsWinAmount,
      betSize: mathSpinResult.betSize,
      betLevel: mathSpinResult.betLevel,
      multiplier: mathSpinResult.multiplier,
      multiplierReelVector: mathSpinResult.multiplierReelVector,
      reelMode: mathSpinResult.reelMode,
      selectionMode: mathSpinResult.selectionMode,
      isMaxWin: mathSpinResult.isMaxWin,
    };

    return result;
  }
}
