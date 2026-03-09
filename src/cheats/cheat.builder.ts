import { LogicBuilder } from '../logic.builder';
import { IGameCheatPayout, IGamePlayRequest, IGamePlayResponseState } from '../logic.types';
import { IMathSpinResult } from '../math';

/**
 * NOTICE: NO CLASS SCOPED VARIABLE HERE, ALL FUNCTION INVOKE COME WITH PARAMETTERS
 */
export class CheatBuilder extends LogicBuilder {
  buildStateResult({
    playReq,
    mathSpinResult,
    cheatPayout,
  }: {
    playReq: IGamePlayRequest;
    mathSpinResult: IMathSpinResult;
    cheatPayout?: IGameCheatPayout;
  }): IGamePlayResponseState {
    const newState = super.buildStateResult({ playReq, mathSpinResult });
    if (newState.data) {
      newState.data.cheatPayout = cheatPayout;
    }
    return newState;
  }
}
