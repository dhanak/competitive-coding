#pragma once

class MYCLIENT;
class STRATEGY
{
public:
	enum ATTACK_FLAGS
	{
		SICKER = 1,
		HEALTHIER = 2,
		EVERYONE = SICKER | HEALTHIER,

		ON_HOME_GROUND = 4,
		ON_NEUTRAL_GROUND = 8,
		ON_ENEMY_GROUND = 16,
		EVERYWHERE = ON_HOME_GROUND | ON_NEUTRAL_GROUND | ON_ENEMY_GROUND,

		ALL = EVERYONE | EVERYWHERE 
	};


	STRATEGY(MYCLIENT *client, int id) : mClient(client), mUnitId(id) {};
	virtual ~STRATEGY() {};

	virtual const char *GetName() const = 0;
	virtual void DoIt() = 0;

	virtual bool AttackIfNearby(int attack_flags = ALL) final;
	const MAP_OBJECT &Myself() const { return mClient->mParser.GetUnit(mUnitId); }

protected:
	MYCLIENT *mClient;
	int mUnitId;
};

class CREEPER : public STRATEGY
{
public:
	POS mFutureTumor;
	bool mRealFuture = false;
	CREEPER(MYCLIENT *client, int id) : STRATEGY(client, id) {}

	const char *GetName() const { return "creeper"; }
	void DoIt();
	POS GetFarthestBorderCreep();
	FUTURE_TUMOR_TYPE::iterator GetNearestFutureTumor();
	POS GetNearestSafeEmptyPos();
};

class ATTACKER : public STRATEGY
{
	bool CanAttackHatchery(POS p);
public:
	ATTACKER(MYCLIENT *client, int id) : STRATEGY(client, id) {}

	const char *GetName() const { return "attacker"; }
	void DoIt();
};

class GUARDIAN : public STRATEGY
{
	bool mOnGuard = false;
public:
	GUARDIAN(MYCLIENT *client, int id) : STRATEGY(client, id) {}

	const char *GetName() const { return "guardian"; }
	void DoIt();
	bool OnGuard();
};

class FLEER : public STRATEGY
{
public:
	FLEER(MYCLIENT *client, int id) : STRATEGY(client, id) {}

	const char *GetName() const { return "fleer"; }
	void DoIt();
};

class DEFENDER : public STRATEGY
{
public:
	DEFENDER(MYCLIENT *client, int id) : STRATEGY(client, id) {}

	const char *GetName() const { return "defender"; }
	void DoIt();
};
