#ifndef RG_LIBRARY_H
#define RG_LIBRARY_H
#include <iostream>
#include <ostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <random>
#include <string>
#include <numeric>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <unordered_map>
#include "json.hpp"
#include "RgRNG.cpp"

class CommonLibrary
{
public:
	struct Winline
	{
		std::vector<int> winPosition;
		int symbolId;
		int lineId;
		double winAmount = 0.0;
		int multiplier = 0;
		double winAmountWithoutMultiplier = 0.0;
		double payout = 0.0;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
			Winline,
			winPosition,
			symbolId,
			lineId,
			winAmount,
			multiplier,
			winAmountWithoutMultiplier,
			payout)
	};

	struct ResultWinline
	{
		std::vector<int> winPosition;
		int symbolId;
		int lineId;
		double winAmount = 0.0;
		int multiplier = 0;
		double winAmountWithoutMultiplier = 0.0;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
			ResultWinline,
			winPosition,
			symbolId,
			lineId,
			winAmount,
			multiplier,
			winAmountWithoutMultiplier)
	};

	struct PayoutWinline
	{
		std::vector<int> winPosition;
		int symbolId;
		int lineId;
		double payout = 0.0;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
			PayoutWinline,
			winPosition,
			symbolId,
			lineId,
			payout)
	};

	struct ClusterPayoutResult
	{
		int totalPayout;
		std::vector<Winline> winlines;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
			ClusterPayoutResult,
			totalPayout,
			winlines)
	};

	struct PayAnywherePayoutResult
	{
		double totalPayout;
		std::vector<PayoutWinline> winlines;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
			PayAnywherePayoutResult,
			totalPayout,
			winlines)
	};
