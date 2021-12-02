#pragma once

#include "Client.h"

struct ACTION
{
	virtual void Execute(CLIENT* Client, int HeroId) = 0;
	virtual ~ACTION() {}
};

struct ATTACK : public ACTION
{
	int mTargetId;

	explicit ATTACK(int TargetId)
		: mTargetId(TargetId)
	{}

	virtual void Execute(CLIENT* Client, int HeroId) override
	{
		Client->Attack(HeroId, mTargetId);
		MAP_OBJECT * pEnemy = Client->mParser.GetUnitByID(mTargetId);
		//printf("ATTACK %d: enemy %d (type %d) at %d,%d\n",
		//	HeroId,
		//	mTargetId,
		//	pEnemy ? pEnemy->t : -1,
		//	POS_TO_FMT(pEnemy ? pEnemy->pos : POS())
		//	);
	}
};

struct MOVE : public ACTION
{
	POS mTargetPos;

	explicit MOVE(POS TargetPos)
		: mTargetPos(TargetPos)
	{}

	virtual void Execute(CLIENT* Client, int HeroId) override
	{
		if (mTargetPos==POS())
			printf("Going to 0,0\n");
		//MAP_OBJECT * pHero = Client->mParser.GetUnitByID(HeroId);
		//printf("MOVE %d: %d,%d -> %d,%d\n", HeroId, POS_TO_FMT(pHero ? pHero->pos : POS()), POS_TO_FMT(mTargetPos));
		Client->Move(HeroId, mTargetPos);
	}
};