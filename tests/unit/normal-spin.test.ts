import { BetType, ENGINE_TYPE, ProfileVersion } from '@revenge-games/logic-sharedlibs/dist/types';
import { LogicService } from '../../src/logic.service';
import { IGameResultRequest } from '../../src/logic.types';
import { IMathBetMode } from '../../src/math';

describe('NORMAL SPIN', () => {
  it('Normal spin - play', () => {
    const playRequest: IGameResultRequest = {
      bet: {
        betId: '',
        betLevel: 1,
        betSize: 1,
        type: BetType.NORMAL,
        data: {
          betMode: Math.round(Math.random() * 2) as IMathBetMode
          // betMode: IMathBetMode.DIAMOND
        },
      },
      profile: ProfileVersion.b3,
      cheatPayout: {},
      state: {
        engine: ENGINE_TYPE.ANYTHING,
        freeSpins: 0,
        freeSpinsWinAmount: 0,
        profile: ProfileVersion.b3,
        data: {},
      },
    };
    const LOGIC_SERVICE = LogicService.GetInstance({} as any);
    const result = LOGIC_SERVICE.refreshInstance({
      body: playRequest,
    } as any).doPlay();
    console.debug(JSON.stringify(result, null, 2));
  });
});
