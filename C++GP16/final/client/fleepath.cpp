#include "stdafx.h"
#include "parser.h"
#include "fleepath.h"

using namespace std;

bool FLEEPATH::AdjecentPos(POS &pos, int dir)
{
	POS p=pos.ShiftDir(dir);
	if (p.x<0 || p.x>=map_dx || p.y<0 || p.y>=map_dx || DistanceToFriendlyCreep[p.x+p.y*map_dx]<0)
	{
		return false;		
	}
	pos=p;
	return true;
}
void FLEEPATH::CreateCreepDist(PARSER *pParser)
{
	int start_t = GetTickCount();
	map_dx=pParser->w;
	map_dy=pParser->h;

	DistanceToFriendlyCreep.resize(map_dx*map_dy);
	DamageOnEnemyCreep.resize(map_dx*map_dy);
	POS p;
	for (p.y = 0; p.y<map_dy; p.y++)
		for (p.x = 0; p.x<map_dx; p.x++)
		{
			PARSER::eGroundType g_t = pParser->GetAt(p);
			DistanceToFriendlyCreep[p.x + p.y*map_dx] = DamageOnEnemyCreep[p.x + p.y*map_dx] = 
				g_t== PARSER::WALL ? -1 :
				g_t==PARSER::CREEP ? 0 :
				g_t == PARSER::ENEMY_CREEP ? -3 :
				-2;
		}

	vector<POS> NextDamageLevelFront;
	vector<POS> EquiDamageFront;

	for (p.y = 0; p.y<map_dy; p.y++)
		for (p.x = 0; p.x<map_dx; p.x++)
		{
			if (DistanceToFriendlyCreep[p.x + p.y*map_dx] <= -2)
			{
				if (p.x>0 && DistanceToFriendlyCreep[p.x - 1 + p.y*map_dx] == 0 ||
					p.x<map_dx - 1 && DistanceToFriendlyCreep[p.x + 1 +p. y*map_dx] == 0 ||
					p.y>0 && DistanceToFriendlyCreep[p.x + (p.y - 1)*map_dx] == 0 ||
					p.y<map_dy - 1 && DistanceToFriendlyCreep[p.x + (p.y + 1)*map_dx] == 0)
				{
					if (DistanceToFriendlyCreep[p.x + p.y*map_dx] == -2)
					{
						DamageOnEnemyCreep[p.x + p.y*map_dx] = 0;
						DistanceToFriendlyCreep[p.x + p.y*map_dx] = 1;
						EquiDamageFront.push_back(p);
					}
					else
					{
						DamageOnEnemyCreep[p.x + p.y*map_dx] = 1;
						DistanceToFriendlyCreep[p.x + p.y*map_dx] = 1;
						NextDamageLevelFront.push_back(p);
					}
				}
			}
		}
	unsigned int idx;
	for (idx = 0; idx<EquiDamageFront.size() || !NextDamageLevelFront.empty(); idx++)
	{
		if (idx >= EquiDamageFront.size())
		{
			std::swap(NextDamageLevelFront, EquiDamageFront);
			NextDamageLevelFront.clear();
			idx = 0;
		}
		p=EquiDamageFront[idx];
		int damage = DamageOnEnemyCreep[p.x + p.y*map_dx];
		int dist = DistanceToFriendlyCreep[p.x + p.y*map_dx];
		assert(dist >= 0 && damage >= 0);
		for(int dir=0;dir<4;dir++)
		{
			POS p1=p.ShiftDir(dir);
			if (p1.x<0 || p1.x>=map_dx || p1.y<0 || p1.y>=map_dy) continue;
			if (DistanceToFriendlyCreep[p1.x+p1.y*map_dx]<=-2)
			{
				if (DistanceToFriendlyCreep[p1.x + p1.y*map_dx]==-2)
				{
					DistanceToFriendlyCreep[p1.x + p1.y*map_dx] = dist + 1;
					DamageOnEnemyCreep[p1.x + p1.y*map_dx] = damage;
					EquiDamageFront.push_back(p1);
				} else
				{
					DistanceToFriendlyCreep[p1.x + p1.y*map_dx] = dist + 1;
					DamageOnEnemyCreep[p1.x + p1.y*map_dx] = damage + 1;
					NextDamageLevelFront.push_back(p1);
				}
			}
		}
	}
	FILE *f = fopen("fleepath.log", "wt");
	fprintf(f, "t=%d\n", GetTickCount() - start_t);
	for (p.y = 0; p.y<map_dy; p.y++)
	{
		for (p.x = 0; p.x<map_dx; p.x++)
		{
			if (DistanceToFriendlyCreep[p.x + p.y*map_dx] == -1) fprintf(f, "  ");
			else fprintf(f, "%2d", DistanceToFriendlyCreep[p.x + p.y*map_dx]);
		}
		fprintf(f, "\n");
		for (p.x = 0; p.x<map_dx; p.x++)
		{
			if (DamageOnEnemyCreep[p.x + p.y*map_dx]==-1) fprintf(f, "  ");
			else fprintf(f, "%2d", DamageOnEnemyCreep[p.x + p.y*map_dx]);
		}
		fprintf(f, "\n");
	}
	fclose(f);
	for (p.y = 0; p.y<map_dy; p.y++)
	{
		for (p.x = 0; p.x<map_dx; p.x++)
		{
			assert(DistanceToFriendlyCreep[p.x + p.y*map_dx] > -2);
			assert(DamageOnEnemyCreep[p.x + p.y*map_dx] > -2);
		}
	}
}
int FLEEPATH::GetDistToFriendlyCreep(const POS &p)
{
	return DistanceToFriendlyCreep[p.x + p.y*map_dx];
}
int FLEEPATH::GetDamageOnEnemyCreep(const POS &p)
{
	return DamageOnEnemyCreep[p.x + p.y*map_dx];
}
POS FLEEPATH::GetNextOffCreep(const POS &p)
{
	int d = DistanceToFriendlyCreep[p.x+p.y*map_dx];
	if (d<0) return POS(0, 0);
	int min_dist = 0xFF;
	int min_dmg = 0xFF;
	int count = 0;
	POS ret;
	for (int dir = 0; dir<4; dir++)
	{
		POS p2 = p;
		if (!AdjecentPos(p2, dir)) continue;
		int dist = DistanceToFriendlyCreep[p2.x + p2.y*map_dx];
		int dmg = DistanceToFriendlyCreep[p2.x + p2.y*map_dx];
		if (dist<0) continue;
		if (dist<min_dist || (dist==min_dist && dmg<min_dmg))
		{
			min_dist = dist;
			min_dmg = dmg;
			count = 1;
			ret = p2;
		}
		else if (dist == min_dist && dmg==min_dmg)
		{
			count++;
			if (((p.x+p.y) % count) == 0)
			{
				ret = p2;
			}
		}
	}
	assert(ret.x != 0);
	return ret;
}
