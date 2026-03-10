#include <iostream>
#include <vector>
#include <chrono>
#include "../../libs/CommonLibrary.h"
#include "GameConfig.h"

using namespace std;
using namespace chrono;

namespace GameStruct
{
	// Game Setting
	enum BetMode
	{
		CIRCLE = 0,
		TRIANGLE = 1,
		DIAMOND = 2,
	};
	NLOHMANN_JSON_SERIALIZE_ENUM(
			BetMode,
			{
					{BetMode::CIRCLE, 0},
					{BetMode::TRIANGLE, 1},
					{BetMode::DIAMOND, 2},
			})

	struct SpinResult
	{
		double baseBetAmount;
		double betAmount;
		double payout;
		double checkPayout;
		double maxWinQuota;

		vector<int> resultVector;
		vector<int> multiplierReelVector;
		vector<bool> winningVector;
		vector<CommonLibrary::Winline> winLines;

		int selectionMode;
		int reelMode;
		int multiplier;
		int freeSpins;
		int totalFreeSpinsCount;
		bool triggerMaxWin;
		bool teaser;
		bool featureBuy;
		double totalFreeGameWin;

		int winFreeSpins;
		double betSize;
		int betLevel;
		bool isEndRound;
		bool isMaxWin;
		string _rngCheatSeed;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
				SpinResult,
				baseBetAmount,
				betAmount,
				payout,
				checkPayout,
				maxWinQuota,

				resultVector,
				multiplierReelVector,
				winningVector,
				winLines,

				selectionMode,
				reelMode,
				multiplier,
				freeSpins,
				totalFreeSpinsCount,
				triggerMaxWin,
				teaser,
				featureBuy,
				totalFreeGameWin,

				winFreeSpins,
				betSize,
				betLevel,
				isEndRound,
				isMaxWin,
				_rngCheatSeed)
	};

	struct SpinRequest
	{
		double betSize = 0;
		int betLevel = 0;
		SpinResult pastSpinResult;
		SpinResult parentSpinResult;

		BetMode betMode;
		int profile = 0;
		GameConfig gameConfig;
		string _rngCheatSeed;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
				SpinRequest,
				betSize,
				betLevel,
				pastSpinResult,
				parentSpinResult,
				betMode,
				profile,
				gameConfig,
				_rngCheatSeed)
	};
}

namespace GameMath
{
	using namespace GameStruct;

	class BaseGameLogic
	{
	public:
		BaseGameLogic(int rtpProfile, GameConfig *gameConfig)
		{
			if (gameConfig != nullptr)
			{
				// gameConfig to BaseProfile
				this->profile = new GameConfigBase();
				// this->profile->reelStripCircle = gameConfig->reelStripCircle;
				// this->profile->reelStripTriangle = gameConfig->reelStripTriangle;
				// this->profile->reelStripDiamond = gameConfig->reelStripDiamond;
				this->profile->triangleMultiplierWeightage = gameConfig->triangleMultiplierWeightage;
				this->profile->diamondMultiplierWeightage = gameConfig->diamondMultiplierWeightage;
				this->profile->mgReelWeightage = gameConfig->mgReelWeightage;
				this->profile->multiplierWeightage = gameConfig->multiplierWeightage;
				this->profile->freeGameChance = gameConfig->freeGameChance;
				this->profile->teaserChance = gameConfig->teaserChance;
				this->profile->fgTriangleMultiplierWeightage = gameConfig->fgTriangleMultiplierWeightage;
				this->profile->fgDiamondMultiplierWeightage = gameConfig->fgDiamondMultiplierWeightage;
				this->profile->fgReelWeightage = gameConfig->fgReelWeightage;
				this->profile->fbReelWeightage = gameConfig->fbReelWeightage;
				this->profile->fgMultiplierWeightage = gameConfig->fgMultiplierWeightage;
				this->profile->featureBuyCost = gameConfig->featureBuyCost;
			}
			else
			{
				switch (rtpProfile)
				{
				case 88:
					this->profile = new GameConfig88();
					break;
				case 94:
					this->profile = new GameConfig94();
					break;
				case 97:
					this->profile = new GameConfig97();
					break;
				case 120:
					this->profile = new GameConfig120();
					break;
				case 140:
					this->profile = new GameConfig140();
					break;
				default:
					this->profile = new GameConfigBase();
					break;
				}
			}
		};

