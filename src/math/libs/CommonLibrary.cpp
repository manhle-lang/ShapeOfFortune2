#include "CommonLibrary.h"
#include "RgRNG.cpp"

thread_local std::random_device          rand_number;
thread_local std::mt19937   generator(rand_number());

// ***********************************
//  	     COMMON LIBRARY
// ***********************************
#pragma region Reels Functions
void CommonLibrary::GenerateResult(const std::vector<std::vector<int>>& reelStrip, const std::vector<int>& reelWindowSize, std::vector<int>& resultVector)
{
	for (size_t col = 0; col < reelWindowSize.size(); col++)
	{
		int randomizeIndex = RandomInt(0, reelStrip[col].size());

		for (int row = 0; row < reelWindowSize[col]; row++)
		{
			resultVector[GetIndexFromColRow(col, row, reelWindowSize)] = reelStrip[col][randomizeIndex];

			randomizeIndex++;

			if (randomizeIndex >= static_cast<int>(reelStrip[col].size()))
				randomizeIndex = 0;
		}
	}
}

void CommonLibrary::GenerateResultWithStickyResult(const std::vector<std::vector<int>>& reelStrip, const std::vector<int>& reelWindowSize, std::vector<int>& resultVector, const std::vector<bool>& stickyVector)
{
	for (size_t col = 0; col < reelWindowSize.size(); col++)
	{
		int randomizeIndex = RandomInt(0, reelStrip[col].size());

		for (int row = 0; row < reelWindowSize[col]; row++)
		{
			int index = GetIndexFromColRow(col, row, reelWindowSize);

			// If there isn't any existing sticky symbol then generate result
			if (!stickyVector[index])
			{
				resultVector[index] = reelStrip[col][randomizeIndex];

				randomizeIndex++;

				if (randomizeIndex >= static_cast<int>(reelStrip[col].size()))
					randomizeIndex = 0;
			}
		}
	}
}

void CommonLibrary::DestroyWinningSymbol(const std::vector<bool>& destroyVector, std::vector<int>& resultVector, const std::vector<bool>& stickyVector)
{
	for (size_t i = 0; i < resultVector.size(); i++)
	{
		if (destroyVector[i] && !stickyVector[i])
			resultVector[i] = blankIndex;
	}
}

void CommonLibrary::RefillResult(const std::vector<std::vector<int>>& reelStrip, const std::vector<int>& reelWindowSize, std::vector<int>& resultVector)
{
	for (size_t col = 0; col < reelWindowSize.size(); col++)
	{
		int randomizeIndex = RandomInt(0, reelStrip[col].size());

		for (int row = 0; row < reelWindowSize[col]; row++)
		{
			if (resultVector[GetIndexFromColRow(col, row, reelWindowSize)] == blankIndex)
			{
				resultVector[GetIndexFromColRow(col, row, reelWindowSize)] = reelStrip[col][randomizeIndex];

				randomizeIndex++;

				if (randomizeIndex >= static_cast<int>(reelStrip[col].size()))
					randomizeIndex = 0;
			}
		}
	}
}

void CommonLibrary::CollapseResult(const std::vector<int>& reelWindowSize, std::vector<int>& resultVector, const std::vector<bool>& stickyVector)
{
	std::vector<int> tempResult;

	for (size_t col = 0; col < reelWindowSize.size(); col++)
	{
		for (int row = reelWindowSize[col] - 1; row >= 0; row--)
		{
			int index = GetIndexFromColRow(col, row, reelWindowSize);

			if (!stickyVector[index] && resultVector[index] != blankIndex)
			{
				tempResult.push_back(resultVector[index]);
				resultVector[index] = blankIndex;
			}
		}

		for (int row = reelWindowSize[col] - 1; row >= 0; row--)
		{
			if (tempResult.size() > 0)
			{
				int index = GetIndexFromColRow(col, row, reelWindowSize);

				if (resultVector[index] == blankIndex)
				{
					resultVector[index] = tempResult.front();
					tempResult.erase(tempResult.begin());
				}
			}
		}
	}
}

void CommonLibrary::CollapseVectorBasedOnResult(const int& endValue, const std::vector<int>& reelWindowSize, std::vector<int>& collapseVector, const std::vector<int>& resultVector, const std::vector<bool>& stickyVector)
{
	std::vector<int> tempResult;

	for (size_t col = 0; col < reelWindowSize.size(); col++)
	{
		for (int row = reelWindowSize[col] - 1; row >= 0; row--)
		{
			int index = GetIndexFromColRow(col, row, reelWindowSize);

			if (!stickyVector[index] && resultVector[index] != blankIndex)
			{
				tempResult.push_back(collapseVector[index]);
				collapseVector[index] = blankIndex;
			}
		}

		for (int row = reelWindowSize[col] - 1; row >= 0; row--)
		{
			if (tempResult.size() > 0)
			{
				int index = GetIndexFromColRow(col, row, reelWindowSize);

				if (!stickyVector[index])
				{
					collapseVector[index] = tempResult.front();
					tempResult.erase(tempResult.begin());
				}
			}
			else
			{
				int index = GetIndexFromColRow(col, row, reelWindowSize);
				collapseVector[index] = endValue;
			}
		}
	}
}

