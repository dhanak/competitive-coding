#pragma once

static const int HATCHERY_BUILD_QUEEN_COST = 24000;
static const int HATCHERY_MAX_ENERGY = 80000;
static const int HATCHERY_MAX_HP = 1500;
static const int QUEEN_BUILD_CREEP_TUMOR_COST = 100;
static const int QUEEN_MAX_ENERGY = 200;
static const int QUEEN_MAX_HP = 175;
static const int QUEEN_FLEE_HP = 60;
static const int CREEP_TUMOR_SPAWN_ENERGY = 60;
static const int CREEP_TUMOR_MAX_HP = 200;
static const int QUEEN_DAMAGE = 40;
static const int HP_DECAY_ON_ENEMY_CREEP = 2;
static const int HP_REGEN_ON_FRIENDLY_CREEP = 2;
static const int ENERGY_REGEN = 1;
static const int MAX_QUEENS = 8;
static const int HATCHERY_SIZE = 3;
static const int MAX_TICK = 1200;

struct POS
{
	enum eDirection
	{
		SHIFT_UP = 0,
		SHIFT_RIGHT = 1,
		SHIFT_DOWN = 2,
		SHIFT_LEFT = 3
	};
	int x, y;
	POS() { x=y=0; }
	POS(int _x, int _y) { x=_x; y=_y;}

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

	bool IsNear(POS const &rhs) const
	{
		return abs(x-rhs.x)+abs(y-rhs.y)<=1;
	}

	bool IsValid() const { return x!=0; }

	int GetManhattan() const
	{
		return abs(x)+abs(y);
	}

	POS ShiftXY(int dx, int dy) const { return POS(x+dx, y+dy); }

	POS ShiftDir(int dir) const
	{
		switch (dir)
		{
		case SHIFT_UP: return POS(x,y-1);
		case SHIFT_RIGHT: return POS(x+1,y);
		case SHIFT_DOWN: return POS(x,y+1);
		default: return POS(x-1,y);
		}
	}

	POS ShiftDirStep(int dir, int step) const
	{
		switch (dir)
		{
		case SHIFT_UP: return POS(x,y-step);
		case SHIFT_RIGHT: return POS(x+step,y);
		case SHIFT_DOWN: return POS(x,y+step);
		default: return POS(x-step,y);
		}
	}

	POS operator+ (POS const &rhs) const { return ShiftXY( rhs.x,  rhs.y); }
	POS operator- (POS const &rhs) const { return ShiftXY(-rhs.x, -rhs.y); }
	int dist2sq(POS const &o) const
	{
		int dx = o.x - x;
		int dy = o.y - y;
		int dx_q1 = 2*dx+(0<dx?1:-1);
		int dy_q1 = 2*dy+(0<dy?1:-1);
		int d2_q2 = dx_q1*dx_q1+dy_q1*dy_q1;
		return d2_q2;
	}
};

std::ostream &operator<<(std::ostream& o, POS p);

struct MAP_OBJECT {
	int id = -1;
	int hp = 0;
	int energy = 0;
	int side = -1;
	POS pos;
};

class PARSER
{
public:
	PARSER();
	int tick;
	int versus[2]; // your ID and enemy ID. versus[1]==0 if not real opponent (test)
	enum eGroundType
	{
		EMPTY,
		WALL,
		CREEP,
		CREEP_CANDIDATE_FRIENDLY,
		CREEP_CANDIDATE_ENEMY,
		CREEP_CANDIDATE_BOTH,
		ENEMY_CREEP,

		GROUND_TYPE_COUNT
	};
	int w, h;
	std::vector<eGroundType> Arena;
	std::vector<MAP_OBJECT> Units;
	MAP_OBJECT OwnHatchery;
	MAP_OBJECT EnemyHatchery;
	std::vector<MAP_OBJECT> CreepTumors;

	eGroundType GetAt(const POS &p) const { return p.x >= 0 && p.y >= 0 && p.x<w && p.y<h?Arena[p.x+p.y*w]:WALL; }
	bool UnitExists(int id) const
	{
		return std::find_if(Units.begin(), Units.end(), [id](const MAP_OBJECT& unit) {return unit.id==id; }) != Units.end();
	}
	const MAP_OBJECT& GetUnit(int id) const
	{
		auto it = std::find_if(Units.begin(), Units.end(), [id](const MAP_OBJECT& unit) {return unit.id==id; });
		assert(it != Units.end());
		return *it;
	}
	void ParseUnits(const std::vector<std::string> &ServerResponse, int &index, int count, std::vector<MAP_OBJECT> &container);
	enum eMatchResult {
		ONGOING,
		VICTORY,
		DRAW,
		DEFEAT
	};
	eMatchResult match_result;
	void Parse(const std::vector<std::string> &ServerResponse);
};
