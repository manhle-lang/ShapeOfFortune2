/* eslint-disable @typescript-eslint/no-require-imports */
import { writeFileSync, appendFileSync } from 'fs';
import { IGameCheatPayout, IGameResultRequest } from '../logic.types';
import { CHEAT_JSON_FILENAME, CheatService, ICheatJsonData } from './cheat.service';
import { BetType, ENGINE_TYPE, ProfileVersion } from '@revenge-games/logic-sharedlibs/dist/types';
import path from 'path';
import { IMathBetMode } from '../math';

/**
 * should apply: seed for special cheats which may take time if pure random
 * should NOT apply: some games(ex: pay anywhere) may need to cheat to show all case as paytable
 * TODO: update this based on game specific
 */
const CHEATS_SEEDING: Partial<IGameCheatPayout> = {
  isTriggerWin: true,
  isTriggerBigWin: true,
  isTriggerMegaWin: true,
  isTriggerSuperMegaWin: true,
  isTriggerFreeSpins: true,
  isTriggerMaxWin: true,
};

const paylines = [1, 2, 3, 4, 5];
const symbolIds = [2, 3, 4, 5, 6, 7, 8, 9, 10];
paylines.forEach((payline) => {
  symbolIds.forEach((symbolId) => {
    const cheatJsonDataKey = `isTriggerSymbols_${symbolId}_payline_${payline}`;
    CHEATS_SEEDING[cheatJsonDataKey as keyof IGameCheatPayout] = true;
  });
});

const numberOfRecordPerCheat = 5;
const timeoutPerCheatSeedingInSecs = 60; // 30s

/**
 * this file will be executed at local for cheats seeding
 */
function main() {
  const cheatJsonData: ICheatJsonData = {
    serverSeed: '',
    clientSeed: '',
    cheats: {},
  };

  Object.keys(CHEATS_SEEDING).forEach((cheatPayoutKey) => {
    if (!CHEATS_SEEDING[cheatPayoutKey as keyof IGameCheatPayout]) {
      // skip key if not enable
      return false;
    }
    const betModes = Object.values(IMathBetMode).filter((v) => typeof v === 'number') as number[];

    for (const betMode of betModes) {
      if (betMode === IMathBetMode.DIAMOND) {
        const meta = CheatService.parseTriggerSymbolsCheat({
          [cheatPayoutKey]: true,
        });
        if (meta && [2, 3].includes(meta.lineId)) {
          // diamond does not support payline 2,3
          continue;
        }
      }
      const startTS = new Date().getTime();
      const isTimeoutExceed = () => {
        const isExceed = new Date().getTime() - startTS >= timeoutPerCheatSeedingInSecs * 1000;
        return isExceed;
      };

      let count = 0;
      while (count < numberOfRecordPerCheat && !isTimeoutExceed()) {
        const cheatRequest: IGameResultRequest = {
          bet: {
            betId: '',
            betLevel: 1,
            betSize: 1,
            type: BetType.NORMAL,
            data: {
              betMode: betMode as IMathBetMode,
            },
          },
          state: {
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
          profile: ProfileVersion.b3,
          cheatPayout: {
            [cheatPayoutKey]: true,
          },
        };
        let cheatService = new CheatService({ body: cheatRequest });
        let data = cheatService._doPlay(true);

        if (cheatPayoutKey === 'isTriggerMaxWin') {
          while (!data.result.bet.isEndRound && !isTimeoutExceed()) {
            cheatRequest.state = data.result.state;
            cheatService = new CheatService({ body: cheatRequest });
            data = cheatService._doPlay(true);
          }
          if (!data.result.result.isMaxWin) {
            continue; // retry
          }
        }

        // Update data when matching condition
        const cheatJsonDataKey = cheatService._genCheatJsonDataKey({
          cheatPayoutKey,
          betMode: data.betMode,
        });
        const { serverSeed, clientSeed, rest } = cheatService._parseRNGCheatSeed(data.rngCheatSeed);

        if (!cheatJsonData.serverSeed) {
          cheatJsonData.serverSeed = serverSeed;
        } else if (cheatJsonData.serverSeed !== serverSeed) {
          throw new Error('Serve seed not match');
        }
        if (!cheatJsonData.clientSeed) {
          cheatJsonData.clientSeed = clientSeed;
        } else if (cheatJsonData.clientSeed !== clientSeed) {
          throw new Error('Client seed not match');
        }
        if (!cheatJsonData.cheats[cheatJsonDataKey]) {
          cheatJsonData.cheats[cheatJsonDataKey] = [rest];
        } else {
          appendFileSync('cheats-tmp.log', `${cheatJsonDataKey}     ${rest}\n`)
          cheatJsonData.cheats[cheatJsonDataKey]?.push(rest);
        }
        count++;
        console.log(`Generated cheat for ${cheatJsonDataKey}: ${count}/${numberOfRecordPerCheat}`);
      }
    }
  });

  // Write data to file
  writeFileSync(
    path.resolve(__dirname, CHEAT_JSON_FILENAME),
    JSON.stringify(cheatJsonData, null, 1),
  );
}

main();