// void CommonLibrary::CollapseVectorBasedOnResult(const std::vector<int>& reelWindowSize, std::vector<bool>& collapseVector, const std::vector<int>& resultVector, const std::vector<bool>& stickyVector)
// {
// 	std::vector<int> tempResult;
//
// 	for (int col = 0; col < reelWindowSize.size(); col++)
// 	{
// 		for (int row = reelWindowSize[col] - 1; row >= 0; row--)
// 		{
// 			int index = GetIndexFromColRow(col, row, reelWindowSize);
//
// 			if (!stickyVector[index] && resultVector[index] != blankIndex)
// 			{
// 				tempResult.push_back(collapseVector[index]);
// 				collapseVector[index] = false;
// 			}
// 		}
//
// 		for (int row = reelWindowSize[col] - 1; row >= 0; row--)
// 		{
// 			if (tempResult.size() > 0)
// 			{
// 				int index = GetIndexFromColRow(col, row, reelWindowSize);
//
// 				if (resultVector[index] == blankIndex)
// 				{
// 					collapseVector[index] = tempResult.front();
// 					tempResult.erase(tempResult.begin());
// 				}
// 			}
// 		}
// 	}
// }

void CommonLibrary::ChangeVectorBasedOnWinningResult(const int& endValue, std::vector<int>& changeVector, const std::vector<bool>& stickyVector, const std::vector<bool>& winningVector)
{
	for (size_t i = 0; i < changeVector.size(); i++)
	{
		if (winningVector[i] && !stickyVector[i])
			changeVector[i] = endValue;
	}
}

void CommonLibrary::RevealMystery(const std::vector<int>& symbolWeightage,std::vector<int>& resultVector)
{
	int generateIndex = GetWeightedIndex(symbolWeightage);

	for (size_t i = 0; i < resultVector.size(); i++)
	{
		if (resultVector[i] == mysteryIndex)
			resultVector[i] = generateIndex;
	}
}

void CommonLibrary::RevealRandom(const std::vector<int>& symbolWeightage, std::vector<int>& resultVector)
{
	for (size_t i = 0; i < resultVector.size(); i++)
	{
		if (resultVector[i] == randomIndex)
			resultVector[i] = GetWeightedIndex(symbolWeightage);
	}
}

const std::vector<bool>& CommonLibrary::GetWinningVector()
{
	return winningVector;
}

const std::vector<CommonLibrary::Winline>& CommonLibrary::GetPayLineData()
{
	return payLineData;
}

#pragma endregion

#pragma region Payout Functions
int CommonLibrary::GetPayLinePayout(const int& selectedPayLine, const std::vector<std::vector<int>>& payline, const std::vector<std::vector<int>>& paytable, const std::vector<int>& reelWindowSize, const std::vector<int>& resultVector)
{
	// Initialize
	int totalPayout = 0;
	winningVector.clear();
	winningVector.resize(GetVectorSum(reelWindowSize));
	payLineData.clear();

	// Loop through each payline
	for (int line = 0; line < selectedPayLine; line++)
	{
		int normalOAK = 1;		// Of a kind for normal symbol
		int wildOAK = 1;		// Of a kind for wild symbol

		// Read the first symbol of this payline on first column
		int firstSymbol = resultVector[GetIndexFromColRow(0, payline[line][0], reelWindowSize)];

		// Ignore scatter symbol
		if (firstSymbol != scatterIndex)
		{
			// Loop through the remaining columns and check if it form a payline
			for (size_t col = 1; col < reelWindowSize.size(); col++)
			{
				int subSymbol = resultVector[GetIndexFromColRow(col, payline[line][col], reelWindowSize)];

				// If first symbol is a wild
				if (firstSymbol == wildIndex)
				{
					// If subsequent symbol is still a wild
					if (subSymbol == wildIndex)
						wildOAK++;

					// Assign the first symbol index to be the subsequent symbol index
					else
					{
						firstSymbol = subSymbol;
						normalOAK = wildOAK + 1;
					}
				}
				else
				{
					// Check if the subsequent symbol matched with the first symbol
					if (subSymbol == firstSymbol || subSymbol == wildIndex)
						normalOAK++;
					else
						break;
				}
			}

			// Compute the win based on the payline
			int wildPayout = 0;
			int normalPayout = 0;

			wildPayout = paytable[wildOAK - 1][wildIndex];

			// Check if it's a valid symbol with payout
			if (firstSymbol < static_cast<int>(paytable[0].size()))
				normalPayout = paytable[normalOAK - 1][firstSymbol];
			else
				normalPayout = 0;

			int payout = std::max(wildPayout, normalPayout);
			totalPayout += payout;

			// Update the winning vector to be displayed on front end
			if (wildPayout + normalPayout > 0)
			{
				CommonLibrary::Winline tempPaylineData;
				tempPaylineData.symbolId = firstSymbol; // For front end data
				tempPaylineData.lineId = line + 1; // For front end data

				int winOAK = normalOAK;			// If it's normal symbol payout
				if (wildPayout > normalPayout)	// If it's wild symbol payout
				{
					winOAK = wildOAK;
					tempPaylineData.symbolId = wildIndex; // For front end data
				}

				int tempIndex = GetIndexFromColRow(0, payline[line][0], reelWindowSize);
				winningVector[tempIndex] = true;

				tempPaylineData.winPosition.push_back(tempIndex); // For front end data

				for (int col = 1; col < winOAK; col++)
				{
					tempIndex = GetIndexFromColRow(col, payline[line][col], reelWindowSize);
					winningVector[tempIndex] = true;

					tempPaylineData.winPosition.push_back(tempIndex); // For front end data
				}

				tempPaylineData.winAmountWithoutMultiplier = (double)payout; // For front end data
				payLineData.push_back(tempPaylineData);	    // For front end data
			}
		}
	}

	return totalPayout;
}

