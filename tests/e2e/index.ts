import { runEndToEndWithHandler } from '@revenge-games/logic-sharedlibs';
import { ProfileVersion } from '@revenge-games/logic-sharedlibs/dist/types';
import path from 'path';
import * as packageJson from '../../package.json';
import { GetGameConfigs } from '../../src/logic.config';

const gameConfigs = GetGameConfigs();

runEndToEndWithHandler({
  workingDir: path.resolve(__dirname),
  workerOpts: {
    packageJson,
    workerPath: path.resolve(__dirname, 'worker.js'),
    disableLogFlush: false,
  },
  gameConfig: {
    baseBet: gameConfigs.baseBet,
    maxMultiplier: gameConfigs.maxWin,
  },
  reportOpts: {
    reportPath: path.join(__dirname, '../../../e2e-report.html'),
  },
  defaultEngineConfig: {
    profile: ProfileVersion.b3,
  },
});