#pragma region Reels Functions

	const int
		wildIndex = 0,
		defaultIndex = 0,
		scatterIndex = 1,
		nullIndex = -1,
		blankIndex = -1,
		trainBlankIndex = -2,
		mysteryIndex = 90,
		randomIndex = 91,
		blockerIndex = 99;

	RgRNG::RNG *rgRNGInstance = RgRNG::GetRNGInstance("");

	// *************************************************************************************************************************************
	// GenerateResult : Function to generate the symbol on the reel by passing in the reel strip set used
	//
	// Parameter :
	//	reelStrip		- A 2 dimensional vector containing the reel strip, used to generate on the reel
	//	reelWindowSize	- A 1 dimensional vector containing the height of each column window size of the game
	//	resultVector	- A 1 dimensional vector containing the result of the reel
	//
	// Output :
	//	resultVector	- Returned with the generated reel strip onto this vector
	// *************************************************************************************************************************************
	void GenerateResult(const std::vector<std::vector<int>> &reelStrip, const std::vector<int> &reelWindowSize, std::vector<int> &resultVector);

	// *************************************************************************************************************************************
	// GenerateResultWithStickyResult : Function to generate the symbol on the reel by passing in the reel strip set used with persisting symbol
	//
	// Parameter :
	//	reelStrip		- A 2 dimensional vector containing the reel strip, used to generate on the reel
	//	reelWindowSize	- A 1 dimensional vector containing the height of each column window size of the game
	//	resultVector	- A 1 dimensional vector containing the result of the reel
	//	stickyVector	- A 1 dimensional vector containing the sticky symbol on the reel
	//
	// Output :
	//	resultVector	- Returned with the generated reel strip onto this vector
	// *************************************************************************************************************************************
	void GenerateResultWithStickyResult(const std::vector<std::vector<int>> &reelStrip, const std::vector<int> &reelWindowSize, std::vector<int> &resultVector, const std::vector<bool> &stickyVector);

	// *************************************************************************************************************************************
	// DestroyWinningSymbol : Function to destroy all the symbol involved in a win, except the sticky symbol
	//
	// Parameter :
	//	destroyVector	- A 1 dimensional vector containing the winning symbol on the reel
	//	resultVector	- A 1 dimensional vector containing the result of the reel
	//	stickyVector	- A 1 dimensional vector containing the sticky symbol on the reel, which will not be destroyed, defaulted to false
	//
	// Output :
	//	resultVector	- Returned with the destroyed winning symbol onto this vector
	// *************************************************************************************************************************************
	void DestroyWinningSymbol(const std::vector<bool> &destroyVector, std::vector<int> &resultVector, const std::vector<bool> &stickyVector);

	// *************************************************************************************************************************************
	// RefillResult : Function to refill all the blank position on the reel
	//
	// Parameter :
	//	reelStrip		- A 2 dimensional vector containing the reel strip, used to generate on the reel
	//	reelWindowSize	- A 1 dimensional vector containing the height of each column window size of the game
	//	resultVector	- A 1 dimensional vector containing the result of the reel
	//
	// Output :
	//	resultVector	- Returned with the refilled reel strip in the blank position onto this vector
	// *************************************************************************************************************************************
	void RefillResult(const std::vector<std::vector<int>> &reelStrip, const std::vector<int> &reelWindowSize, std::vector<int> &resultVector);

	// *************************************************************************************************************************************
	// CollapseResult : Function to collapse all the remaining symbol down the reel, except the sticky symbol
	//
	// Parameter :
	//	reelWindowSize	- A 1 dimensional vector containing the height of each column window size of the game
	//	resultVector	- A 1 dimensional vector containing the result of the reel
	//	stickyVector	- A 1 dimensional vector containing the sticky symbol on the reel, which will not be destroyed, defaulted to false
	//
	// Output :
	//	resultVector	- Returned with the collapsed remaining symbol on the reel
	// *************************************************************************************************************************************
	void CollapseResult(const std::vector<int> &reelWindowSize, std::vector<int> &resultVector, const std::vector<bool> &stickyVector);

	// *************************************************************************************************************************************
	// CollapseVectorBasedOnResult : Function to collapse the vector based on the remaining symbol on the reel, except the sticky symbol
	//
	// Parameter :
	//  endValue		- An integer of the value to be changed if it's a winning symbol
	//	reelWindowSize	- A 1 dimensional vector containing the height of each column window size of the game
	//	collapseVector	- A 1 dimensional vector containing the vector that meant to be collapse down based on the remaining symbol on reel
	//	resultVector	- A 1 dimensional vector containing the result of the reel
	//	stickyVector	- A 1 dimensional vector containing the sticky symbol on the reel, which will not be destroyed, defaulted to false
	//
	// Output :
	//	resultVector	- Returned with the collapsed remaining symbol on the reel
	// *************************************************************************************************************************************
	void CollapseVectorBasedOnResult(const int &endValue, const std::vector<int> &reelWindowSize, std::vector<int> &collapseVector, const std::vector<int> &resultVector, const std::vector<bool> &stickyVector);

	// *************************************************************************************************************************************
	// CollapseVectorBasedOnResult : Function to collapse the vector based on the remaining symbol on the reel, except the sticky symbol
	//
	// Parameter :
	//	reelWindowSize	- A 1 dimensional vector containing the height of each column window size of the game
	//	collapseVector	- A 1 dimensional vector containing the vector that meant to be collapse down based on the remaining symbol on reel
	//	resultVector	- A 1 dimensional vector containing the result of the reel
	//	stickyVector	- A 1 dimensional vector containing the sticky symbol on the reel, which will not be destroyed, defaulted to false
	//
	// Output :
	//	resultVector	- Returned with the collapsed remaining symbol on the reel
	// *************************************************************************************************************************************
	// void CollapseVectorBasedOnResult(const std::vector<int>& reelWindowSize, std::vector<bool>& collapseVector, const std::vector<int>& resultVector, const std::vector<bool>& stickyVector);

	// *************************************************************************************************************************************
	// ChangeVectorBasedOnWinningResult : Function to update the value of a vector if there's winning
	//
	// Parameter :
	//	endValue		- An integer of the value to be changed if it's a winning symbol
	//	changeVector	- A 1 dimensional vector containing the vector to be change
	//	stickyVector	- A 1 dimensional vector containing the sticky symbol on the reel, which will not be destroyed, defaulted to false
	//	winningVector	- A 1 dimensional vector containing the winning symbol on the reel
	//
	// Output :
	//	resultVector	- Returned with the collapsed remaining symbol on the reel
	// *************************************************************************************************************************************
	void ChangeVectorBasedOnWinningResult(const int &endValue, std::vector<int> &changeVector, const std::vector<bool> &stickyVector, const std::vector<bool> &winningVector);

	// *************************************************************************************************************************************
	// RevealMystery : Function to reveal all the mystery symbol on the reel
	//
	// Parameter :
	//	symbolWeightage	- A 1 dimensional vector containing the weightage of all the symbol
	//	resultVector	- A 1 dimensional vector containing the result of the reel
	//
	// Output :
	//	resultVector	- Returned with the revealed mystery symbol on the reel
	// *************************************************************************************************************************************
	void RevealMystery(const std::vector<int> &symbolWeightage, std::vector<int> &resultVector);

	// *************************************************************************************************************************************
	// RevealRandom : Function to reveal all the random symbol on the reel
	//
	// Parameter :
	//	symbolWeightage	- A 1 dimensional vector containing the weightage of all the symbol
	//	resultVector	- A 1 dimensional vector containing the result of the reel
	//
	// Output :
	//	resultVector	- Returned with the revealed random symbol on the reel
	// *************************************************************************************************************************************
	void RevealRandom(const std::vector<int> &symbolWeightage, std::vector<int> &resultVector);

	// *************************************************************************************************************************************
	// GetWinningVector : Function to return the winning vector of the game
	//
	// Parameter :
	//
	// Output :
	//	winningVector	- Returned with the winning vector on the reel
	// *************************************************************************************************************************************
	const std::vector<bool> &GetWinningVector();

	// *************************************************************************************************************************************
	// GetPayLineData : Function to return the payline data for the front end
	//
	// Parameter :
	//
	// Output :
	//	payLineData	- Returned with the data for the payline in that spin
	// *************************************************************************************************************************************
	const std::vector<Winline> &GetPayLineData();

