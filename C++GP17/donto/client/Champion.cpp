#include "stdafx.h"
#include "Champion.h"

static const int MAX_TICK_FOR_BEEFING_UP = 300;

OWNERSHIP ACTION* CHAMPION::Process()
{
	//if (mHero.id >= 3)
	//	return NULL;

	ZONE myZone = Zones[mHero.pos];

	if (myZone==TOP_LANE || myZone==BOTTOM_LANE || myZone==MIDDLE_LANE || myZone==ENEMY_BASE)
	{
		return ProceedInLane(vector<ZONE> {myZone, ENEMY_BASE});
	}

	if (myZone==TOP_JUNGLE)
	{
		return MoveToBetterLane(vector<ZONE> { TOP_LANE, MIDDLE_LANE } );
	}

	if (myZone==BOTTOM_JUNGLE)
	{
		return MoveToBetterLane(vector<ZONE> { BOTTOM_LANE, MIDDLE_LANE });
	}

	if (myZone==HOME_BASE)
	{
		OWNERSHIP ACTION * action = ProceedInLane(vector<ZONE>());
		if (action)
			return action;

		return MoveToBetterLane(vector<ZONE> { BOTTOM_LANE, MIDDLE_LANE, TOP_LANE });
	}

	return NULL;
}

ACTION* CHAMPION::ProceedInLane(const vector<ZONE> & targetZones)
{
	MAP_OBJECT* enemy = NULL;

	if (Zones[mHero.pos] != ENEMY_BASE)
	{
		POS fleePos = GetNextLeastAttackedPos();
		if (mInfo.mMinionsWereHere.count(fleePos) && fleePos != mHero.pos)
		{
			return new MOVE(fleePos);
		}
	}

	enemy = FindEnemy(targetZones);

	if (enemy == NULL)
		return NULL;

	//printf("hero %d (in zone %d) targeting enemy %d in zone %d\n", mHero.id, Zones[mHero.pos], enemy->id, Zones[enemy->pos]);

	int dist = mHero.pos.DistSquare(enemy->pos);
	if (dist <= HERO_RANGE_SQ)
	{
		return new ATTACK(enemy->id);
	}
	else
	{
		POS targetPos = GetNextInZoneTowards(mHero.pos, GetBestAttackPos(*enemy));
		//printf("next step: %d,%d, current damage: %d, next damage: %d\n", POS_TO_FMT(targetPos), GetMaxPossibleEnemyDamage(mHero.pos), GetMaxPossibleEnemyDamage(targetPos));

		if (!targetPos.IsValid())
			return NULL;

		if (Zones[mHero.pos] == ENEMY_BASE || mInfo.mMinionsWereHere.count(mHero.pos)==0 || GetMaxPossibleEnemyDamage(targetPos) <= GetMaxPossibleEnemyDamage(mHero.pos))
			return new MOVE(targetPos);
		else
			return NULL;
	}
}

MAP_OBJECT* CHAMPION::FindEnemy(const vector<ZONE> & targetZones)
{
	vector<MAP_OBJECT*> inRange;
	for (MAP_OBJECT& obj : mParser.Units)
	{
		if (obj.side==1)
		{
			if ((mHero.pos.DistSquare(obj.pos)) <= HERO_RANGE_SQ)
				inRange.push_back(&obj);
		}
	}


	sort(inRange.begin(), inRange.end(),
		[this](const MAP_OBJECT* a, const MAP_OBJECT* b)
		{
			map<UNIT_TYPE, int> prio{ {TURRET, 0}, {BASE, 1}, {MINION, 2}, {HERO, 3} };

			if (mParser.level[0] > 100)
				prio[MINION] = -1;
			if (a->t == b->t)
				return a->hp < b->hp;
			
			return prio[a->t] < prio[b->t];
		});

	if (!inRange.empty())
		return inRange[0];

	if (targetZones.empty())
	{	// special mode: look for enemy near home base
		for (auto &obj : mParser.Units)
		{
			const int DANGER_ZONE_SQ = 70;
			if (obj.side==1 && obj.t == HERO && obj.pos.DistSquare(GetHomeBase().pos) < DANGER_ZONE_SQ)
				return &obj;
		}
	}
	else
	{
		for (ZONE z : targetZones)
		{
			MAP_OBJECT* enemy = GetClosestEnemyInZone(mHero.pos, z);
			if (enemy != NULL)
				return enemy;
		}
	}
	return NULL;
}