int CommonLibrary::GetPayLinePayoutMultiplierAdditive(const int& selectedPayLine, const std::vector<std::vector<int>>& payline, const std::vector<std::vector<int>>& paytable, const std::vector<int>& reelWindowSize, const std::vector<int>& resultVector, const std::vector<int>& multiplierVector)
{
	// Initialize
	int totalPayout = 0;
	winningVector.clear();
	winningVector.resize(GetVectorSum(reelWindowSize));

	// Loop through each payline
	for (int line = 0; line < selectedPayLine; line++)
	{
		int normalOAK = 1;			// Of a kind for normal symbol
		int wildOAK = 1;			// Of a kind for wild symbol
		int multiplier = 1;			// Multiplier of this payline for normal symbol of a kind pay
		int wildMultiplier = 1;		// Multiplier of this payline for wild symbol of a kind pay

		// Read the first symbol of this payline on first column
		int firstIndex = GetIndexFromColRow(0, payline[line][0], reelWindowSize);
		int firstSymbol = resultVector[firstIndex];

		// Ignore scatter symbol
		if (firstSymbol != scatterIndex)
		{
			// If this symbol have multiplier
			if (multiplierVector[firstIndex] != 0)
			{
				multiplier = multiplierVector[firstIndex];

				// If it's wild then update the wild multiplier
				if (firstSymbol == wildIndex)
					wildMultiplier = multiplierVector[firstIndex];
			}

			// Loop through the remaining columns and check if it form a payline
			for (size_t col = 1; col < reelWindowSize.size(); col++)
			{
				int subIndex = GetIndexFromColRow(col, payline[line][col], reelWindowSize);
				int subSymbol = resultVector[subIndex];

				// If first symbol is a wild
				if (firstSymbol == wildIndex)
				{
					// If subsequent symbol is still a wild
					if (subSymbol == wildIndex)
					{
						wildOAK++;

						// If this symbol have multiplier then update the multiplier
						if (multiplierVector[subIndex] != 0)
						{
							if (multiplier == 1)
								multiplier = multiplierVector[subIndex];
							else
								multiplier += multiplierVector[subIndex];

							if (wildMultiplier == 1)
								wildMultiplier = multiplierVector[subIndex];
							else
								wildMultiplier += multiplierVector[subIndex];
						}
					}

					// Assign the first symbol index to be the subsequent symbol index
					else
					{
						firstSymbol = subSymbol;
						normalOAK = wildOAK + 1;

						// If this symbol have multiplier then update the multiplier
						if (multiplierVector[subIndex] != 0)
						{
							if (multiplier == 1)
								multiplier = multiplierVector[subIndex];
							else
								multiplier += multiplierVector[subIndex];
						}
					}
				}
				else
				{
					// Check if the subsequent symbol matched with the first symbol
					if (subSymbol == firstSymbol || subSymbol == wildIndex)
					{
						normalOAK++;

						// If this symbol have multiplier then update the multiplier
						if (multiplierVector[subIndex] != 0)
						{
							if (multiplier == 1)
								multiplier = multiplierVector[subIndex];
							else
								multiplier += multiplierVector[subIndex];
						}
					}
					else
						break;
				}
			}

			// Compute the win based on the payline
			int wildPayout = 0;
			int normalPayout = 0;

			wildPayout = paytable[wildOAK - 1][wildIndex] * wildMultiplier;

			// Check if it's a valid symbol with payout
			if (firstSymbol < static_cast<int>(paytable[0].size()))
				normalPayout = paytable[normalOAK - 1][firstSymbol] * multiplier;
			else
				normalPayout = 0;

			totalPayout += std::max(wildPayout, normalPayout);

			// Update the winning vector to be displayed on front end
			if (wildPayout + normalPayout > 0)
			{
				int winOAK = normalOAK;			// If it's normal symbol payout
				if (wildPayout > normalPayout)	// If it's wild symbol payout
					winOAK = wildOAK;

				winningVector[GetIndexFromColRow(0, payline[line][0], reelWindowSize)] = true;
				for (int col = 1; col < winOAK; col++)
				{
					winningVector[GetIndexFromColRow(col, payline[line][col], reelWindowSize)] = true;
				}
			}
		}
	}

	return totalPayout;
}