#pragma endregion

#pragma region Payout Functions
	// *************************************************************************************************************************************
	// GetPayLinePayout : Function to compute the payout of line game based on the reel
	//
	// Parameter :
	//	selectedPayLine	- An integer containing the number of selected payline of the game
	//	payline			- A 2 dimensional vector containing the payline position of the game
	//	paytable		- A 2 dimensional vector containing the paytable of the game
	//	reelWindowSize	- A 1 dimensional vector containing the height of each column window size of the game
	//	resultVector	- A 1 dimensional vector containing the result of the reel
	//
	// Output :
	//					- Returned the payout of the game in integer format
	// *************************************************************************************************************************************
	int GetPayLinePayout(const int &selectedPayLine, const std::vector<std::vector<int>> &payline, const std::vector<std::vector<int>> &paytable, const std::vector<int> &reelWindowSize, const std::vector<int> &resultVector);

	// *************************************************************************************************************************************
	// GetPayLinePayoutMultiplierAdditive : Function to compute the payout of line game based on the reel with additive multiplier
	//
	// Parameter :
	//	selectedPayLine	 - An integer containing the number of selected payline of the game
	//	payline			 - A 2 dimensional vector containing the payline position of the game
	//	paytable		 - A 2 dimensional vector containing the paytable of the game
	//	reelWindowSize	 - A 1 dimensional vector containing the height of each column window size of the game
	//	resultVector	 - A 1 dimensional vector containing the result of the reel
	//	multiplierVector - A 1 dimensional vector containing the multiplier of the reel
	//
	// Output :
	//					- Returned the payout of the game in integer format
	// *************************************************************************************************************************************
	int GetPayLinePayoutMultiplierAdditive(const int &selectedPayLine, const std::vector<std::vector<int>> &payline, const std::vector<std::vector<int>> &paytable, const std::vector<int> &reelWindowSize, const std::vector<int> &resultVector, const std::vector<int> &multiplierVector);

	// *************************************************************************************************************************************
	// GetPayLinePayoutMultiplierMultiplicative : Function to compute the payout of line game based on the reel with multiplicative multiplier
	//
	// Parameter :
	//	selectedPayLine	 - An integer containing the number of selected payline of the game
	//	payline			 - A 2 dimensional vector containing the payline position of the game
	//	paytable		 - A 2 dimensional vector containing the paytable of the game
	//	reelWindowSize	 - A 1 dimensional vector containing the height of each column window size of the game
	//	resultVector	 - A 1 dimensional vector containing the result of the reel
	//	multiplierVector - A 1 dimensional vector containing the multiplier of the reel
	//
	// Output :
	//					- Returned the payout of the game in integer format
	// *************************************************************************************************************************************
	int GetPayLinePayoutMultiplierMultiplicative(const int &selectedPayLine, const std::vector<std::vector<int>> &payline, const std::vector<std::vector<int>> &paytable, const std::vector<int> &reelWindowSize, const std::vector<int> &resultVector, const std::vector<int> &multiplierVector);

	// *************************************************************************************************************************************
	// GetPayWayPayout : Function to compute the payout of way game based on the reel
	//
	// Parameter :
	//	paytable		- A 2 dimensional vector containing the paytable of the game
	//	reelWindowSize	- A 1 dimensional vector containing the height of each column window size of the game
	//	resultVector	- A 1 dimensional vector containing the result of the reel
	//
	// Output :
	//					- Returned the payout of the game in integer format
	// *************************************************************************************************************************************
	int GetPayWayPayout(const std::vector<std::vector<int>> &paytable, const std::vector<int> &reelWindowSize, const std::vector<int> &resultVector);

	// *************************************************************************************************************************************
	// GetPayWayPayoutMultiplierMultiplicative : Function to compute the payout of way game based on the reel
	//
	// Parameter :
	//	paytable		- A 2 dimensional vector containing the paytable of the game
	//	reelWindowSize	- A 1 dimensional vector containing the height of each column window size of the game
	//	resultVector	- A 1 dimensional vector containing the result of the reel
	//  multiplierVector - A 1 dimensional vector containing the multiplier of the reel
	//
	// Output :
	//					- Returned the payout of the game in integer format
	// *************************************************************************************************************************************
	int GetPayWayPayoutMultiplierMultiplicative(const std::vector<std::vector<int>> &paytable, const std::vector<int> &reelWindowSize, const std::vector<int> &resultVector, const std::vector<int> &multiplierVector);

	// *************************************************************************************************************************************
	// GetAnywherePayout : Function to compute the payout of anywhere based on the reel
	//
	// Parameter :
	//	paytable		- A 2 dimensional vector containing the paytable of the game
	//	reelWindowSize	- A 1 dimensional vector containing the height of each column window size of the game
	//	resultVector	- A 1 dimensional vector containing the result of the reel
	//
	// Output :
	//					- Returned the payout of the game in integer format
	// *************************************************************************************************************************************
	int GetAnywherePayout(const std::vector<std::vector<int>> &paytable, const std::vector<int> &reelWindowSize, const std::vector<int> &resultVector);

	PayAnywherePayoutResult GetPayAnywherePayout(const std::vector<std::vector<int>> &paytable, const std::vector<int> &reelWindowSize, const std::vector<int> &resultVector);

	// *************************************************************************************************************************************
	// GetClusterPayout : Function to compute the payout of cluster based on the reel
	//
	// Parameter :
	//	paytable		- A 2 dimensional vector containing the paytable of the game
	//	reelWindowSize	- A 1 dimensional vector containing the height of each column window size of the game
	//	resultVector	- A 1 dimensional vector containing the result of the reel
	//
	// Output :
	//					- Returned the payout of the game in integer format
	// *************************************************************************************************************************************
	ClusterPayoutResult GetClusterPayout(const std::vector<std::vector<int>> &paytable, const std::vector<int> &reelWindowSize, const std::vector<int> &resultVector);

	// *************************************************************************************************************************************
	// GetClusterPayout : Function to compute the payout of cluster based on the reel with additive multiplier
	//
	// Parameter :
	//	paytable		 - A 2 dimensional vector containing the paytable of the game
	//	reelWindowSize	 - A 1 dimensional vector containing the height of each column window size of the game
	//	resultVector	 - A 1 dimensional vector containing the result of the reel
	//	resultVector	 - A 1 dimensional vector containing the result of the reel
	//  multiplierVector - A 1 dimensional vector containing the multiplier of the reel
	//
	// Output :
	//					- Returned the payout of the game in integer format
	// *************************************************************************************************************************************
	int GetClusterPayoutMultiplierAdditive(const std::vector<std::vector<int>> &paytable, const std::vector<int> &reelWindowSize, const std::vector<int> &resultVector, const std::vector<int> &multiplierVector);

