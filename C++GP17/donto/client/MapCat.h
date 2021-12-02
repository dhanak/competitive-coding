#pragma once

#include "parser.h"

enum ZONE
{
	WALL,
	TOP_LANE,
	MIDDLE_LANE,
	BOTTOM_LANE,
	TOP_JUNGLE,
	BOTTOM_JUNGLE,
	HOME_BASE,
	ENEMY_BASE,

	ZONE_COUNT
};

namespace std
{
	template<>
	struct hash<POS>
	{
		size_t operator()(const POS &p) const { return p.x ^ (p.y << 1); }
	};
}

extern unordered_map<POS, ZONE> Zones;
