import { BetConfigs } from './logic.config';
import { IGamePlayRequest } from './logic.types';

export const getBetConfigItem = (request: IGamePlayRequest) => {
  const result = BetConfigs.find((e) => e.canUseThisFeature(request.bet));
  if (!result) throw new Error(`INVALID betConfigs`);
  return result;
};

export const getBetAmountForCalculateBracket = (
  request: IGamePlayRequest,
  totalBetAmount: number,
) => {
  const betConfigItem = getBetConfigItem(request);
  return betConfigItem.isCalculateBracketWithBetCost
    ? totalBetAmount
    : totalBetAmount / betConfigItem.betCost;
};