POS CHAMPION::GetBestAttackPos(const MAP_OBJECT& enemy)
{
	pair<int, int> best(INT_MAX, INT_MAX);
	POS best_pos;
	for (int x = -3; x <= 3; x++)
	{
		for (int y = -3; y <= 3; y++)
		{
			POS p(enemy.pos.x + x, enemy.pos.y + y);
			if (!p.IsValid() || Zones[p] != Zones[enemy.pos] || enemy.pos.DistSquare(p) > HERO_RANGE_SQ)
				continue;

			pair<int, int> current(GetMaxPossibleEnemyDamage(p), mDistCache.GetDist(mHero.pos, p));
			if (current < best)
			{
				best = current;
				best_pos = p;
			}
		}
	}
	//printf("Best attack pos for enemy %d at %d,%d is %d,%d\n", enemy.id, POS_TO_FMT(enemy.pos), POS_TO_FMT(best_pos));
	return best_pos;
}

ACTION* CHAMPION::MoveToBetterLane(const vector<ZONE> & targetZones)
{
	const bool bAgressive = mParser.tick > MAX_TICK_FOR_BEEFING_UP ||
		mInfo.mFriendlyTurretsPerZone[TOP_LANE] == 0 ||
		mInfo.mFriendlyTurretsPerZone[MIDDLE_LANE] == 0 ||
		mInfo.mFriendlyTurretsPerZone[BOTTOM_LANE] == 0 ||
		mInfo.mFriendlyTurretsPerZone[HOME_BASE] == 0;

	POS targetPos;
	if (bAgressive)
	{
		vector<pair<int, ZONE>> hz; // (hp, zone)
		for (ZONE z : targetZones)
		{
			hz.emplace_back(GetZoneHp(z), z);
		}

		assert(hz.size() >= 2);
		sort(hz.begin(), hz.end());

		POS closestInZone0 = GetClosestInZone(mHero.pos, hz[0].second);

		int threshold = 40;
		if (hz[0].first < hz[1].first - threshold)
		{
			targetPos = closestInZone0;
		}
		else
		{
			POS closestInZone1 = GetClosestInZone(mHero.pos, hz[1].second);
			int dist0 = hz[0].second == MIDDLE_LANE ? 0 : mDistCache.GetDist(mHero.pos, closestInZone0);
			int dist1 = hz[1].second == MIDDLE_LANE ? 0 : mDistCache.GetDist(mHero.pos, closestInZone1);
			if (dist0 < dist1)
				targetPos = closestInZone0;
			else
				targetPos = closestInZone1;
		}
	}
	else
	{
		auto get_hp = [](MAP_OBJECT* obj) { return obj ? obj->hp : INT_MAX; };
		vector<tuple<int, int, ZONE>> mClosestEnemyHPs;
		for (ZONE z : targetZones)
		{
			mClosestEnemyHPs.emplace_back(get_hp(GetClosestEnemyInZone(mHero.pos, z)), GetZoneHp(z), z);
		}
		auto it = min_element(mClosestEnemyHPs.begin(), mClosestEnemyHPs.end());
		targetPos = GetClosestInZone(mHero.pos, get<ZONE>(*it));
	}

	return new MOVE(mDistCache.GetNextTowards(mHero.pos, targetPos));
}

MAP_OBJECT* CHAMPION::GetClosestEnemyTurretInZone(ZONE zone)
{
	// int minDist = std::numeric_limits<int>::max();
	int minDist = INT_MAX;
	MAP_OBJECT* closestEnemyTower = NULL;
	POS refPoint;
	switch (zone)
	{
		case TOP_LANE:
			refPoint = POS(0, 38);
			break;
		case MIDDLE_LANE:
			refPoint = POS(0, 0);
			break;
		case BOTTOM_LANE:
			refPoint = POS(38, 0);
			break;
		default:
			return NULL;
	}

	for (auto& unit : mParser.Units)
	{
		// Enemy turrets
		if (unit.side == 1 && unit.t == TURRET)
		{
			// Is it in given zone?j
			auto& itZones = Zones.find(unit.pos);
			if (itZones != Zones.end() && itZones->second == zone)
			{
				// Is it the closest one?
				int dist = refPoint.DistSquare(unit.pos);
				if (dist < minDist)
				{
					minDist = dist;
					closestEnemyTower = &unit;
				}
			}
		}
	}
	return closestEnemyTower;
}