int CommonLibrary::GetPayLinePayoutMultiplierMultiplicative(const int& selectedPayLine, const std::vector<std::vector<int>>& payline, const std::vector<std::vector<int>>& paytable, const std::vector<int>& reelWindowSize, const std::vector<int>& resultVector, const std::vector<int>& multiplierVector)
{
	// Initialize
	int totalPayout = 0;
	winningVector.clear();
	winningVector.resize(GetVectorSum(reelWindowSize));

	// Loop through each payline
	for (int line = 0; line < selectedPayLine; line++)
	{
		int normalOAK = 1;			// Of a kind for normal symbol
		int wildOAK = 1;			// Of a kind for wild symbol
		int multiplier = 1;			// Multiplier of this payline for normal symbol of a kind pay
		int wildMultiplier = 1;		// Multiplier of this payline for wild symbol of a kind pay

		// Read the first symbol of this payline on first column
		int firstIndex = GetIndexFromColRow(0, payline[line][0], reelWindowSize);
		int firstSymbol = resultVector[firstIndex];

		// Ignore scatter index
		if (firstSymbol != scatterIndex)
		{
			// If this symbol have multiplier
			if (multiplierVector[firstIndex] != 0)
			{
				multiplier = multiplierVector[firstIndex];

				// If it's wild then update the wild multiplier
				if (firstSymbol == wildIndex)
					wildMultiplier = multiplierVector[firstIndex];
			}

			// Loop through the remaining columns and check if it form a payline
			for (size_t col = 1; col < reelWindowSize.size(); col++)
			{
				int subIndex = GetIndexFromColRow(col, payline[line][col], reelWindowSize);
				int subSymbol = resultVector[subIndex];

				// If first symbol is a wild
				if (firstSymbol == wildIndex)
				{
					// If subsequent symbol is still a wild
					if (subSymbol == wildIndex)
					{
						wildOAK++;

						// If this symbol have multiplier then update the multiplier
						if (multiplierVector[subIndex] != 0)
						{
							if (multiplier == 1)
								multiplier = multiplierVector[subIndex];
							else
								multiplier *= multiplierVector[subIndex];

							if (wildMultiplier == 1)
								wildMultiplier = multiplierVector[subIndex];
							else
								wildMultiplier *= multiplierVector[subIndex];
						}
					}

					// Assign the first symbol index to be the subsequent symbol index
					else
					{
						firstSymbol = subSymbol;
						normalOAK = wildOAK + 1;

						// If this symbol have multiplier then update the multiplier
						if (multiplierVector[subIndex] != 0)
						{
							if (multiplier == 1)
								multiplier = multiplierVector[subIndex];
							else
								multiplier *= multiplierVector[subIndex];
						}
					}
				}
				else
				{
					// Check if the subsequent symbol matched with the first symbol
					if (subSymbol == firstSymbol || subSymbol == wildIndex)
					{
						normalOAK++;

						// If this symbol have multiplier then update the multiplier
						if (multiplierVector[subIndex] != 0)
						{
							if (multiplier == 1)
								multiplier = multiplierVector[subIndex];
							else
								multiplier *= multiplierVector[subIndex];
						}
					}
					else
						break;
				}
			}

			// Compute the win based on the payline
			int wildPayout = 0;
			int normalPayout = 0;

			wildPayout = paytable[wildOAK - 1][wildIndex] * wildMultiplier;

			// Check if it's a valid symbol with payout
			if (firstSymbol < static_cast<int>(paytable[0].size()))
				normalPayout = paytable[normalOAK - 1][firstSymbol] * multiplier;
			else
				normalPayout = 0;

			totalPayout += std::max(wildPayout, normalPayout);

			// Update the winning vector to be displayed on front end
			if (wildPayout + normalPayout > 0)
			{
				int winOAK = normalOAK;			// If it's normal symbol payout
				if (wildPayout > normalPayout)	// If it's wild symbol payout
					winOAK = wildOAK;

				winningVector[GetIndexFromColRow(0, payline[line][0], reelWindowSize)] = true;
				for (int col = 1; col < winOAK; col++)
				{
					winningVector[GetIndexFromColRow(col, payline[line][col], reelWindowSize)] = true;
				}
			}
		}
	}

	return totalPayout;
}

