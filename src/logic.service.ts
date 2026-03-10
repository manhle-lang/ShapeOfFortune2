import {
  ENGINE_TYPE,
  GetProfileRTPPercent,
  ProfileVersion,
} from '@revenge-games/logic-sharedlibs/dist/types';
import { GameMath } from './math';
import { GetAvailableProfiles, GetGameConfigs } from './logic.config';
import {
  IBaseService,
  IDescriptionRequest,
  IGetInfoResponse,
} from '@revenge-games/logic-sharedlibs/dist/server/app.type';
import { IGameDescriptionResponse, IGamePlayRequest, IGamePlayResponse } from './logic.types';
import { getBetConfigItem } from './logic.helper';
import { LogicBuilder } from './logic.builder';
const RESPONSE_BUILDER = new LogicBuilder();

export class LogicService extends IBaseService {
  getInfo(): IGetInfoResponse {
    const gameBaseConfig = GetGameConfigs();
    return {
      availableProfiles: GetAvailableProfiles(),
      name: gameBaseConfig.name,
      version: gameBaseConfig.version,
      baseBet: gameBaseConfig.baseBet,
    };
  }

  doInit(): IGameDescriptionResponse {
    const { profile } = this.REQUEST.query as IDescriptionRequest;
    const config = GetGameConfigs(profile as ProfileVersion);

    return {
      baseBet: config.baseBet,
      code: config.code,
      build: '1.0.0',
      name: config.name,
      version: config.version,
      matrixDefault: config.matrixDefault,
      maxWin: config.maxWin,
      payTable: config.payTable,
      profile: config.profile,
      reelSize: config.reelSize,
      winBrackets: config.winBrackets,
      freeSpin: config.freeSpin,
      payline: config.payline,
      selectedPayLine: config.selectedPayLine,
      circleIndex: config.circleIndex,
      triangleIndex: config.triangleIndex,
      diamondIndex: config.diamondIndex,
      symbols: config.symbols,
    };
  }

  doPlay(): IGamePlayResponse {
    const playReq = this.REQUEST.body as IGamePlayRequest;
    const { state, bet } = playReq;
    if (state?.engine === ENGINE_TYPE.ENGINE) {
      throw new Error('Incompatible Engine');
    }

    // if (Object.keys(playReq.state?.data ?? {}).length === 0) playReq.state.freeSpinsWinAmount = 0; // Why we need this?

    const mathSpinResult = GameMath.DoSpin({
      betLevel: bet.betLevel,
      betSize: bet.betSize,
      betMode: getBetConfigItem(playReq).betMode,
      profile: GetProfileRTPPercent(playReq.profile),
      parentSpinResult: playReq.state?.data?.parentSpinResult,
      pastSpinResult: playReq.state?.data?.pastSpinResult,
      featureBuy: playReq.bet.data?.isBuyFeature,
    });

    // // DO BUILDING RESULTS
    const result = RESPONSE_BUILDER.build({ mathSpinResult, playReq });
    return result;
  }
}
