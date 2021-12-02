#pragma once

#include "Client.h"
#include "fleepath.h"
#include "util.h"

typedef std::vector<POS> FUTURE_TUMOR_TYPE;

class STRATEGY;
class MYCLIENT : public CLIENT
{
public:
	MYCLIENT();

	std::map<int, MAP_OBJECT> mPrevTick;
	FLEEPATH mFleePath;

	struct BORDER_INFO
	{
		bool nearEmpty = false;
		bool nearEnemyCreep = false;
		bool nearWall = false;
		BORDER_INFO() {}
		BORDER_INFO(bool e, bool ec, bool w) : nearEmpty(e), nearEnemyCreep(ec), nearWall(w) {}
	};
	std::map<POS, BORDER_INFO> mBorderCreep;
	FUTURE_TUMOR_TYPE mFutureTumors; // lista elejen a 'legjobb' valaszthato

	MAP_OBJECT mDangerUnit;
	std::map<POS, int> mUnderAttack;
	std::map<POS, int> mOwnTumors;

	int IsUnderAttackBy(POS p) { return contains(mUnderAttack, p) ? mUnderAttack[p] : -1; }
	bool HasTumor(POS p) { return contains(mOwnTumors, p); }

	std::vector<int> mGuardians;
protected:
	virtual std::string GetPassword() { return std::string("qLA56H"); }
	virtual std::string GetPreferredOpponents() { return std::string("any"); }
	virtual bool NeedDebugLog() { return true; }
	virtual void Process();

	void CalculateUnderAttack();
	void FillOwnTumors();
	void RemoveDeadUnits();
	void FindBorderCreeps();
	void SpawnTumors();
	void FindDefenders();
	void DetermineStrategies();
	void CollectGuardians();

	bool IsAttackingHatchery(MAP_OBJECT &unit);
	bool ShouldDefend(MAP_OBJECT &unit);
	bool ShouldCreep(MAP_OBJECT &unit);
	bool ShouldFlee(MAP_OBJECT &unit);
	bool ShouldAttack(MAP_OBJECT& unit);

	void FillPrevTickMap();

	void CollectFutureTumor();
	void CollectFutureTumors();

	template <class MyStrategy>
	bool IsStrategy(STRATEGY* Strategy);

	template <class MyStrategy>
	bool IsStrategy(int id);

	template <class MyStrategy>
	void SetStrategy(int unit_id);
	std::map<int, std::unique_ptr<STRATEGY>> mStrategies;
	std::vector<int> mDefenders;


private:
	void SelectAttackGroup();
};

template <class MyStrategy>
bool MYCLIENT::IsStrategy(STRATEGY* Strategy)
{
	return dynamic_cast<MyStrategy*>(Strategy) == Strategy;
}

template <class MyStrategy>
bool MYCLIENT::IsStrategy(int id)
{
	auto it = mStrategies.find(id);
	if (it == mStrategies.end())
		return false;
	return IsStrategy<MyStrategy>(it->second.get());
}

template <class MyStrategy>
void MYCLIENT::SetStrategy(int unit_id)
{
	if (!IsStrategy<MyStrategy>(unit_id))
	{
		mStrategies[unit_id] = std::make_unique<MyStrategy>(this, unit_id);
		std::cout << "Setting strategy of unit #" << unit_id << " to " << mStrategies[unit_id]->GetName() << std::endl;
	}
}