int CommonLibrary::GetPayWayPayout(const std::vector<std::vector<int>>& paytable, const std::vector<int>& reelWindowSize, const std::vector<int>& resultVector)
{
	// Initialize
	int totalPayout = 0;
	winningVector.clear();
	winningVector.resize(GetVectorSum(reelWindowSize));

	// Loop through the symbol on each row
	for (int row = 0; row < reelWindowSize[0]; row++)
	{
		int OAK = 1;		// Of a kind for normal symbol
		int ways = 1;		// Pay ways computed

		// Read the first symbol of this row on first column
		int firstSymbol = resultVector[GetIndexFromColRow(0, row, reelWindowSize)];

		// If it's scatter then skip
		if (firstSymbol != scatterIndex)
		{
			// Loop through the remaining columns and check if it form a payway
			for (size_t col = 1; col < reelWindowSize.size(); col++)
			{
				int subSymbolCount = GetSymbolCountInCol(col, firstSymbol, reelWindowSize, resultVector, true);

				if (subSymbolCount == 0)
					break;
				else
				{
					OAK++;
					ways *= subSymbolCount;
				}
			}

			// Compute the win based on the pay ways and number of kind
			int normalPayout = 0;

			// Check if it's a valid symbol with payout
			if (firstSymbol < static_cast<int>(paytable[0].size()))
				normalPayout = paytable[OAK - 1][firstSymbol] * ways;
			else
				normalPayout = 0;

			totalPayout += normalPayout;

			// Update the winning vector to be displayed on front end
			if (normalPayout > 0)
			{
				winningVector[GetIndexFromColRow(0, row, reelWindowSize)] = true;
				for (int col = 1; col < OAK; col++)
				{
					for (int row = 0; row < reelWindowSize[col]; row++)
					{
						int index = GetIndexFromColRow(col, row, reelWindowSize);

						if (resultVector[index] == firstSymbol || resultVector[index] == wildIndex)
							winningVector[index] = true;
					}
				}
			}
		}
	}

	return totalPayout;
}

int CommonLibrary::GetPayWayPayoutMultiplierMultiplicative(const std::vector<std::vector<int>>& paytable, const std::vector<int>& reelWindowSize, const std::vector<int>& resultVector, const std::vector<int>& multiplierVector)
{
	// Initialize
	int totalPayout = 0;
	winningVector.clear();
	winningVector.resize(GetVectorSum(reelWindowSize));

	// Loop through the symbol on each row
	for (int row = 0; row < reelWindowSize[0]; row++)
	{
		int OAK = 1;		// Of a kind for normal symbol
		int ways = 1;		// Pay ways computed

		// Read the first symbol of this row on first column
		int firstIndex = GetIndexFromColRow(0, row, reelWindowSize);
		int firstSymbol = resultVector[firstIndex];
		ways *= (multiplierVector[firstIndex] != 0) ? multiplierVector[firstIndex] : 1;

		// If it's scatter then skip
		if (firstSymbol != scatterIndex)
		{
			// Loop through the remaining columns and check if it form a payway
			for (size_t col = 1; col < reelWindowSize.size(); col++)
			{
				int subSymbolCount = GetSymbolCountInCol(col, firstSymbol, reelWindowSize, resultVector, true);

				if (subSymbolCount == 0)
					break;
				else
				{
					OAK++;
					int multiplier = 0;

					// Sum the payways based on the multiplier vector
					for (int row = 0; row < reelWindowSize[col]; row++)
					{
						int index = GetIndexFromColRow(col, row, reelWindowSize);
						if (resultVector[index] == firstSymbol || (resultVector[index] == wildIndex))
							multiplier += (multiplierVector[index] != 0) ? multiplierVector[index] : 1;
					}

					ways *= multiplier;
				}
			}

			// Compute the win based on the pay ways and number of kind
			int normalPayout = 0;

			// Check if it's a valid symbol with payout
			if (firstSymbol < static_cast<int>(paytable[0].size()))
				normalPayout = paytable[OAK - 1][firstSymbol] * ways;
			else
				normalPayout = 0;

			totalPayout += normalPayout;

			// Update the winning vector to be displayed on front end
			if (normalPayout > 0)
			{
				winningVector[GetIndexFromColRow(0, row, reelWindowSize)] = true;
				for (int col = 1; col < OAK; col++)
				{
					for (int row = 0; row < reelWindowSize[col]; row++)
					{
						int index = GetIndexFromColRow(col, row, reelWindowSize);

						if (resultVector[index] == firstSymbol || resultVector[index] == wildIndex)
							winningVector[index] = true;
					}
				}
			}
		}
	}

	return totalPayout;
}

