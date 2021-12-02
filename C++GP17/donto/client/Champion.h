#pragma once

#include "parser.h"
#include "distcache.h"
#include "Action.h"
#include "MapCat.h"

struct CLIENT_INFO
{
	unordered_set<POS> mMinionsWereHere;
	vector<int> mFriendlyTurretsPerZone;
};


struct CHAMPION
{
	PARSER & mParser;
	DISTCACHE & mDistCache;
	MAP_OBJECT& mHero;
	const CLIENT_INFO& mInfo;

	CHAMPION(PARSER & Parser, DISTCACHE & DistCache, const CLIENT_INFO& ClientInfo, MAP_OBJECT & Hero)
		: mParser(Parser)
		, mDistCache(DistCache)
		, mInfo(ClientInfo)
		, mHero(Hero)
	{
	}

	const int HeroDamage(int side)
	{
		return int(HERO_DAMAGE_BASE + HERO_DAMAGE_PER_LEVEL*mParser.level[side]);
	}

	OWNERSHIP ACTION* Process();

	ACTION* ProceedInLane(const vector<ZONE> & targetZones);

	MAP_OBJECT* FindEnemy(const vector<ZONE> & targetZones);

	ACTION* MoveToBetterLane(const vector<ZONE> & targetZones);

	MAP_OBJECT* GetClosestEnemyTurretInZone(ZONE zone);
	MAP_OBJECT* GetClosestEnemyInZone(POS pos, ZONE zone);
	MAP_OBJECT& GetHomeBase();
	MAP_OBJECT& GetEnemyBase();
	POS GetClosestInZone(const POS& p, ZONE z);
	POS GetNextInZoneTowards(const POS &p0, const POS &p1);
	POS GetBestAttackPos(const MAP_OBJECT& enemy);

	int GetZoneHp(ZONE zone);
	int GetMaxPossibleEnemyDamage(POS pos);
	POS GetNextLeastAttackedPos();
	bool IsFriendlyMinionOrTurretInRange(POS pos, int range);
};