		GameConfig GetConfigs() const
		{
			GameConfig configs = {
					.reelStripCircle = this->profile->reelStripCircle,
					.reelStripTriangle = this->profile->reelStripTriangle,
					.reelStripDiamond = this->profile->reelStripDiamond,
					.paytable = this->profile->paytable,
					.payline = this->profile->payline,
					.circleIndex = this->profile->circleIndex,
					.triangleIndex = this->profile->triangleIndex,
					.diamondIndex = this->profile->diamondIndex,
					.reelWindowSize = this->profile->reelWindowSize,
					.triangleMultiplierWeightage = this->profile->triangleMultiplierWeightage,
					.diamondMultiplierWeightage = this->profile->diamondMultiplierWeightage,
					.mgReelWeightage = this->profile->mgReelWeightage,
					.multiplierWeightage = this->profile->multiplierWeightage,
					.baseBet = this->profile->baseBet,
					.selectedPayLine = this->profile->selectedPayLine,
					.maxWinMultiplier = this->profile->maxWinMultiplier,
					.freeGameChance = this->profile->freeGameChance,
					.teaserChance = this->profile->teaserChance,
					.fgTriangleMultiplierWeightage = this->profile->fgTriangleMultiplierWeightage,
					.fgDiamondMultiplierWeightage = this->profile->fgDiamondMultiplierWeightage,
					.fgReelWeightage = this->profile->fgReelWeightage,
					.fbReelWeightage = this->profile->fbReelWeightage,
					.fgMultiplierWeightage = this->profile->fgMultiplierWeightage,
					.featureBuyCost = this->profile->featureBuyCost};

			return configs;
		}

