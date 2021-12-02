#include "stdafx.h"
#include "MyClient.h"
#include "Strategy.h"
#include "parser.h"
#include "util.h"

MYCLIENT::MYCLIENT()
{
}

void MYCLIENT::Process()
{
	mFleePath.CreateCreepDist(&mParser);
	CalculateUnderAttack();
	FillOwnTumors();
	RemoveDeadUnits();
	FindBorderCreeps();
	CollectFutureTumors();
	SpawnTumors();
	DetermineStrategies();
	CollectGuardians();

	for (auto &s : mStrategies)
	{
		s.second->DoIt();
	}

	FillPrevTickMap();

	for (auto& it : mStrategies)
	{
		std::cout << it.first << ": " << it.second->GetName() << ", ";
	}
	std::cout << std::endl;
}

void MYCLIENT::CalculateUnderAttack()
{
	mUnderAttack.clear();
	for (auto &unit : mParser.Units)
	{
		if (unit.side != 0)
		{
			mUnderAttack[unit.pos] = unit.id;
			mUnderAttack[unit.pos.ShiftDir(POS::SHIFT_LEFT)] = unit.id;
			mUnderAttack[unit.pos.ShiftDir(POS::SHIFT_RIGHT)] = unit.id;
			mUnderAttack[unit.pos.ShiftDir(POS::SHIFT_UP)] = unit.id;
			mUnderAttack[unit.pos.ShiftDir(POS::SHIFT_DOWN)] = unit.id;
		}
	}
}

void MYCLIENT::FillOwnTumors()
{
	mOwnTumors.clear();
	for (auto &tumor : mParser.CreepTumors)
	{
		mOwnTumors[tumor.pos] = tumor.id;
	}
}

void MYCLIENT::RemoveDeadUnits()
{
	std::vector<int> liveUnits;
	liveUnits.reserve(mParser.Units.size());
	for (auto &unit : mParser.Units)
	{
		liveUnits.push_back(unit.id);
	}
	std::sort(liveUnits.begin(), liveUnits.end());
	for (auto iStrat = mStrategies.begin(); iStrat != mStrategies.end(); )
	{
		if (!std::binary_search(liveUnits.begin(), liveUnits.end(), iStrat->first))
		{
			iStrat = mStrategies.erase(iStrat);
		}
		else
		{
			++iStrat;
		}
	}
}

void MYCLIENT::FindBorderCreeps()
{
	auto isBorder = [this](POS p) { auto t = mParser.GetAt(p); return t == PARSER::EMPTY || t == PARSER::ENEMY_CREEP; };

	mBorderCreep.clear();
	for (int y = 1; y < mParser.h-1; y++)
	{
		for (int x = 1; x < mParser.w-1; x++)
		{
			POS p(x, y);
			if (mParser.GetAt(p) == PARSER::CREEP)
			{
				std::array<int, PARSER::GROUND_TYPE_COUNT> cnt = { 0 };
				cnt[mParser.GetAt(p.ShiftDir(POS::SHIFT_LEFT))]++;
				cnt[mParser.GetAt(p.ShiftDir(POS::SHIFT_RIGHT))]++;
				cnt[mParser.GetAt(p.ShiftDir(POS::SHIFT_UP))]++;
				cnt[mParser.GetAt(p.ShiftDir(POS::SHIFT_DOWN))]++;
				cnt[PARSER::EMPTY] += cnt[PARSER::CREEP_CANDIDATE_FRIENDLY] + cnt[PARSER::CREEP_CANDIDATE_ENEMY] + cnt[PARSER::CREEP_CANDIDATE_BOTH];
				if (cnt[PARSER::EMPTY] + cnt[PARSER::ENEMY_CREEP] > 0)
				{
					mBorderCreep[p] = BORDER_INFO(cnt[PARSER::EMPTY]>0, cnt[PARSER::ENEMY_CREEP]>0, cnt[PARSER::WALL]>0);
				}
			}
		}
	}
}

const int SPAWN_RADIUS = 10;
const int SPAWN_RADIUS_2_SQ = 4*SPAWN_RADIUS*SPAWN_RADIUS;