#pragma endregion

#pragma region Common Functions
	// *************************************************************************************************************************************
	// RandomInt : Function to return an integer number based on the min and max(with max excluded)
	//
	// Parameter :
	//	min			- An integer containing the min of the randomized range
	//	max			- An integer containing the max of the randomized range with max excluded
	//
	// Output :
	//				- Returned a randomized integer between the range with max excluded
	// *************************************************************************************************************************************
	int RandomInt(unsigned min, unsigned max);

	// *************************************************************************************************************************************
	// Random : Function to return a randomized double number
	//
	// Output :
	//				- Returned a randomized double value
	// *************************************************************************************************************************************
	double RandomDouble();

	// *************************************************************************************************************************************
	// GetWeightedIndex : Function to return an integer number based on the weightage of the vector
	//
	// Parameter :
	//	weightVec	- A 1 dimensional vector containing the weight of each index
	//
	// Output :
	//				- Returned the index number generated based on the weightage
	// *************************************************************************************************************************************
	int GetWeightedIndex(const std::vector<int> &weightVec);

	// *************************************************************************************************************************************
	// GetVectorSum : Function to return the sum of the integer vector
	//
	// Parameter :
	//	vec			- A 1 dimensional integer vector
	//
	// Output :
	//				- Returned the sum of the vector
	// *************************************************************************************************************************************
	int GetVectorSum(const std::vector<int> &vec);

	// *************************************************************************************************************************************
	// GetVectorSum : Function to return the sum of the double vector
	//
	// Parameter :
	//	vec			- A 1 dimensional double vector
	//
	// Output :
	//				- Returned the sum of the vector
	// *************************************************************************************************************************************
	double GetVectorSum(const std::vector<double> &vec);

	// *************************************************************************************************************************************
	// CheckColumnWin : Function to check if there's winning on this column
	//
	// Parameter :
	//	col				- An integer of the target column
	//  reelWindowSize	- A 1 dimensional vector containing the height of each column window size of the game
	//	winningVector	- A 1 dimensional vector containing the winning symbol on the reel
	//
	// Output :
	//					- Returned true if there's winning in this column
	// *************************************************************************************************************************************
	bool CheckColumnWin(const int &col, const std::vector<int> &reelWindowSize, const std::vector<bool> &winningVector);

	// *************************************************************************************************************************************
	// GetSymbolCountInResult : Function to return the count of a specific symbol in the reel
	//
	// Parameter :
	//	index			- An integer of the index of the symbol to count
	//  resultVector	- A 1 dimensional vector containing the result of the reel
	//  countWild		- A boolean to include if wild is taken into consideration, defaulted to false
	//
	// Output :
	//					- Returned the count of the specific symbol in the reel
	// *************************************************************************************************************************************
	int GetSymbolCountInResult(const int &index, const std::vector<int> &resultVector, const bool countWild = false);

	// *************************************************************************************************************************************
	// GetSymbolCountInCol : Function to return the count of a specific symbol in a certain column of the reel
	//
	// Parameter :
	//	col				- An integer of the target column
	//	index			- An integer of the index of the symbol to count
	//  reelWindowSize	- A 1 dimensional vector containing the height of each column window size of the game
	//  resultVector	- A 1 dimensional vector containing the result of the reel
	//  countWild		- A boolean to include if wild is taken into consideration, defaulted to false
	//
	// Output :
	//					- Returned the count of the specific symbol in a certain column of the reel
	// *************************************************************************************************************************************
	int GetSymbolCountInCol(const int &col, const int &index, const std::vector<int> &reelWindowSize, const std::vector<int> &resultVector, const bool countWild = false);

	// *************************************************************************************************************************************
	// GetColFromIndex : Function to return the column index based on the index of the reel
	//
	// Parameter :
	//	index			- An integer of the reel position index
	//  reelWindowSize	- A 1 dimensional vector containing the height of each column window size of the game
	//
	// Output :
	//					- Returned the column index of the reel
	// *************************************************************************************************************************************
	int GetColFromIndex(const int &index, const std::vector<int> &reelWindowSize);

	// *************************************************************************************************************************************
	// GetRowFromIndex : Function to return the row index based on the index of the reel
	//
	// Parameter :
	//	index			- An integer of the reel position index
	//  reelWindowSize	- A 1 dimensional vector containing the height of each column window size of the game
	//
	// Output :
	//					- Returned the row index of the reel
	// *************************************************************************************************************************************
	int GetRowFromIndex(const int &index, const std::vector<int> &reelWindowSize);

	// *************************************************************************************************************************************
	// GetIndexFromColRow : Function to return the reel index based on the column and row
	//
	// Parameter :
	//	col				- An integer of the target column
	//	row				- An integer of the target row
	//  reelWindowSize	- A 1 dimensional vector containing the height of each column window size of the game
	//
	// Output :
	//					- Returned the index of the reel
	// *************************************************************************************************************************************
	int GetIndexFromColRow(const int &col, const int &row, const std::vector<int> &reelWindowSize);

	// *************************************************************************************************************************************
	// PrintResult : Function to print the integer vector in the format of the reel
	//
	// Parameter :
	//  reelWindowSize	- A 1 dimensional vector containing the height of each column window size of the game
	//  vec				- A 1 dimensional integer vector
	//
	// Output :
	//					- Print the integer vector in the format of the reel
	// *************************************************************************************************************************************
	void PrintVector(const std::vector<int> &reelWindowSize, const std::vector<int> &vec);

	// *************************************************************************************************************************************
	// PrintResult : Function to print the integer vector in the format of the reel
	//
	// Parameter :
	//  reelWindowSize	- A 1 dimensional vector containing the height of each column window size of the game
	//  vec				- A 1 dimensional boolean vector
	//
	// Output :
	//					- Print the integer vector in the format of the reel
	// *************************************************************************************************************************************
	void PrintVector(const std::vector<int> &reelWindowSize, const std::vector<bool> &vec);

#pragma endregion

protected: // Members of the class
	std::vector<bool> winningVector;
	std::vector<std::vector<int>> cluster;
	std::vector<int> clusterCheck;
	std::vector<int> clusterSymbol;
	std::vector<Winline> payLineData;

	int currentCluster;

private: // Private function for the class
	// *************************************************************************************************************************************
	// Private Function : No description
	// *************************************************************************************************************************************
	void FindCluster(const std::vector<int> &reelWindowSize, const std::vector<int> &resultVector, std::vector<std::vector<int>> &cluster);

	// *************************************************************************************************************************************
	// Private Function : No description
	// *************************************************************************************************************************************
	void CheckCluster(const int &col, const int &row, const int &index, const int &symbol, const std::vector<int> &reelWindowSize, const std::vector<int> &resultVector, std::vector<std::vector<int>> &cluster);
};

extern CommonLibrary library;

#endif // RG_LIBRARY_H