		virtual SpinResult FreeGameSpin(const SpinRequest spinRequest)
		{
			SpinResult spinResult;

			// Initialization of the other vector
			spinResult.payout = 0.0;
			spinResult.checkPayout = 0.0;
			spinResult.resultVector = vector<int>(library.GetVectorSum(this->profile->reelWindowSize));
			spinResult.multiplierReelVector = vector<int>(this->profile->reelWindowSize.size(), 0);
			spinResult.selectionMode = 2;
			spinResult.multiplier = 0;
			spinResult.triggerMaxWin = false;

			// New free game triggered
			if (spinRequest.pastSpinResult.totalFreeSpinsCount == 0)
			{
				spinResult.baseBetAmount = spinRequest.betSize * spinRequest.betLevel * this->profile->baseBet;
				spinResult.betAmount = spinRequest.betSize * spinRequest.betLevel * this->profile->baseBet;
				spinResult.maxWinQuota = spinResult.betAmount * (double)this->profile->maxWinMultiplier; // Reset max win quota
				spinResult.totalFreeSpinsCount = 1;
				spinResult.featureBuy = true;

				// Adjust bet amount if player playing feature buy
				if (spinResult.featureBuy)
					spinResult.betAmount = spinRequest.betSize * spinRequest.betLevel * this->profile->featureBuyCost[spinResult.selectionMode];
			}

			// Continuation from previous free game
			else
			{
				spinResult.baseBetAmount = 0.0;
				spinResult.betAmount = 0.0;
				spinResult.totalFreeSpinsCount = spinRequest.pastSpinResult.totalFreeSpinsCount + 1;
				spinResult.maxWinQuota = spinRequest.pastSpinResult.maxWinQuota;
				spinResult.featureBuy = spinRequest.pastSpinResult.featureBuy;
			}

			// Circle Mode
			if (spinResult.selectionMode == this->profile->circleIndex)
			{
				spinResult.multiplierReelVector[0] = library.blockerIndex;
				spinResult.multiplierReelVector[1] = library.blockerIndex;
			}

			// Triangle Mode
			else if (spinResult.selectionMode == this->profile->triangleIndex)
				spinResult.multiplierReelVector[0] = library.blockerIndex;

			// Get the reel strip mode of the game
			spinResult.reelMode = library.GetWeightedIndex(this->profile->fgReelWeightage[spinResult.selectionMode]);
			if (spinResult.featureBuy)
				spinResult.reelMode = library.GetWeightedIndex(this->profile->fbReelWeightage[spinResult.selectionMode]);

			// Generate reel strip on screen
			library.GenerateResult(GetReelStrip(spinResult.reelMode, spinResult.selectionMode, true), this->profile->reelWindowSize, spinResult.resultVector);

			// Circle Mode
			if (spinResult.selectionMode == this->profile->circleIndex)
			{
				spinResult.resultVector[library.GetIndexFromColRow(1, 1, this->profile->reelWindowSize)] = library.wildIndex;
			}

			// Diamond Mode
			if (spinResult.selectionMode == this->profile->diamondIndex)
			{
				spinResult.resultVector[library.GetIndexFromColRow(1, 0, this->profile->reelWindowSize)] = library.blockerIndex;
				spinResult.resultVector[library.GetIndexFromColRow(1, 2, this->profile->reelWindowSize)] = library.blockerIndex;
			}

			// First reel and third reel will synced
			for (int row = 0; row < this->profile->reelWindowSize.size(); row++)
				spinResult.resultVector[library.GetIndexFromColRow(2, row, this->profile->reelWindowSize)] = spinResult.resultVector[library.GetIndexFromColRow(0, row, this->profile->reelWindowSize)];

			// If there's no payout then alternate the multiplier
			spinResult.checkPayout = library.GetPayLinePayout(this->profile->selectedPayLine, this->profile->payline, this->profile->paytable, this->profile->reelWindowSize, spinResult.resultVector);

			// For triangle and diamond the first 2 spins are always 0 payout, re-generate the result until no win is generated
			if (spinResult.selectionMode == this->profile->triangleIndex || spinResult.selectionMode == this->profile->diamondIndex)
			{
				while (spinResult.totalFreeSpinsCount < 2 && spinResult.checkPayout > 0.0)
				{
					// Get the reel strip mode of the game
					spinResult.reelMode = library.GetWeightedIndex(this->profile->fgReelWeightage[spinResult.selectionMode]);
					if (spinResult.featureBuy)
						spinResult.reelMode = library.GetWeightedIndex(this->profile->fbReelWeightage[spinResult.selectionMode]);

					// Generate reel strip on screen
					library.GenerateResult(GetReelStrip(spinResult.reelMode, spinResult.selectionMode, true), this->profile->reelWindowSize, spinResult.resultVector);

					// Diamond Mode
					if (spinResult.selectionMode == this->profile->diamondIndex)
					{
						spinResult.resultVector[library.GetIndexFromColRow(1, 0, this->profile->reelWindowSize)] = library.blockerIndex;
						spinResult.resultVector[library.GetIndexFromColRow(1, 2, this->profile->reelWindowSize)] = library.blockerIndex;
					}

					// First reel and third reel will synced
					for (int row = 0; row < this->profile->reelWindowSize.size(); row++)
						spinResult.resultVector[library.GetIndexFromColRow(2, row, this->profile->reelWindowSize)] = spinResult.resultVector[library.GetIndexFromColRow(0, row, this->profile->reelWindowSize)];

					// If there's no payout then alternate the multiplier
					spinResult.checkPayout = library.GetPayLinePayout(this->profile->selectedPayLine, this->profile->payline, this->profile->paytable, this->profile->reelWindowSize, spinResult.resultVector);
				}
			}

			// If there's already 5 spin and still no win, re-generate the result until a win is generated
			while (spinResult.totalFreeSpinsCount == 5 && spinResult.checkPayout == 0.0)
			{
				// Get the reel strip mode of the game
				spinResult.reelMode = library.GetWeightedIndex(this->profile->fgReelWeightage[spinResult.selectionMode]);
				if (spinResult.featureBuy)
					spinResult.reelMode = library.GetWeightedIndex(this->profile->fbReelWeightage[spinResult.selectionMode]);

				// Generate reel strip on screen
				library.GenerateResult(GetReelStrip(spinResult.reelMode, spinResult.selectionMode, true), this->profile->reelWindowSize, spinResult.resultVector);

				// Circle Mode
				if (spinResult.selectionMode == this->profile->circleIndex)
					spinResult.resultVector[library.GetIndexFromColRow(1, 1, this->profile->reelWindowSize)] = library.wildIndex;

				// Diamond Mode
				if (spinResult.selectionMode == this->profile->diamondIndex)
				{
					spinResult.resultVector[library.GetIndexFromColRow(1, 0, this->profile->reelWindowSize)] = library.blockerIndex;
					spinResult.resultVector[library.GetIndexFromColRow(1, 2, this->profile->reelWindowSize)] = library.blockerIndex;
				}

				// First reel and third reel will synced
				for (int row = 0; row < this->profile->reelWindowSize.size(); row++)
					spinResult.resultVector[library.GetIndexFromColRow(2, row, this->profile->reelWindowSize)] = spinResult.resultVector[library.GetIndexFromColRow(0, row, this->profile->reelWindowSize)];

				// If there's no payout then alternate the multiplier
				spinResult.checkPayout = library.GetPayLinePayout(this->profile->selectedPayLine, this->profile->payline, this->profile->paytable, this->profile->reelWindowSize, spinResult.resultVector);
			}

			// Geenerate multiplier on multiplier reel
			spinResult.multiplier = GenerateMultiplierReel(spinResult.multiplierReelVector, spinResult.selectionMode, spinResult.reelMode, spinResult.checkPayout, true);

			// Get the payout of the game
			spinResult.payout = min((double)library.GetPayLinePayout(this->profile->selectedPayLine, this->profile->payline, this->profile->paytable, this->profile->reelWindowSize, spinResult.resultVector) * spinResult.multiplier, spinResult.maxWinQuota);

			// If there's no payout then continue the free game
			if (spinResult.payout == 0.0)
				spinResult.freeSpins = 1;
			else
				spinResult.freeSpins = 0;

			// Get the front end data for the payline
			if (spinResult.payout > 0.0)
			{
				spinResult.winLines = library.GetPayLineData();
				spinResult.winningVector = library.GetWinningVector();

				if (spinResult.payout == spinResult.maxWinQuota)
					spinResult.triggerMaxWin = true;
			}

			return spinResult;
		}