void MYCLIENT::SpawnTumors()
{
	for (auto &tumor : mParser.CreepTumors)
	{
		if (tumor.side != 0 || tumor.energy < CREEP_TUMOR_SPAWN_ENERGY)
			continue;

		POS spawnTo;
		for (auto it = mFutureTumors.begin(); it != mFutureTumors.end(); ++it)
		{
			auto &future = *it;
			int dist2sq = tumor.pos.dist2sq(future);
			if (dist2sq <= SPAWN_RADIUS_2_SQ && !HasTumor(future))
			{
				std::cout << "Spawning tumor #" << tumor.id << " at " << tumor.pos << " onto ideal future " << future << " (dist " << dist2sq << ')' << std::endl;
				spawnTo = future;
				mFutureTumors.erase(it);
				break;
			}
		}

		if (!spawnTo.IsValid())
		{
			int spawnDist = -1;
			bool spawnOnFront = false;
			for (auto &border : mBorderCreep)
			{
				int dist2sq = tumor.pos.dist2sq(border.first);
				if (dist2sq <= SPAWN_RADIUS_2_SQ && !HasTumor(border.first) && (dist2sq > spawnDist || (!spawnOnFront && border.second.nearEnemyCreep)))
				{
					spawnDist = dist2sq;
					spawnOnFront = border.second.nearEnemyCreep;
					spawnTo = border.first;
				}
			}
			if (spawnTo.IsValid())
			{
				std::cout << "Spawning tumor #" << tumor.id << " at " << tumor.pos << " onto border at " << spawnTo << " (dist " << spawnDist << ')' << std::endl;
			}
		}
		if (!spawnTo.IsValid())
		{
			int bestScore = INT_MIN;
			for (int x = tumor.pos.x - SPAWN_RADIUS + 1; x < tumor.pos.x + SPAWN_RADIUS; ++x)
			{
				for (int y = tumor.pos.y - SPAWN_RADIUS + 1; y < tumor.pos.y + SPAWN_RADIUS; ++y)
				{
					POS p(x, y);
					int dist2sq = tumor.pos.dist2sq(p);
					int score = dist2sq - mParser.EnemyHatchery.pos.dist2sq(p);
					if (dist2sq < SPAWN_RADIUS_2_SQ && !HasTumor(p) && mParser.GetAt(p) == PARSER::CREEP && score > bestScore)
					{
						bestScore = score;
						spawnTo = p;
					}
				}
			}
			if (spawnTo.IsValid())
			{
				std::cout << "Spawning tumor #" << tumor.id << " at " << tumor.pos << " onto " << spawnTo << std::endl;
			}
		}
		if (spawnTo.IsValid())
		{
			command_buffer << "creep_tumor_spawn " << tumor.id << ' ' << spawnTo << '\n';
		}
		else
		{
			std::cout << "Dead tumor #" << tumor.id << " at " << tumor.pos << std::endl;
		}
	}
}

void MYCLIENT::FindDefenders()
{
	mDefenders.clear();
	mDangerUnit = MAP_OBJECT();

	std::vector<std::pair<int, int>> ownDistanceMap; // distance -> unit id
	std::vector<std::pair<int, int>> enemyDistanceMap; // distance -> unit id
	for (auto& unit : mParser.Units)
	{
		auto &m = (unit.side == 0) ? ownDistanceMap : enemyDistanceMap;
		m.push_back(std::make_pair(mDistCache.GetDist(unit.pos, mParser.OwnHatchery.pos), unit.id));
	}
	std::sort(ALL(ownDistanceMap));
	std::sort(ALL(enemyDistanceMap));

	for (auto& enemy : enemyDistanceMap)
	{
		if (ownDistanceMap.empty())
			return; // no more defenders

		auto &candidate = ownDistanceMap.front();
		if (enemy.first > candidate.first)
			break;

		mDefenders.push_back(candidate.second);
		ownDistanceMap.erase(ownDistanceMap.begin());
	}
	if (!mDefenders.empty())
	{
		mDangerUnit = mParser.GetUnit(enemyDistanceMap.front().second);
		std::cout << "Selecting enemy unit " << mDangerUnit.id << " as defender target" << std::endl;
		if (!ownDistanceMap.empty())
		{
			mDefenders.push_back(ownDistanceMap.front().second);
		}
	}
	else
	{
		std::cout << "No more defender targets" << std::endl;
	}
}

void MYCLIENT::DetermineStrategies()
{
	FindDefenders();
	for (auto &unit : mParser.Units)
	{
		if (unit.side != 0)
			continue;

		if (IsAttackingHatchery(unit))
		{
			// no change here
		}
		if (ShouldDefend(unit))
		{
			SetStrategy<DEFENDER>(unit.id);
		}
		else if (ShouldFlee(unit))
		{
			SetStrategy<GUARDIAN>(unit.id);
		}
		else if (ShouldAttack(unit))
		{
			SetStrategy<ATTACKER>(unit.id); // not really needed right now
		}
		else if (ShouldCreep(unit))
		{
			SetStrategy<CREEPER>(unit.id);
		}
		else
		{
			SetStrategy<GUARDIAN>(unit.id);
		}
	}

	SelectAttackGroup();
}

void MYCLIENT::CollectGuardians()
{
	mGuardians.clear();
	for (auto &strategy : mStrategies)
	{
		if (IsStrategy<GUARDIAN>(strategy.second.get()))
		{
			mGuardians.push_back(strategy.first);
		}
	}
	std::sort(ALL(mGuardians));
}

