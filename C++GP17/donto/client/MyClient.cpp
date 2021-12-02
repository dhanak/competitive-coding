#include "stdafx.h"
#include "Client.h"
#include "parser.h"
#include "Champion.h"

// sample

std::string GetPasswordCustom();

class MYCLIENT : public CLIENT
{
public:
	MYCLIENT();
protected:
	virtual std::string GetPassword()
	{
		return GetPasswordCustom(); 
	}

	virtual std::string GetPreferredOpponents() { return std::string("melee"); }
	virtual bool NeedDebugLog() { return true; }
	virtual void Process();

	CLIENT_INFO mInfo;

	typedef tuple<int, int, int, int> STAT;
	STAT mLastStat;
};

MYCLIENT::MYCLIENT()
{
}

void MYCLIENT::Process()
{
	mInfo.mFriendlyTurretsPerZone.resize(ZONE_COUNT);
	for (auto &obj: mParser.Units)
	{
		if (obj.side==0 && obj.t==MINION)
		{
			for (int i = -1; i<=1; i++)
				for (int j = -1; j<=1; j++)
					mInfo.mMinionsWereHere.insert(POS(obj.pos.x+i, obj.pos.y+j));
		}
		if (obj.side==0 && obj.t==TURRET)
		{
			mInfo.mFriendlyTurretsPerZone[Zones[obj.pos]]++;
		}
	}

	for (auto &ctrl: mParser.Controllers)
	{
		if (ctrl.controller_id == 0)
		{
			MAP_OBJECT *pHero = mParser.GetUnitByID(ctrl.hero_id);
			if (pHero == NULL) continue; // respawning

			CHAMPION Champion(mParser, mDistCache, mInfo, *pHero);
			std::unique_ptr<ACTION> Action(Champion.Process());
			if (Action.get())
				Action->Execute(this, pHero->id);
		}
	}

	STAT stat{ mParser.GetUnitByID(11)->hp, mParser.level[0], mParser.GetUnitByID(12)->hp, mParser.level[1] };
	if (stat != mLastStat)
		printf("TICK: %d\tUS: %d/%d\tTHEM: %d/%d\n", mParser.tick, get<0>(stat), get<1>(stat), get<2>(stat), get<3>(stat));
	mLastStat = stat;
}


CLIENT *CreateClient()
{
	return new MYCLIENT();
}