int CommonLibrary::GetAnywherePayout(const std::vector<std::vector<int>>& paytable, const std::vector<int>& reelWindowSize, const std::vector<int>& resultVector)
{
	// Initialize
	int totalPayout = 0;
	winningVector.clear();
	winningVector.resize(GetVectorSum(reelWindowSize));

	// Loop through each symbol on the reel
	for (size_t symbolType = 0; symbolType < paytable[0].size(); symbolType++)
	{
		// Skip wild or scatter
		if (static_cast<int>(symbolType) != wildIndex && static_cast<int>(symbolType) != scatterIndex)
		{
			int symbolOAK = GetSymbolCountInResult(static_cast<int>(symbolType), resultVector, true);

			// Compute the win based on the anywhere pay
			int normalPayout = 0;

			// Check if it's a valid symbol with payout
			if (symbolOAK > 0)
			{
				normalPayout = paytable[symbolOAK - 1][symbolType];
				totalPayout += normalPayout;

				// Update the winning vector to be displayed on front end
				if (normalPayout != 0)
				{
					for (size_t i = 0; i < resultVector.size(); i++)
					{
						if (resultVector[i] == static_cast<int>(wildIndex) || resultVector[i] == static_cast<int>(symbolType))
							winningVector[i] = true;
					}
				}
			}
		}
	}

	return totalPayout;
}

CommonLibrary::PayAnywherePayoutResult CommonLibrary::GetPayAnywherePayout(const std::vector<std::vector<int>>& paytable, const std::vector<int>& reelWindowSize, const std::vector<int>& resultVector)
{
	// Initialize
	double totalPayout = 0.0;
	winningVector.clear();
	winningVector.resize(GetVectorSum(reelWindowSize));

	std::vector<PayoutWinline> winlines;

	// Loop through each symbol on the reel
	for (size_t symbolType = 0; symbolType < paytable[0].size(); symbolType++)
	{
		// Skip wild or scatter
		if (static_cast<int>(symbolType) != wildIndex && static_cast<int>(symbolType) != scatterIndex)
		{
			int symbolOAK = GetSymbolCountInResult(static_cast<int>(symbolType), resultVector, true);

			// Check if it's a valid symbol with payout
			if (symbolOAK > 0)
			{
				int lineId = symbolOAK - 1;
				double normalPayout = static_cast<double>(paytable[lineId][symbolType]);

				// Update the winning vector to be displayed on front end
				if (normalPayout != 0)
				{
					totalPayout += normalPayout;

					PayoutWinline winline = PayoutWinline{};
					winline.lineId = lineId;
					winline.symbolId = static_cast<int>(symbolType);
					winline.payout = normalPayout;

					for (size_t i = 0; i < resultVector.size(); i++)
					{
						if (resultVector[i] == static_cast<int>(wildIndex) || resultVector[i] == static_cast<int>(symbolType)) {
							winningVector[i] = true;
							winline.winPosition.push_back(i);
						}
					}

					winlines.push_back(winline);
				}
			}
		}
	}

	return {totalPayout, winlines};
}

CommonLibrary::ClusterPayoutResult CommonLibrary::GetClusterPayout(const std::vector<std::vector<int>> &paytable, const std::vector<int> &reelWindowSize, const std::vector<int> &resultVector)
{
	// Initialize
	int totalPayout = 0;

	currentCluster = 0;
	winningVector.clear();
	winningVector.resize(GetVectorSum(reelWindowSize));

	clusterCheck.clear();
	clusterCheck.resize(GetVectorSum(reelWindowSize), -1);

	cluster.clear();
	clusterSymbol.clear();
	std::vector<Winline> winline;

	FindCluster(reelWindowSize, resultVector, cluster);

	// cout << "[MG STARTED] spinResult.freeSpins: " << spinResult.freeSpins << endl;

	// for (size_t i = 0; i < cluster.size(); i++)
	// {
	// 	for (size_t j = 0; j < cluster[i].size(); j++)
	// 	{
	// 		cout << cluster[i][j];
	// 		if (j < cluster[i].size() - 1)
	// 			cout << ",";
	// 	}
	// 	cout << "------" << endl;
	// }

	// for (size_t i = 0; i < clusterSymbol.size(); i++)
	// {
	// 	/* code */
	// 	cout << clusterSymbol[i] << "..";
	// }
	// cout << "----------------------" << endl;

	// Compute the payout
	for (size_t clusterCount = 0; clusterCount < cluster.size(); clusterCount++)
	{
		const int symbol = clusterSymbol[clusterCount];
		const int clusterSize = cluster[clusterCount].size();
		const int payout = paytable[clusterSize - 1][symbol];

		// Check the payout of this cluster
		if (paytable[cluster[clusterCount].size() - 1][clusterSymbol[clusterCount]] > 0)
		{

			// If there's payout then update the payout
			if (payout > 0)
			{
				totalPayout += paytable[cluster[clusterCount].size() - 1][clusterSymbol[clusterCount]];

				Winline win;
				win.symbolId = symbol;
				win.winAmount = payout;
				win.winPosition = cluster[clusterCount]; // copy vector

				// for (size_t i = 0; i < win.winPosition.size(); i++)
				// {
				// 		cout << win.winAmount << ".." << endl;
				// 		cout << win.winPosition[i] << ".w." << endl;
				// 		cout << win.symbolId << ".." << endl;
				// }


				winline.push_back(win);
				// Update the winning vector
				for (size_t indexes = 0; indexes < cluster[clusterCount].size(); indexes++)
				{
					winningVector[cluster[clusterCount][indexes]] = true;
				}
			}
		}
	}

	return {totalPayout, winline};
}

