#ifndef RG_GAME_CONFIG_H
#define RG_GAME_CONFIG_H
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
#include "../../libs/json.hpp"

using namespace std;

struct GameConfig
{
  vector<vector<vector<int>>> reelStripCircle;
  vector<vector<vector<int>>> reelStripTriangle;
  vector<vector<vector<int>>> reelStripDiamond;
  vector<vector<int>> paytable;
  vector<vector<int>> payline;
  int circleIndex;
  int triangleIndex;
  int diamondIndex;
  vector<int> reelWindowSize;
  vector<vector<int>> triangleMultiplierWeightage;
  vector<vector<int>> diamondMultiplierWeightage;
  vector<vector<int>> mgReelWeightage;
  vector<vector<vector<int>>> multiplierWeightage;
  int baseBet;
  int selectedPayLine;
  int maxWinMultiplier;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
      GameConfig,
      reelStripCircle,
      reelStripTriangle,
      reelStripDiamond,
      paytable,
      payline,
      circleIndex,
      triangleIndex,
      diamondIndex,
      reelWindowSize,
      triangleMultiplierWeightage,
      diamondMultiplierWeightage,
      mgReelWeightage,
      multiplierWeightage,
      baseBet,
      selectedPayLine,
      maxWinMultiplier)
};

// Declare derived structs
struct GameConfigBase : GameConfig
{
  GameConfigBase();
};

// Declare derived structs
struct GameConfig88 : GameConfigBase
{
  GameConfig88();
};
struct GameConfig94 : GameConfigBase
{
  GameConfig94();
};
struct GameConfig97 : GameConfigBase
{
  GameConfig97();
};
struct GameConfig120 : GameConfigBase
{
  GameConfig120();
};
struct GameConfig140 : GameConfigBase
{
  GameConfig140();
};

#endif // RG_GAME_CONFIG_H