		// Function to simulate a spin of the game
		virtual SpinResult MainGameSpin(const SpinRequest spinRequest)
		{
			double betAmount = spinRequest.betSize * spinRequest.betLevel * this->profile->baseBet;
			SpinResult spinResult = {};
			spinResult.baseBetAmount = spinRequest.betSize * spinRequest.betLevel * this->profile->baseBet;
			spinResult.betAmount = spinRequest.betSize * spinRequest.betLevel * this->profile->baseBet;
			spinResult.payout = 0.0;
			spinResult.resultVector = vector<int>(library.GetVectorSum(this->profile->reelWindowSize));
			spinResult.multiplierReelVector = vector<int>(this->profile->reelWindowSize.size(), 0);
			spinResult.selectionMode = spinRequest.betMode;
			spinResult.multiplier = 0;
			spinResult.betSize = spinRequest.betSize;
			spinResult.betLevel = spinRequest.betLevel;

			spinResult.maxWinQuota = betAmount * (double)this->profile->maxWinMultiplier; // Reset max win quota

			// Circle Mode
			if (spinResult.selectionMode == this->profile->circleIndex)
			{
				spinResult.multiplierReelVector[0] = library.blockerIndex;
				spinResult.multiplierReelVector[1] = library.blockerIndex;
			}

			// Triangle Mode
			else if (spinResult.selectionMode == this->profile->triangleIndex)
				spinResult.multiplierReelVector[0] = library.blockerIndex;

			// Main Game
			// Get the reel strip mode of the game
			spinResult.reelMode = library.GetWeightedIndex(this->profile->mgReelWeightage[spinResult.selectionMode]);

			// Generate reel strip on screen
			library.GenerateResult(GetReelStrip(spinResult.reelMode, spinResult.selectionMode), this->profile->reelWindowSize, spinResult.resultVector);

			// Circle Mode
			if (spinResult.selectionMode == this->profile->circleIndex)
			{
				spinResult.resultVector[library.GetIndexFromColRow(1, 1, this->profile->reelWindowSize)] = library.wildIndex;
			}

			// Diamond Mode
			if (spinResult.selectionMode == this->profile->diamondIndex)
			{
				spinResult.resultVector[library.GetIndexFromColRow(1, 0, this->profile->reelWindowSize)] = library.blockerIndex;
				spinResult.resultVector[library.GetIndexFromColRow(1, 2, this->profile->reelWindowSize)] = library.blockerIndex;
			}

			// If there's no payout then alternate the multiplier
			spinResult.checkPayout = library.GetPayLinePayout(this->profile->selectedPayLine, this->profile->payline, this->profile->paytable, this->profile->reelWindowSize, spinResult.resultVector);

			// Geenerate multiplier on multiplier reel
			spinResult.multiplier = GenerateMultiplierReel(spinResult.multiplierReelVector, spinResult.selectionMode, spinResult.reelMode, spinResult.checkPayout);

			// Get the payout of the game
			spinResult.payout = min((double)library.GetPayLinePayout(this->profile->selectedPayLine, this->profile->payline, this->profile->paytable, this->profile->reelWindowSize, spinResult.resultVector) * spinResult.multiplier * spinRequest.betSize * spinRequest.betLevel, spinResult.maxWinQuota);

			// Get the front end data for the payline
			if (spinResult.payout > 0)
			{
				spinResult.winLines = library.GetPayLineData();

				// look for winLines and update multiplier
				for (int i = 0; i < spinResult.winLines.size(); i++)
				{
					spinResult.winLines[i].multiplier = spinResult.multiplier;
					spinResult.winLines[i].winAmountWithoutMultiplier = spinResult.winLines[i].winAmountWithoutMultiplier * spinRequest.betSize * spinRequest.betLevel;
					spinResult.winLines[i].winAmount = spinResult.winLines[i].winAmountWithoutMultiplier * spinResult.multiplier;
					spinResult.winLines[i].payout = spinResult.winLines[i].winAmount / spinResult.betAmount;
				}

				spinResult.winningVector = library.GetWinningVector();
			}

			spinResult.maxWinQuota -= spinResult.payout;

			if (spinResult.maxWinQuota <= 0.0)
			{
				spinResult.isMaxWin = true;
				goto endSpin;
			}

		endSpin:
			spinResult.isEndRound = true;
			return spinResult;
		}