int CommonLibrary::GetClusterPayoutMultiplierAdditive(const std::vector<std::vector<int>>& paytable, const std::vector<int>& reelWindowSize, const std::vector<int>& resultVector, const std::vector<int>& multiplierVector)
{
	// Initialize
	int totalPayout = 0;

	currentCluster = 0;
	winningVector.clear();
	winningVector.resize(GetVectorSum(reelWindowSize));

	clusterCheck.clear();
	clusterCheck.resize(GetVectorSum(reelWindowSize), -1);

	cluster.clear();
	clusterSymbol.clear();

	FindCluster(reelWindowSize, resultVector, cluster);

	// Compute the payout
	for (size_t clusterCount = 0; clusterCount < cluster.size(); clusterCount++)
	{
		// Check the payout of this cluster
		if (paytable[cluster[clusterCount].size() - 1][clusterSymbol[clusterCount]] > 0)
		{
			// Compute the multiplier
			int multiplier = 1;

			// Update the winning vector
			for (size_t indexes = 0; indexes < cluster[clusterCount].size(); indexes++)
			{
				if (multiplierVector[cluster[clusterCount][indexes]] != 0)
				{
					if (multiplier == 1)
						multiplier = multiplierVector[cluster[clusterCount][indexes]];
					else
						multiplier += multiplierVector[cluster[clusterCount][indexes]];
				}
			}

			// If there's payout then update the payout
			totalPayout += (paytable[cluster[clusterCount].size() - 1][clusterSymbol[clusterCount]] * multiplier);

			// Update the winning vector
			for (size_t indexes = 0; indexes < cluster[clusterCount].size(); indexes++)
				winningVector[cluster[clusterCount][indexes]] = true;
		}
	}

	return totalPayout;
}

#pragma endregion

#pragma region Common Functions
int CommonLibrary::RandomInt(unsigned min, unsigned max)
{
	if (rgRNGInstance->IsEnable()) {
		return rgRNGInstance->RandomInt(min, max);
	}

	if(max <= min) {
		return min;
	}

	std::uniform_int_distribution<int>  random(min, max - 1);
	return random(generator);
}

// Function to random a double number between 0 and 1
double CommonLibrary::RandomDouble()
{
	if (rgRNGInstance->IsEnable()) {
		return rgRNGInstance->RandomDouble();
	}

	std::uniform_real_distribution<> random(0, 1.0);
	return random(generator);
};

int CommonLibrary::GetWeightedIndex(const std::vector<int>& weightedVec)
{
	int totalWeight = 0;

	// Sum up the total weight of the vector
	for (size_t i = 0; i < weightedVec.size(); i++)
		totalWeight += weightedVec[i];

	int randomWeightage = RandomInt(0, totalWeight);
	int loopWeight = 0;

	for (size_t i = 0; i < weightedVec.size(); i++)
	{
		loopWeight += weightedVec[i];
		if (loopWeight > randomWeightage)
			return i;
	}

	return nullIndex;
}

int CommonLibrary::GetVectorSum(const std::vector<int>& vec)
{
	int sum = 0;
	for (size_t i = 0; i < vec.size(); i++)
		sum += vec[i];

	return sum;
}

double CommonLibrary::GetVectorSum(const std::vector<double>& vec)
{
	double sum = 0.0;
	for (size_t i = 0; i < vec.size(); i++)
		sum += vec[i];

	return sum;
}

bool CommonLibrary::CheckColumnWin(const int& col, const std::vector<int>& reelWindowSize, const std::vector<bool>& winningVector)
{
	int startIndex = 0;

	for (int i = 0; i < col; i++)
		startIndex += reelWindowSize[i];

	for (int row = 0; row < reelWindowSize[col]; row++)
	{
		if (winningVector[startIndex])
			return true;

		startIndex++;
	}

	return false;
}

int CommonLibrary::GetSymbolCountInResult(const int& index, const std::vector<int>& resultVector, const bool countWild)
{
	int count = 0;
	for (size_t i = 0; i < resultVector.size(); i++)
	{
		if (resultVector[i] == index || (countWild && resultVector[i] == wildIndex))
			count++;
	}

	return count;
}

