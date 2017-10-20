#pragma once

enum class LevelBrickContent
{
	None,
	BallSizePlus,
	BallSizeMinus,
	RacketWidthPlus,
	RacketWidthMinus,
	Fireball,
	BallPlus,
	HPPlus,
	HPMinus
};

struct LevelBrick
{
	unsigned durability{ 0 };
	LevelBrickContent content{ LevelBrickContent::None };
};

