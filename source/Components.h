#pragma once

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
	int x;
	int y;
};

struct SizeComponent : Component {
	int height;
	int width;
};

struct IdleComponent : Component {
	char* textureLocation;
	int framesPerSecond;
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