int CommonLibrary::GetSymbolCountInCol(const int& col, const int& index, const std::vector<int>& reelWindowSize, const std::vector<int>& resultVector, const bool countWild)
{
	int count = 0;
	int startIndex = 0;

	for (int i = 0; i < col; i++)
		startIndex += reelWindowSize[i];

	for (int row = 0; row < reelWindowSize[col]; row++)
	{
		if (resultVector[startIndex] == index || (countWild && resultVector[startIndex] == wildIndex))
			count++;

		startIndex++;
	}

	return count;
}

int CommonLibrary::GetColFromIndex(const int& index, const std::vector<int> & reelWindowSize)
{
	int sum = 0;
	for (size_t col = 0; col < reelWindowSize.size(); col++)
	{
		sum += reelWindowSize[col];

		if (index < sum)
			return col;
	}

	return nullIndex;
}

int CommonLibrary::GetRowFromIndex(const int& index, const std::vector<int>& reelWindowSize)
{
	int sum = 0;
	for (size_t col = 0; col < reelWindowSize.size(); col++)
	{
		if (index < (sum + reelWindowSize[col]))
			return index - sum;
		else
			sum += reelWindowSize[col];
	}

	return nullIndex;
}

int CommonLibrary::GetIndexFromColRow(const int& col, const int& row, const std::vector<int>& reelWindowSize)
{
	int index = 0;

	for (int i = 0; i < col; i++)
		index += reelWindowSize[i];

	return index + row;
}

void CommonLibrary::PrintVector(const std::vector<int>& reelWindowSize, const std::vector<int>& vec)
{
	int maxRow = 0;

	for (size_t i = 0; i < reelWindowSize.size(); i++)
		maxRow = reelWindowSize[i] > maxRow ? reelWindowSize[i] : maxRow;

	for (int row = 0; row < maxRow; row++)
	{
		for (size_t col = 0; col < reelWindowSize.size(); col++)
		{
			if (row < reelWindowSize[col])
				std::cout << std::setw(4) << vec[GetIndexFromColRow(col, row, reelWindowSize)] << " | ";
			else
				std::cout << std::setw(4) << "- | ";
		}
		std::cout << std::endl;
	}
}

void CommonLibrary::PrintVector(const std::vector<int>& reelWindowSize, const std::vector<bool>& vec)
{
	int maxRow = 0;

	for (size_t i = 0; i < reelWindowSize.size(); i++)
		maxRow = reelWindowSize[i] > maxRow ? reelWindowSize[i] : maxRow;

	for (int row = 0; row < maxRow; row++)
	{
		for (size_t col = 0; col < reelWindowSize.size(); col++)
		{
			if (row < reelWindowSize[col])
				std::cout << std::setw(4) << vec[GetIndexFromColRow(col, row, reelWindowSize)] << " | ";
			else
				std::cout << std::setw(4) << "- | ";
		}
		std::cout << std::endl;
	}
}

#pragma endregion

#pragma region Private Functions
void CommonLibrary::FindCluster(const std::vector<int>& reelWindowSize, const std::vector<int>& resultVector, std::vector<std::vector<int>>& cluster)
{
	for (size_t i = 0; i < resultVector.size(); i++)
	{
		if (resultVector[i] != wildIndex && resultVector[i] != scatterIndex && resultVector[i] != blockerIndex && clusterCheck[i] == -1)
		{
			currentCluster++;
			int col = GetColFromIndex(i, reelWindowSize);
			int row = GetRowFromIndex(i, reelWindowSize);
			int symbol = resultVector[i];
			clusterSymbol.push_back(symbol);
			cluster.push_back({});
			CheckCluster(col, row, i, symbol, reelWindowSize, resultVector, cluster);
		}
	}
}

void CommonLibrary::CheckCluster(const int &col, const int &row, const int &index, const int &symbol, const std::vector<int> &reelWindowSize, const std::vector<int> &resultVector, std::vector<std::vector<int>> &cluster)
{
	// If it's a match then update the cluster
	if ((resultVector[index] == wildIndex || resultVector[index] == symbol) && clusterCheck[index] != currentCluster)
	{
		cluster[currentCluster - 1].push_back(index);
		clusterCheck[index] = currentCluster;

		// Check Left
		if (col > 0)
			CheckCluster(col - 1, row, index - reelWindowSize[col], symbol, reelWindowSize, resultVector, cluster);

		// Check Right
		if (col < static_cast<int>(reelWindowSize.size()) - 1)
			CheckCluster(col + 1, row, index + reelWindowSize[col], symbol, reelWindowSize, resultVector, cluster);

		// Check Up
		if (row > 0)
			CheckCluster(col, row - 1, index - 1, symbol, reelWindowSize, resultVector, cluster);

		// Check Down
		if (row < reelWindowSize[col] - 1)
			CheckCluster(col, row + 1, index + 1, symbol, reelWindowSize, resultVector, cluster);
	}

}

#pragma endregion


