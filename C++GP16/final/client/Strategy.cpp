#include "stdafx.h"
#include "MyClient.h"
#include "Strategy.h"
#include "util.h"

bool STRATEGY::AttackIfNearby(int attack_flags /* = ALL */)
{
	const MAP_OBJECT &self = Myself();
	if (mClient->IsUnderAttackBy(self.pos)==-1)
		return false;

	for (auto &unit : mClient->mParser.Units)
	{
		if (unit.side == 0)
			continue;
		if (!self.pos.IsNear(unit.pos))
			continue;
		if ((attack_flags & SICKER) == 0 && unit.hp < self.hp ||
			(attack_flags & HEALTHIER) == 0 && unit.hp > self.hp)
			continue;
		PARSER::eGroundType groundType = mClient->mParser.GetAt(unit.pos);
		if (((attack_flags & ON_HOME_GROUND) == 0 && groundType == PARSER::CREEP) ||
			((attack_flags & ON_NEUTRAL_GROUND) == 0 && groundType == PARSER::EMPTY) ||
			((attack_flags & ON_ENEMY_GROUND) == 0 && groundType == PARSER::ENEMY_CREEP))
			continue;

		mClient->command_buffer << "queen_attack " << mUnitId << ' ' << unit.id << '\n';
		//mClient->mUnitTarget[mUnitId] = CLIENT::CMD(CLIENT::CMD_ATTACK, unit.pos, unit.id);
		return true;
	}
	return false;
}

void CREEPER::DoIt()
{
	if (AttackIfNearby())
		return;

	if (mClient->mFutureTumors.empty())
		return;

	if (contains(mClient->mUnitTarget, mUnitId) && mFutureTumor.IsValid() &&
		mRealFuture && !mClient->HasTumor(mFutureTumor))
	{
		ASSERT(mClient->mUnitTarget[mUnitId] == CLIENT::CMD(CLIENT::CMD_SPAWN, mFutureTumor));
		return;
	}

	FUTURE_TUMOR_TYPE::iterator itFutureTumor = GetNearestFutureTumor();
	if (itFutureTumor != mClient->mFutureTumors.end())
	{
		mFutureTumor = *itFutureTumor;
		mRealFuture = true;
	}
	else
	{
		mFutureTumor = GetNearestSafeEmptyPos();
		mRealFuture = false;
	}

	CLIENT::CMD& oldCmd = mClient->mUnitTarget[mUnitId];
	CLIENT::CMD& newCmd = CLIENT::CMD(CLIENT::CMD_SPAWN, mFutureTumor);
	if (oldCmd != newCmd)
		std::cout << "Setting creeper #" << mUnitId << " to spawn at " << mFutureTumor << " - oldCmd: " << oldCmd.c << " " << oldCmd.pos << " " << oldCmd.target_id << std::endl;
	mClient->mUnitTarget[mUnitId] = newCmd;

	if (itFutureTumor != mClient->mFutureTumors.end())
	{
		mClient->mFutureTumors.erase(itFutureTumor);
	}
}

POS CREEPER::GetFarthestBorderCreep()
{
	POS iMaxBorderCreep(7, 14);
	int maxManhattan = -1;
	for (const auto& iBorderCreep : mClient->mBorderCreep)
	{
		int iManhattan = iBorderCreep.first.GetManhattan();
		if (maxManhattan < iManhattan)
		{
			iMaxBorderCreep = iBorderCreep.first;
			maxManhattan = iManhattan;
		}
	}
	return iMaxBorderCreep;
}

FUTURE_TUMOR_TYPE::iterator CREEPER::GetNearestFutureTumor()
{
	static const int nearSkipDistance = 8;
	int iMinFutureDistance = 99999;
	FUTURE_TUMOR_TYPE::iterator itMinFutureTumor = mClient->mFutureTumors.end();

	for (FUTURE_TUMOR_TYPE::iterator itFutureTumor = mClient->mFutureTumors.begin();
		itFutureTumor != mClient->mFutureTumors.end();
		++itFutureTumor)
	{
		int iFutureDistance = (*itFutureTumor - Myself().pos).GetManhattan();

		if (iFutureDistance <= nearSkipDistance)
			continue;

		if (iFutureDistance < iMinFutureDistance)
		{
			POS pos = Myself().pos;
			bool isPathClear = true;
			while (pos.IsValid() && isPathClear)
			{
				pos = mClient->mDistCache.GetNextTowards(pos, *itFutureTumor);
				if (mClient->mParser.GetAt(pos) == PARSER::ENEMY_CREEP)
					isPathClear = false;
			}

			if (isPathClear)
			{
				iMinFutureDistance = iFutureDistance;
				itMinFutureTumor = itFutureTumor;
			}
		}
	}

	return itMinFutureTumor;
}

