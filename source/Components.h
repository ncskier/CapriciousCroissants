#pragma once
#include "cugl\cugl.h"
#include <memory>
#include <string>

//All structs are components and MUST extend Component struct
struct Component {};

struct DumbMovementComponent : Component {
	int movementDistance;
};

struct SmartMovementComponent : Component {
	int movementDistance;
};

struct ImmobileMovementComponent : Component {
};

struct TeleportMovementComponent : Component {
	int movementDistance;
};

struct MeleeAttackComponent : Component {
};

struct RangeOrthoAttackComponent : Component {
	bool horizontal;
	bool vertical;
};

struct RangeDiagAttackComponent : Component {
};

struct SplashAttackModComponent : Component {
	int splashArea;
};

struct DuplicationOnTurnComponent : Component {
	int turnsPerDuplicate;
	int numberOfDuplicates;
};

struct TileMoveOnTurnComponent : Component {
};

struct LocationComponent : Component {
	enum direction : unsigned int{
		UP, DOWN, RIGHT, LEFT
	};

	int x;
	int y;

	direction dir;
};

struct SizeComponent : Component {
	int height;
	int width;
};

struct IdleComponent : Component {
	std::vector<std::string> textureKey;
	std::vector<std::tuple<int, int>> textureStartStopFrame;
	std::vector<int> textureRows;
	std::vector<int> textureColumns;
	std::vector<int> textureSize;
	std::vector<int> speed;

	std::shared_ptr<cugl::AnimationNode> sprite;
	std::shared_ptr<cugl::ActionManager> _actions;
	std::set<std::string> _interruptingActions;
};

struct NonIdleComponent : Component {
	char* textureLocation;
	int framesPerSecond;
};

struct ExplodeOnDeathComponent : Component {
	int radius;
	bool affectTiles;
};

struct WeaknessResistComponent : Component {
	int* weaknesses;
};

struct StrengthResistComponent: Component {
	int* strengths;
};

struct ImmovablePlayerLimitComponent : Component {
};

struct FlyPlayerLimitComponent : Component {
};