		SpinResult Spin(const SpinRequest spinRequest)
		{
			bool featureTriggered = library.RandomInt(1, this->profile->freeGameChance[spinRequest.betMode] + 1) == 1;

			if (spinRequest.parentSpinResult.freeSpins > 0 || featureTriggered)
				return FreeGameSpin(spinRequest);
			else
				return MainGameSpin(spinRequest);
		}

		~BaseGameLogic()
		{
			delete this->profile;
		}

	protected:
		GameConfig *profile;
		CommonLibrary library;

		// Function to generate the reel strip based on the game mode
		vector<vector<int>> &GetReelStrip(const int &reelMode, const int &shapeMode, bool isFreeGame = false)
		{
			// Return reel strips
			if (shapeMode == this->profile->circleIndex)
				return this->profile->reelStripCircle[reelMode];

			else if (shapeMode == this->profile->triangleIndex)
				return this->profile->reelStripTriangle[reelMode];

			else if (shapeMode == this->profile->diamondIndex)
				return this->profile->reelStripDiamond[reelMode];
		}

		// Function to generate the multiplier on the multiplier reels
		int GenerateMultiplierReel(vector<int> &multiplierReelVector, const int &shapeMode, const int &reelMode, const double &checkPayout, const bool &freeGame = false)
		{
			int multiplierSum = 0;

			bool changeMultiplier = false;

			// Alternate the multiplier if there's no payout
			if (checkPayout == 0.0 && library.RandomInt(1, 10 + 1) <= 2)
				changeMultiplier = true;

			for (int i = 0; i < multiplierReelVector.size(); i++)
			{
				// Check if this position is a blocker
				if (multiplierReelVector[i] != library.blockerIndex)
				{
					if (!freeGame)
						multiplierReelVector[i] = library.GetWeightedIndex(this->profile->multiplierWeightage[shapeMode][reelMode]) - 1;
					else
						multiplierReelVector[i] = library.GetWeightedIndex(this->profile->fgMultiplierWeightage[shapeMode][reelMode]) - 1;
					if (changeMultiplier)
						multiplierReelVector[i] = library.RandomInt(0, 9 + 1);
				}
			}

			if (!changeMultiplier)
			{
				// Determine the total number of multiplier available
				// Triangle Mode
				if (shapeMode == this->profile->triangleIndex)
				{
					int blankReel;
					if (!freeGame)
						blankReel = 2 - library.GetWeightedIndex(this->profile->triangleMultiplierWeightage[reelMode]);
					else
						blankReel = 2 - library.GetWeightedIndex(this->profile->fgTriangleMultiplierWeightage[reelMode]);

					// Determine which reel to be blank
					if (blankReel > 0)
					{
						int randomizeIndex = library.RandomInt(1, 2 + 1);
						if (freeGame)
							randomizeIndex = 1;

						if (!freeGame)
							multiplierReelVector[randomizeIndex] = library.blankIndex;
						else
							multiplierReelVector[randomizeIndex] = 0;
					}
				}

				// Diamond Mode
				else if (shapeMode == this->profile->diamondIndex)
				{
					int blankReel;
					if (!freeGame)
						blankReel = 3 - library.GetWeightedIndex(this->profile->diamondMultiplierWeightage[reelMode]);
					else
						blankReel = 3 - library.GetWeightedIndex(this->profile->fgDiamondMultiplierWeightage[reelMode]);

					// Determine which reel to be blank
					if (blankReel > 0)
					{
						vector<int> blankCol = {0, 1, 2};

						while (blankReel > 0)
						{
							int randomizeIndex = library.RandomInt(0, blankCol.size());
							if (freeGame)
								randomizeIndex = 0;

							if (!freeGame)
								multiplierReelVector[blankCol[randomizeIndex]] = library.blankIndex;
							else
								multiplierReelVector[blankCol[randomizeIndex]] = 0;

							blankReel--;
							blankCol.erase(blankCol.begin() + randomizeIndex);
						}
					}
				}
			}

			int divider = 1;

			// Compute the multiplier to be returned
			for (int i = multiplierReelVector.size() - 1; i >= 0; i--)
			{
				// If it's not a blocker or blank
				if (multiplierReelVector[i] != library.blockerIndex && multiplierReelVector[i] != library.blankIndex)
				{
					multiplierSum = multiplierSum + (multiplierReelVector[i] * divider);
					divider *= 10;
				}
			}

			// If there's no valid multiplier, then at the minimum return 1
			multiplierSum = (multiplierSum == 0) ? 1 : multiplierSum;

			return multiplierSum;
		}
	};

	class GameLogic : public BaseGameLogic
	{
	public:
		using BaseGameLogic::BaseGameLogic;
	};

	class CheatLogic : public BaseGameLogic
	{
	public:
		using BaseGameLogic::BaseGameLogic;

		SpinResult MainGameSpin(const SpinRequest spinRequest) override
		{
			if (!spinRequest._rngCheatSeed.empty())
			{
				library.rgRNGInstance->SetStateByCheatSeed(spinRequest._rngCheatSeed);
			}

			string _rngCheatSeed = library.rgRNGInstance->GetCheatSeedFromState();

			library.rgRNGInstance->Enable();
			SpinResult result = BaseGameLogic::MainGameSpin(spinRequest);
			library.rgRNGInstance->Disable();

			// return turn back _rngCheatSeed
			result._rngCheatSeed = _rngCheatSeed;
			return result;
		}
	};
}
