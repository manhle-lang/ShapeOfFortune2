import { GetServerHandlers } from '@revenge-games/logic-sharedlibs/dist/server/app.handler';
import {
  ExpressRequest,
  IPlayRequest,
} from '@revenge-games/logic-sharedlibs/dist/server/app.type';
import { ENGINE_TYPE } from '@revenge-games/logic-sharedlibs/dist/types';
import { CheatService } from './cheats/cheat.service';
import { LogicService } from './logic.service';
import { AppSecret } from '@revenge-games/logic-sharedlibs';
import { IGameResultRequest } from './logic.types';

const serverHandlers = GetServerHandlers({
  service: LogicService,
  cheatService: CheatService,
  getLogicInstance: (request: ExpressRequest) => {
    if (request.method.toLowerCase() === 'post') {
      const playReq = request?.body as IPlayRequest;
      if (playReq?.state?.engine === ENGINE_TYPE.ENGINE) {
        throw new Error('Incompatible Engine');
      }

      const supposedCheat = playReq as IGameResultRequest;
      if (
        supposedCheat?.state?.data?.cheatPayout &&
        Object.keys(supposedCheat.state?.data?.cheatPayout).length
      ) {
        // support for cheats which need do continuos cheat
        supposedCheat.cheatPayout = supposedCheat.state.data.cheatPayout;
        if (supposedCheat?.cheatPayout && Object.keys(supposedCheat?.cheatPayout).length) {
          return CheatService.GetInstance(request);
        }
      }

      if (request.url === '/api/v1/result') {
        return CheatService.GetInstance(request);
      }
    }

    return LogicService.GetInstance(request);
  },
  serveHttp: !AppSecret.IS_LAMDA,
});
// eslint-disable-next-line @typescript-eslint/no-explicit-any
export const handler: any = serverHandlers.serverlessHandler;