POS CREEPER::GetNearestSafeEmptyPos()
{
	POS pos = Myself().pos;
	if (!mClient->HasTumor(pos) && mClient->mParser.GetAt(pos) == PARSER::CREEP)
	{
		return pos;
	}
	int stepSize = 1;
	for (int i = 0; i < 30; )
	{
		pos = pos.ShiftDirStep(i % 4, stepSize);
		if (!mClient->HasTumor(pos) && mClient->mParser.GetAt(pos) == PARSER::CREEP)
		{
			return pos;
		}
		++i;
		if (i % 2 == 0)
		{
			++stepSize;
		}
	}
	return mClient->mParser.OwnHatchery.pos;
}

bool ATTACKER::CanAttackHatchery(POS p)
{
	const int minc = mClient->mParser.EnemyHatchery.pos.x-1;
	const int maxc = minc+4;

	if (p.x < minc || p.x > maxc || p.y < minc || p.y > maxc)
		return false;
	if ((p.x == minc || p.x == maxc) && (p.y == minc || p.y == maxc))
		return false;
	return true;
}

void ATTACKER::DoIt()
{
	POS p = Myself().pos;
	int Cost = 0;
	int FirstNearEnemyId = -1;
	while (p.IsValid() && !CanAttackHatchery(p))
	{
		p = mClient->mDistCache.GetNextTowards(p, mClient->mParser.EnemyHatchery.pos);
		PARSER::eGroundType Ground = mClient->mParser.GetAt(p);
		if (Ground == PARSER::ENEMY_CREEP)
			Cost += HP_DECAY_ON_ENEMY_CREEP;

		FirstNearEnemyId = mClient->IsUnderAttackBy(p);
		if (FirstNearEnemyId != -1)
			break;
	}

	if (FirstNearEnemyId == -1)
	{
		std::cout << "Attacking hatchery with #" << Myself().id << " costs " << Cost << " hp\n";

		const int MinResidualHp = ((HATCHERY_MAX_HP / QUEEN_DAMAGE) / 4) * HP_DECAY_ON_ENEMY_CREEP;
		if (MinResidualHp + Cost < Myself().hp)
		{
			// attack hatchery
			mClient->mUnitTarget[mUnitId] = CLIENT::CMD(CLIENT::CMD_ATTACK, mClient->mParser.EnemyHatchery.pos, mClient->mParser.EnemyHatchery.id);
			return;
		}
	}

	std::cout << "Not attacking hatchery\n";

	int MinDistTumorId = -1;
	POS MinDistTumorPos;
	for (MAP_OBJECT& tumor : mClient->mParser.CreepTumors)
	{
		if (tumor.side == 0)
			continue;
		if (MinDistTumorId==-1 || mClient->mDistCache.GetDist(Myself().pos, tumor.pos) < mClient->mDistCache.GetDist(Myself().pos, MinDistTumorPos))
		{
			MinDistTumorPos = tumor.pos;
			MinDistTumorId = tumor.id;
		}
	}

	if (MinDistTumorId != -1)
	{
		mClient->mUnitTarget[mUnitId] = CLIENT::CMD(CLIENT::CMD_ATTACK, MinDistTumorPos, MinDistTumorId);
		return;
	}

	if (FirstNearEnemyId != -1)
	{
		mClient->mUnitTarget[mUnitId] = CLIENT::CMD(CLIENT::CMD_ATTACK, mClient->mParser.GetUnit(FirstNearEnemyId).pos, FirstNearEnemyId);
		return;
	}

	GUARDIAN(mClient, mUnitId).DoIt();
}

void GUARDIAN::DoIt()
{
	if (AttackIfNearby(EVERYONE | ON_HOME_GROUND))
		return;

	if (mUnitId != mClient->mGuardians[0])
	{
		mClient->mUnitTarget[mUnitId] = CLIENT::CMD(CLIENT::CMD_MOVE, mClient->mParser.GetUnit(mClient->mGuardians[0]).pos);
		return;
	}


	if (contains(mClient->mBorderCreep,  Myself().pos))
	{
		// border
		mClient->mUnitTarget.erase(mUnitId);
		mOnGuard = true;
	}
	else
	{
		// not border
		if (mClient->mParser.GetAt(Myself().pos) != PARSER::CREEP)
		{
			mClient->mUnitTarget[mUnitId] = CLIENT::CMD(CLIENT::CMD_MOVE, mClient->mFleePath.GetNextOffCreep(Myself().pos));
		}
		else
		{
			POS target = mClient->mDistCache.GetNextTowards(Myself().pos, mClient->mParser.EnemyHatchery.pos);
			if (mClient->IsUnderAttackBy(target) == -1)
			{
				mClient->mUnitTarget[mUnitId] = CLIENT::CMD(CLIENT::CMD_MOVE, target);
			}
		}
		mOnGuard = false;
	}
}

bool GUARDIAN::OnGuard()
{
	return mOnGuard;
}

void FLEER::DoIt()
{
	mClient->mUnitTarget[mUnitId] = CLIENT::CMD(CLIENT::CMD_MOVE, mClient->mFleePath.GetNextOffCreep(Myself().pos));
}

void DEFENDER::DoIt()
{
	mClient->mUnitTarget[mUnitId] = CLIENT::CMD(CLIENT::CMD_ATTACK, mClient->mDangerUnit.pos, mClient->mDangerUnit.id);
}
