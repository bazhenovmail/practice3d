#pragma once

#include <vector>
#include "LevelBrick.h"

struct Level
{
	size_t width{ 0 };
	size_t height{ 0 };
	std::vector<LevelBrick> bricks;
};

