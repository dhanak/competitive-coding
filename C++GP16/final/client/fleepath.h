#pragma once

class FLEEPATH 
{	
	int map_dx, map_dy;
	std::vector<int> DamageOnEnemyCreep; // -1: wall, -2: tmp empty, -3: tmp enemy creep
	std::vector<int> DistanceToFriendlyCreep;
	bool AdjecentPos(POS &p, int dir);
public:
	FLEEPATH() {};
	void CreateCreepDist(PARSER *pParser);
	int GetDistToFriendlyCreep(const POS &p);
	int GetDamageOnEnemyCreep(const POS &p);
	POS GetNextOffCreep(const POS &p);
};