bool MYCLIENT::IsAttackingHatchery(MAP_OBJECT &unit)
{
	return IsStrategy<ATTACKER>(unit.id) && mUnitTarget[unit.id].target_id == mParser.EnemyHatchery.id;
}

bool MYCLIENT::ShouldDefend(MAP_OBJECT &unit)
{
	return contains(mDefenders, unit.id);
}

bool MYCLIENT::ShouldAttack(MAP_OBJECT& unit)
{
	return IsStrategy<ATTACKER>(unit.id);
}

bool MYCLIENT::ShouldCreep(MAP_OBJECT &unit)
{
	return unit.energy >= QUEEN_BUILD_CREEP_TUMOR_COST;
}

bool MYCLIENT::ShouldFlee(MAP_OBJECT &unit)
{
	return mParser.GetAt(unit.pos) != PARSER::CREEP && unit.hp <= QUEEN_FLEE_HP && unit.hp >= mFleePath.GetDamageOnEnemyCreep(unit.pos);
}

void MYCLIENT::FillPrevTickMap()
{
	mPrevTick.clear();
	for (auto &unit : mParser.Units)
	{
		mPrevTick[unit.id] = unit;
	}
}

void MYCLIENT::SelectAttackGroup()
{
	typedef MAP_OBJECT* CANDIDATE;

	std::vector<CANDIDATE> Candidates;
	for (MAP_OBJECT& unit : mParser.Units)
		if (unit.side==0 && IsStrategy<GUARDIAN>(unit.id) && unit.hp == QUEEN_MAX_HP)
			Candidates.push_back(&unit);

	const int MinNumber = 4;
	const int Radius = 1;
	std::set<CANDIDATE> SelectedCandidates;
	
	for (CANDIDATE Center : Candidates)
	{
		std::vector<CANDIDATE> NearCandidates;
		std::copy_if(
			ALL(Candidates),
			std::back_inserter(NearCandidates),
			[&](CANDIDATE Candidate) { return (Candidate->pos - Center->pos).GetManhattan() <= Radius; }
		);
		if (NearCandidates.size() >= MinNumber)
		{
			for (CANDIDATE Candidate : NearCandidates)
				SelectedCandidates.insert(Candidate);
		}
	}

	for (CANDIDATE Candidate : SelectedCandidates)
	{
		SetStrategy<ATTACKER>(Candidate->id);
	}
}

void MYCLIENT::CollectFutureTumor()
{
	static const double fFutureTumor = 0.5;
	static const double fEnemyQueen = 0.2;
	POS iMaxBorderCreep;
	int iMaxBorderCreepDistance = -1;
	for (const auto& iBorderCreep : mBorderCreep)
	{
		int iBorderCreepDistance = 0;
		iBorderCreepDistance += (mParser.OwnHatchery.pos - iBorderCreep.first).GetManhattan();
		// Account for all built creep tumors
		for (const auto& iCreepTumor : mParser.CreepTumors)
		{
			iBorderCreepDistance += (iCreepTumor.pos - iBorderCreep.first).GetManhattan();
		}
		// Account for already assigned tumors (not built yet)
		for (const auto& strategy : mStrategies)
		{
			CREEPER* creeper = dynamic_cast<CREEPER*>(strategy.second.get());
			if (creeper != nullptr && creeper->mFutureTumor.IsValid())
			{
				iBorderCreepDistance += static_cast<int>(fFutureTumor * (creeper->mFutureTumor - iBorderCreep.first).GetManhattan());
			}
		}
		// Account for future tumors (might be built in the future)
		for (const auto& iFutureTumor : mFutureTumors)
		{
			iBorderCreepDistance += static_cast<int>(fFutureTumor * (iFutureTumor - iBorderCreep.first).GetManhattan());
		}
		// Avoid enemy queens
		for (const auto &unit : mParser.Units)
		{
			if (unit.side == 1)
			{
				iBorderCreepDistance += static_cast<int>(fEnemyQueen * (unit.pos - iBorderCreep.first).GetManhattan());
			}
		}

		if (iBorderCreepDistance > iMaxBorderCreepDistance)
		{
			iMaxBorderCreepDistance = iBorderCreepDistance;
			iMaxBorderCreep = iBorderCreep.first;
		}
	}
	if (iMaxBorderCreep.IsValid())
		mFutureTumors.push_back(iMaxBorderCreep);
}

void MYCLIENT::CollectFutureTumors()
{
	mFutureTumors.clear();
	mFutureTumors.reserve(MAX_QUEENS);
	for (int i = 0; i < MAX_QUEENS; ++i)
	{
		CollectFutureTumor();
	}
}

CLIENT *CreateClient()
{
	return new MYCLIENT();
}
