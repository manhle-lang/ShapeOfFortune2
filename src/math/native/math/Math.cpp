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
		double checkPayout;
		double payout;
		double maxWinQuota;

		vector<int> resultVector;
		vector<int> multiplierReelVector;
		vector<bool> winningVector;
		vector<CommonLibrary::Winline> winLines;

		int selectionMode;
		int reelMode;
		int multiplier;

		bool triggerMaxWin;
		double totalFreeGameWin;

		int winFreeSpins;
		int freeSpins;
		double betSize;
		int betLevel;
		bool isEndRound;
		bool isMaxWin;
		string _rngCheatSeed;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
				SpinResult,
				baseBetAmount,
				betAmount,
				checkPayout,
				payout,
				maxWinQuota,
				resultVector,
				multiplierReelVector,
				winningVector,
				winLines,
				selectionMode,
				reelMode,
				multiplier,
				triggerMaxWin,
				totalFreeGameWin,
				winFreeSpins,
				freeSpins,
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
			};

			return configs;
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
			// New spin
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
		vector<vector<int>> &GetReelStrip(const int &reelMode, const int &shapeMode)
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
		int GenerateMultiplierReel(vector<int> &multiplierReelVector, const int &shapeMode, const int &reelMode, const double& checkPayout)
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
					// Circle Mode
					if (shapeMode == this->profile->circleIndex)
					{
						multiplierReelVector[i] = library.GetWeightedIndex(this->profile->multiplierWeightage[shapeMode][reelMode]) - 1;
						if (changeMultiplier) multiplierReelVector[i] = library.RandomInt(0, 9 + 1);
					}

					// Triangle Mode
					else if (shapeMode == this->profile->triangleIndex)
					{
						multiplierReelVector[i] = library.GetWeightedIndex(this->profile->multiplierWeightage[shapeMode][reelMode]) - 1;
						if (changeMultiplier) multiplierReelVector[i] = library.RandomInt(0, 9 + 1);
					}

					// Diamond Mode
					else if (shapeMode == this->profile->diamondIndex)
					{
						multiplierReelVector[i] = library.GetWeightedIndex(this->profile->multiplierWeightage[shapeMode][reelMode]) - 1;
						if (changeMultiplier) multiplierReelVector[i] = library.RandomInt(0, 9 + 1);
					}
				}
			}

			// Determine the total number of multiplier available
			// Triangle Mode
			if (shapeMode == this->profile->triangleIndex)
			{
				int blankReel = 2 - library.GetWeightedIndex(this->profile->triangleMultiplierWeightage[reelMode]);

				// Determine which reel to be blank
				if (blankReel > 0)
				{
					int randomizeIndex = library.RandomInt(1, 2 + 1);
					multiplierReelVector[randomizeIndex] = library.blankIndex;
				}
			}

			// Diamond Mode
			else if (shapeMode == this->profile->diamondIndex)
			{
				int blankReel = 3 - library.GetWeightedIndex(this->profile->diamondMultiplierWeightage[reelMode]);

				// Determine which reel to be blank
				if (blankReel > 0)
				{
					vector<int> blankCol = {0, 1, 2};

					while (blankReel > 0)
					{
						int randomizeIndex = library.RandomInt(0, blankCol.size());
						multiplierReelVector[blankCol[randomizeIndex]] = library.blankIndex;

						blankReel--;
						blankCol.erase(blankCol.begin() + randomizeIndex);
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
