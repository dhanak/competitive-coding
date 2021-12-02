#pragma once
static const int BASE_MAX_HP = 1000;
static const int TURRET_MAX_HP = 50;
static const int HERO_MAX_HP = 80;
static const int HERO_DAMAGE = 5;
static const int HERO_RANGE_SQ = 13;

struct POS
{
	int x, y;
	POS() { x = y = 0; }
	POS(int _x, int _y) { x = _x; y = _y; }
	bool operator== (POS const &rhs) const
	{
		return x == rhs.x && y == rhs.y;
	}
	bool operator!= (POS const &rhs) const
	{
		return x != rhs.x || y != rhs.y;
	}
	bool operator< (POS const &rhs) const
	{
		if (y != rhs.y)
			return y<rhs.y;
		return x<rhs.x;
	}
	bool IsValid() const
	{
		return x != 0;
	}
	int DistSquare(POS const &rhs) const
	{
		return (x - rhs.x)*(x - rhs.x) + (y - rhs.y)*(y - rhs.y);
	}
};
struct ITEM_INFO {
	POS pos;
	enum eItemType {
		NO_ITEM,
		ARMOR,
		CROSSBOW,
		HAMMER,
		POTION,
		BOMB,
	};
	eItemType t;
	int side;
};
struct HERO_INFO {
	int id, hp, side;
	POS pos;
	ITEM_INFO::eItemType held_item;
	int held_item_side;
	int controller_id; // 0 if you are the controller. 100 if it's manually controlled with the viewer in test mode. team_id otherwise.
	int respawn_timer; // 0 if alive
	int score;
};
struct TURRET_INFO {
	int id, hp, side; // side: 0 yours, 1 enemy, 2 neutral
	POS pos;
};

struct ACTION_INFO {
	enum eActionType
	{
		ATTACK,
		BLAST,
		HEAL,
		REPAIR
	};
	int attacker_id;
	POS attacker_pos;
	int target_id;
	POS target_pos;
	eActionType action_type;
};
struct TEAM_INFO
{
	int team_id;
	int match_wins;
	int elo_points;
	int qualified;
	std::string name;
};

class PARSER
{
public:
	PARSER();
	// received after connecting:
	int w, h;
	std::vector<unsigned char> ArenaFree; // 0: wall, 1: free, 2: base
	std::vector<TEAM_INFO> Teams;

	// received each tick:
	int tick;
	int match_id;
	enum eMatchType
	{
		MELEE,
		DUEL
	};
	eMatchType match_type;
	int base_health[2]; // your and enemy base remaining health
	
	std::vector<HERO_INFO> Heroes;
	std::vector<ITEM_INFO> Items;
	std::vector<ACTION_INFO> Actions;
	std::vector<TURRET_INFO> Turrets;

	bool IsFree(const POS &p) const { return p.x<w && p.y<h ? ArenaFree[p.x + p.y*w]!=0 : false; }
	bool IsBaseGround(const POS &p) const { return p.x<w && p.y<h ? ArenaFree[p.x + p.y*w] == 2 : false; }
	HERO_INFO *GetHeroByID(int id);
	TURRET_INFO *GetTurretByID(int id);
	TEAM_INFO *GetTeamByID(int team_id);
	enum eMatchResult {
		ONGOING,
		VICTORY,
		DRAW,
		DEFEAT
	};
	eMatchResult match_result;
	void Parse(const std::vector<std::string> &ServerResponse);
	void ParseMap(const std::vector<std::string> &ServerResponse);
	void ParseTeams(const std::vector<std::string> &ServerResponse);
};