MAP_OBJECT* CHAMPION::GetClosestEnemyInZone(POS pos, ZONE zone)
{
	int minDist = INT_MAX;
	MAP_OBJECT* closestEnemy = NULL;

	for (auto& unit : mParser.Units)
	{
		// Enemy turrets
		if (unit.side == 1 && Zones[unit.pos] == zone)
		{
			if (zone == ENEMY_BASE && unit.t==MINION)
				continue;

			// Is it the closest one?
			int dist = mDistCache.GetDist(pos, unit.pos);
			if (dist < minDist)
			{
				minDist = dist;
				closestEnemy = &unit;
			}
		}
	}
	return closestEnemy;
}

#pragma warning (push)
#pragma warning (disable: 4715)
MAP_OBJECT& CHAMPION::GetHomeBase()
{
	for (auto& unit : mParser.Units)
	{
		if (unit.side == 0 && unit.t==BASE)
			return unit;
	}
}

MAP_OBJECT& CHAMPION::GetEnemyBase()
{
	for (auto& unit : mParser.Units)
	{
		if (unit.side == 1 && unit.t==BASE)
			return unit;
	}
}
#pragma warning(pop)

POS CHAMPION::GetClosestInZone(const POS& p, ZONE z)
{
	int min_dist = INT_MAX;
	POS closest;
	for (auto &v : Zones)
	{
		if (v.second != z)
			continue;

		int dist = mDistCache.GetDist(p, v.first);
		if (dist < min_dist)
		{
			min_dist = dist;
			closest = v.first;
		}
	}
	return closest;
}

POS CHAMPION::GetNextInZoneTowards(const POS &p0, const POS &p1)
{
	const ZONE z0 = Zones[p0];
	const ZONE z1 = Zones[p1];

	if (z0 != z1)
	{
		POS closestInZone(GetClosestInZone(p0, z1));
		return mDistCache.GetNextTowards(p0, closestInZone);
	}

	// do BFS towards p1
	queue<POS> q;
	unordered_map<POS, POS> parents;
	q.push(p0);
	while (!q.empty())
	{
		POS p = q.front();
		if (p == p1)
			break;
		q.pop();

		for (int dx = -1; dx < 2; ++dx)
		{
			for (int dy = -1; dy < 2; ++dy)
			{
				POS pn(p.x+dx, p.y+dy);
				if (pn != p && Zones[pn] == z0 && !parents[pn].IsValid())
				{
					parents[pn] = p;
					q.push(pn);
				}
			}
		}
	}
	assert(!q.empty());

	POS p = p1;
	while (parents[p] != p0)
	{
		p = parents[p];
	}
	return p;
}

int CHAMPION::GetMaxPossibleEnemyDamage(POS pos)
{
	int sumEnemyDamage = 0;
	for (auto& unit : mParser.Units)
	{
		if (unit.side == 1 && unit.t == TURRET &&
			!IsFriendlyMinionOrTurretInRange(unit.pos, TURRET_RANGE_SQ) &&
			pos.DistSquare(unit.pos) <= TURRET_RANGE_SQ)
		{
			int enemyDamage = (unit.hp * TURRET_DAMAGE > mHero.hp * HeroDamage(0))  ? TURRET_DAMAGE : 0;
			sumEnemyDamage += enemyDamage;
		}
	}
	return sumEnemyDamage;
}

POS CHAMPION::GetNextLeastAttackedPos()
{
	int minSumDamage = GetMaxPossibleEnemyDamage(mHero.pos);
	POS minFleePos = mHero.pos;
	for (int i = -1; i < 2; ++i)
	{
		for (int j = -1; j < 2; ++j)
		{
			POS fleePos(mHero.pos);
			fleePos.x += i;
			fleePos.y += j;
			if (Zones[fleePos] == WALL || fleePos == mHero.pos)
				continue;
			int sumDamage = GetMaxPossibleEnemyDamage(fleePos);
			if (sumDamage < minSumDamage)
			{
				minSumDamage = sumDamage;
				minFleePos = fleePos;
			}
		}
	}
	return minFleePos;
}

int CHAMPION::GetZoneHp(ZONE zone)
{
	int sum = 0;
	for (auto& unit : mParser.Units)
	{
		if (unit.side == 1 && Zones[unit.pos] == zone)
		{
			sum += unit.hp;
		}
	}
	return sum;
}

bool CHAMPION::IsFriendlyMinionOrTurretInRange(POS pos, int range)
{
	for (auto& unit : mParser.Units)
	{
		if (unit.side == 0 && (unit.t == TURRET || unit.t == MINION) && pos.DistSquare(unit.pos) <= range)
		{
			return true;
		}
	}
	return false;
}

