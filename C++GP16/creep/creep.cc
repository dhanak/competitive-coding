#define COLOR_NONE
#define FONT_ASCII
#define FAST_FWD 1e6

#include <cassert>
#include <cmath>
#include <cstdio>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
// bal alsó sarok 0,0, jobbra x, felfelé y tengely
int const map_max_dx=64;
int const map_max_dy=64;
// egy tick hossza, 1/256s-ben
int const dt_tick_q8=0.25*256; // 64
// ilyen tempóban regenerálja az energiáját egy unit, 1/256 energy/s egységben
int const energy_regeneration_q8=0.5*256; // 128
// ilyen tempóban regenerálja az életét egy unit, 1/256 hp/s egységben
int const health_regeneration_q8=0.25*256; // 64

// cellák rajzolásához
// színek
#if defined COLOR_ANSI
#	define COLOR_DEFAULT "\x1b[0m"
#	define COLOR_GREEN "\x1b[32m"
#	define COLOR_RED "\x1b[31m"
#	define COLOR_YELLOW "\x1b[33m"
#	define COLOR_BLUE "\x1b[34m"
#	define COLOR_MAGENTA "\x1b[35m"
#	define COLOR_LIGHT_MAGENTA "\x1b[35;1m"
#elif defined COLOR_NONE
#	define COLOR_DEFAULT ""
#	define COLOR_GREEN ""
#	define COLOR_RED ""
#	define COLOR_YELLOW ""
#	define COLOR_BLUE ""
#	define COLOR_MAGENTA ""
#	define COLOR_LIGHT_MAGENTA ""
#else
#	error "define COLOR_ANSI|COLOR_NONE plz"
#endif
char const *color_default=COLOR_DEFAULT;
char const *color_green=COLOR_GREEN;
char const *color_yellow=COLOR_YELLOW;
// karakterek/stringek
// pl terminal 12pt
#if defined FONT_DOS
char const *print_wall=COLOR_DEFAULT "Ű"; // Ű˛±°
char const *print_creep=COLOR_MAGENTA "˛";
char const *print_creep_candidate=COLOR_YELLOW "±";
char const *print_creep_radius=COLOR_YELLOW "°";
char const *print_empty=COLOR_DEFAULT " ";
char const *print_hatchery=COLOR_LIGHT_MAGENTA "Ű" COLOR_DEFAULT;
char const *print_creep_tumor_cooldown=COLOR_YELLOW "Ű";
char const *print_creep_tumor_active=COLOR_GREEN "Ű";
char const *print_creep_tumor_inactive=COLOR_LIGHT_MAGENTA "Ű" COLOR_DEFAULT;
// csak ascii karakterek
#elif defined FONT_ASCII
char const *print_wall=COLOR_DEFAULT "#";
char const *print_creep=COLOR_MAGENTA "C";
char const *print_creep_candidate=COLOR_YELLOW "+";
char const *print_creep_radius=COLOR_YELLOW ".";
char const *print_empty=COLOR_DEFAULT " ";
char const *print_hatchery=COLOR_LIGHT_MAGENTA "H" COLOR_DEFAULT;
char const *print_creep_tumor_cooldown=COLOR_YELLOW "T";
char const *print_creep_tumor_active=COLOR_GREEN "A";
char const *print_creep_tumor_inactive=COLOR_LIGHT_MAGENTA "*" COLOR_DEFAULT;
// unicode font, pl lucida console 10pt
#elif defined FONT_UNICODE
char const *print_wall=COLOR_DEFAULT "#";
char const *print_creep=COLOR_MAGENTA "C";
char const *print_creep_candidate=COLOR_YELLOW "•"; // "•·█▓▒░"
char const *print_creep_radius=COLOR_YELLOW  "·";
char const *print_empty=COLOR_DEFAULT " ";
char const *print_hatchery=COLOR_LIGHT_MAGENTA "H" COLOR_DEFAULT;
char const *print_creep_tumor_cooldown=COLOR_YELLOW "T";
char const *print_creep_tumor_active=COLOR_GREEN "A";
char const *print_creep_tumor_inactive=COLOR_LIGHT_MAGENTA "*" COLOR_DEFAULT;
#else
#	error "define FONT_DOS|FONT_ASCII|FONT_UNICODE plz"
#endif

struct pos
{
	pos(): x(0), y(0) {}
	pos(int _x, int _y): x(_x), y(_y) {}
	static int dot(pos const &a, pos const &b) { return a.x*b.x+a.y*b.y; }
	friend bool operator== (pos const &lhs, pos const &rhs)
	{
		return lhs.x==rhs.x && lhs.y==rhs.y;
	}
	friend bool operator< (pos const &lhs, pos const &rhs)
	{
		if(lhs.y!=rhs.y)
			return lhs.y<rhs.y;
		return lhs.x<rhs.x;
	}
	int x,y;
};

struct bound_rect
{
	bound_rect(): x0(0), y0(0), x1(0), y1(0) {}
	bound_rect(int _x0, int _y0, int _x1, int _y1): x0(_x0), y0(_y0), x1(_x1), y1(_y1) {}
	int x0,y0,x1,y1;
};

struct game;
struct object;
struct building;
struct hatchery;
struct creep_tumor;
struct unit;
struct queen;

struct object
{
	object():
		id(0), br(),
		max_health_q8(0), health_q8(0),
		max_energy_q8(0), energy_q8(0),
		creep_spread_radius(0)
	{}
	virtual ~object() {}
	virtual void tick()
	{
		health_q8=std::min(max_health_q8,
			health_q8+health_regeneration_q8*dt_tick_q8/256);
		energy_q8=std::min(max_energy_q8,
			energy_q8+energy_regeneration_q8*dt_tick_q8/256);
	}
	virtual char const *name() const =0;
	virtual char const *map_cell_code() const { return "?"; }
	virtual void dump_available_abilities(std::ostream &o) const {}

	int id;
	bound_rect br;
	int max_health_q8;
	int health_q8;
	int max_energy_q8;
	int energy_q8;
	int creep_spread_radius;
};

struct building: public object
{
	building(pos const &p0, int dx, int dy, int max_health)
	{
		br.x0=p0.x;
		br.y0=p0.y;
		br.x1=br.x0+dx;
		br.y1=br.y0+dy;
		max_health_q8=max_health*256;
		health_q8=max_health_q8;
	}
	virtual ~building() {}
	virtual creep_tumor& a_creep_tumor_i_suppose() { assert(0 && "not a creep tumor"); }
};

struct hatchery: public building
{
	hatchery(pos const &p0): building(p0,3,3,1500)
	{
		creep_spread_radius=10;
	}
	virtual ~hatchery() {}
	virtual char const *name() const { return "hatchery"; }
	virtual char const *map_cell_code() const { return print_hatchery; }
};

struct creep_tumor: public building
{
	creep_tumor(pos const &p0): building(p0,1,1,200)
	{
		creep_spread_radius=10;
		spawn_creep_tumor_active=1;
		dt_spawn_creep_tumor_cooldown_q8=15.0*256;
	}
	virtual ~creep_tumor() {}
	virtual void tick()
	{
		building::tick();
		dt_spawn_creep_tumor_cooldown_q8=
			std::max(0,dt_spawn_creep_tumor_cooldown_q8-dt_tick_q8);
	}
	virtual char const *name() const { return "creep_tumor"; }
	virtual char const *map_cell_code() const
	{
		if(spawn_creep_tumor_active)
			return dt_spawn_creep_tumor_cooldown_q8?print_creep_tumor_cooldown:print_creep_tumor_active;
		else return print_creep_tumor_inactive;
	}
	virtual void dump_available_abilities(std::ostream &o) const
	{
		if(!spawn_creep_tumor_active)
			o << ",inactive";
		else if(0<dt_spawn_creep_tumor_cooldown_q8)
			o << "," << color_yellow << "on_cooldown,dt=("
				<< dt_spawn_creep_tumor_cooldown_q8
				<< "," << std::fixed << std::setprecision(2)
				<< dt_spawn_creep_tumor_cooldown_q8/256.0 << ")"
				<< color_default;
		else o << "," << color_green << "available" << color_default;
	}
	virtual creep_tumor& a_creep_tumor_i_suppose() { return *this; }

	int spawn_creep_tumor_active;
	int dt_spawn_creep_tumor_cooldown_q8;
	static int const spawn_creep_tumor_radius=10;
};

struct unit: public object
{
	unit(int max_health)
	{
		max_health_q8=max_health*256;
		health_q8=max_health_q8;
	}
	virtual ~unit() {}
	virtual queen& a_queen_i_suppose() { assert(0 && "not a queen"); }
};

struct queen: public unit
{
	queen(): unit(175)
	{
		max_energy_q8=200.0*256;
		energy_q8=25.0*256;
	}
	virtual ~queen() {}
	virtual char const *name() const { return "queen"; }
	virtual void dump_available_abilities(std::ostream &o) const
	{
		o << ",";
		if(energy_q8<queen::spawn_creep_tumor_energy_cost_q8)
			o << color_yellow << "needs_25_energy" << color_default;
		else o << color_green << "available" << color_default;
	}
	virtual queen& a_queen_i_suppose() { return *this; }

	static int const dt_build_time_q8=int(60.0*256);
	static int const spawn_creep_tumor_energy_cost_q8=int(25.0*256);
};

struct game
{
	game(char const *map_file_name):
		next_id(0), t_q2(0), t_limit_q2(0), p_base(),
		buildings(), units(),
		map_dx(), map_dy(),
		map_creep_gen(), map_creep(), creep_cover(0),
		map_wall(), map_building()
	{
		FILE *f=fopen(map_file_name,"rt");
		assert(f && "can't open map file");
		bool ok=fscanf(f,"%d",&t_limit_q2)==1
			&& 10<=t_limit_q2 && t_limit_q2<=100000;
		assert(ok && "map time limit");
		ok=fscanf(f,"%d%d",&map_dx,&map_dy)==2
			&& 16<=map_dx && map_dx<=map_max_dx
			&& 16<=map_dy && map_dy<=map_max_dy;
		assert(ok && "map dimensions");

		int ne=0;
		for(int y=map_dy-1; ok && 0<=y; --y)
		{
			char s[129];
			ok=fscanf(f,"%128s",s)==1 && map_dx<=128 && s[map_dx]==0;
			for(int x=0; ok && x<map_dx; ++x)
			{
				if(s[x]=='#')
					map_wall[y][x]=1;
				else
				{
					ok=s[x]=='.';
					++ne;
				}
			}
		}
		for(uint y=0; ok && y<map_dy; ++y)
			ok=map_wall[y][0] && map_wall[y][map_dx-1];
		for(uint x=0; ok && x<map_dx; ++x)
			ok=map_wall[0][x] && map_wall[map_dy-1][x];
		assert(ok && "map file format");

		ok=fscanf(f,"%d%d",&p_base.x,&p_base.y)==2
			&& 0<=p_base.x && p_base.x+5<=map_dx
			&& 0<=p_base.y && p_base.y+5<=map_dy;
		assert(ok && "map hatch position");
		fclose(f);

		{
			bool v[map_max_dy][map_max_dx]={};
			std::vector<pos> q(1,p_base);
			int n=0;
			while(!q.empty())
			{
				pos p=q.back();
				q.pop_back();
				if(map_wall[p.y][p.x] || v[p.y][p.x])
					continue;
				v[p.y][p.x]=1;
				++n;
				for(int d=0; d<4; ++d)
					q.push_back(pos(p.x+d%2*(2-d),p.y+(d-1)*(d+1)%2));
			}
			assert(n==ne && "connected space plz");
		}

		add_building(new hatchery(p_base)); // id=1
		add_unit(new queen()); // id=2
		all_creep_now_plz();
	}
	~game()
	{
		for(auto p: units)
			delete p;
		for(auto p: buildings)
			delete p;
	}
	bool valid_pos(pos const &p) const
	{
		return 0<=p.x && p.x<map_dx && 0<=p.y && p.y<map_dy;
	}
	// p0 pozíciójú cella közepe köré rajzolt radius sugarú körön belüli cellák
	void valid_cells_in_a_radius(std::vector<pos> &cells,
		pos const &p0, int radius) const
	{
		for(int dy=-radius+1; dy<radius; ++dy)
			for(int dx=-radius+1; dx<radius; ++dx)
			{
				pos p(p0.x+dx,p0.y+dy);
				if(!valid_pos(p))
					continue;
				int dx_q1=2*dx+(0<dx?1:-1);
				int dy_q1=2*dy+(0<dy?1:-1);
				int d2_q2=dx_q1*dx_q1+dy_q1*dy_q1;
				if(d2_q2<=radius*radius*4)
					cells.push_back(p);
			}
	}
	// ha ez a cella szabad és nincs rajta creep,
	// a szomszédban valahol van creep, akkor ide terjeszkedhet
	// előfeltétel hogy a cella egy generátor területén belül legyen
	bool creep_spread_candidate(pos const &p) const
	{
		return !map_wall[p.y][p.x] && !map_building[p.y][p.x]
			&& !map_creep[p.y][p.x]
			&& (0<p.x && map_creep[p.y][p.x-1]
				|| p.x+1<map_dx && map_creep[p.y][p.x+1]
				|| 0<p.y && map_creep[p.y-1][p.x]
				|| p.y+1<map_dy && map_creep[p.y+1][p.x]);
	}
	bool creep_spread_candidates(std::vector<pos> &candidates,
		std::vector<pos> const &cells) const
	{
		for(auto p: cells)
			if(creep_spread_candidate(p))
				candidates.push_back(p);
		return !candidates.empty();
	}
	bool creep_cells(std::vector<pos> &cells, object const &o) const
	{
		if(0<o.creep_spread_radius)
		{
			pos p0((o.br.x0+o.br.x1)/2,(o.br.y0+o.br.y1)/2);
			valid_cells_in_a_radius(cells,p0,o.creep_spread_radius);
			return true;
		}
		return false;
	}
	void spread_creep()
	{
		for(auto pb: buildings)
		{
			building const &b=*pb;
			std::vector<pos> all;
			std::vector<pos> wave;
			if(creep_cells(all,b) && creep_spread_candidates(wave,all))
			{
				uint k=(t_q2*t_q2+37)%wave.size();
				pos nc=wave[k];
				map_creep[nc.y][nc.x]=1;
				++creep_cover;
			}
		}
	}
	bool anything_to_do() const
	{
		for(int y=0; y<map_dy; ++y)
			for(int x=0; x<map_dx; ++x)
				if(map_creep_gen[y][x] && creep_spread_candidate(pos(x,y)))
					return true;
		return false;
	}
	void all_creep_now_plz()
	{
		bool go;
		do
		{
			go=0;
			for(auto pb: buildings)
			{
				building const &b=*pb;
				std::vector<pos> all;
				std::vector<pos> wave;
				if(creep_cells(all,b) && creep_spread_candidates(wave,all))
					for(auto p: wave)
					{
						map_creep[p.y][p.x]=1;
						++creep_cover;
						go=1;
					}
			}
		}
		while(go);
	}
	void tick()
	{
		spread_creep();
		for(auto p: buildings)
			(*p).tick();
		for(auto p: units)
			(*p).tick();
		++t_q2;
		if(t_q2*dt_tick_q8%queen::dt_build_time_q8==0)
			add_unit(new queen());
	}
	void add_building(building *p)
	{
		buildings.push_back(p);
		building &b=*p;
		b.id=++next_id;
		for(int y=b.br.y0; y<b.br.y1; ++y)
			for(int x=b.br.x0; x<b.br.x1; ++x)
			{
				assert(!map_building[y][x] && "a building there");
				assert(!map_wall[y][x] && "a wall there");
				map_building[y][x]=p;
				if(0<b.creep_spread_radius && !map_creep[y][x])
				{
					map_creep[y][x]=1;
					++creep_cover;
				}
			}
		std::vector<pos> cells;
		if(creep_cells(cells,b))
			for(auto p: cells)
				++map_creep_gen[p.y][p.x];
	}
	void add_unit(unit *p)
	{
		units.push_back(p);
		unit &u=*p;
		u.id=++next_id;
	}
	queen& get_queen(int id)
	{
		for(auto p: units)
			if((*p).id==id)
				return (*p).a_queen_i_suppose();
		assert(0 && "invalid id");
	}
	creep_tumor& get_creep_tumor(int id)
	{
		for(auto p: buildings)
			if((*p).id==id)
				return (*p).a_creep_tumor_i_suppose();
		assert(0 && "invalid id");
	}
	void queen_spawn_creep_tumor(queen &u, pos const &p)
	{
		assert(queen::spawn_creep_tumor_energy_cost_q8<=u.energy_q8 && "not enough energy");
		assert(valid_pos(p) && !map_wall[p.y][p.x] && !map_building[p.y][p.x]
			&& map_creep[p.y][p.x] && "not on creep");
		u.energy_q8-=queen::spawn_creep_tumor_energy_cost_q8;
		add_building(new creep_tumor(p));
	}
	void creep_tumor_spawn_creep_tumor(creep_tumor &b, pos const &p)
	{
	    //if (b.spawn_creep_tumor_active!=1) {
	    {
		std::ostream& o = std::cerr;
		o << "id=" << b.id << "," << b.name()
		  << ",p=(" << b.br.x0 << "," << b.br.y0 << ")";
		b.dump_available_abilities(o);
		o << '\n';
	    }
		assert(b.spawn_creep_tumor_active==1 && "creep tumor not active");
		assert(b.dt_spawn_creep_tumor_cooldown_q8==0 && "spawn creep tumor cooldown");
		assert(valid_pos(p) && !map_wall[p.y][p.x] && !map_building[p.y][p.x]
			&& map_creep[p.y][p.x] && "not on creep");
		std::vector<pos> cells;
		pos p0(b.br.x0,b.br.y0);
		valid_cells_in_a_radius(cells,p0,creep_tumor::spawn_creep_tumor_radius);
		bool ok=0;
		for(uint i=0; !ok && i<cells.size(); ++i)
			ok=cells[i]==p;
		assert(ok && "target too far");
		b.spawn_creep_tumor_active=0;
		add_building(new creep_tumor(p));
	}
	friend std::ostream &operator<< (std::ostream &o, game const &g)
	{
		o << "t=(" << g.t_q2
			<< "," << std::fixed << std::setprecision(2) << g.t_q2/4.0 << ")\n";
		o << "buildings[" << g.buildings.size() << "]=\n";
		for(auto p: g.buildings)
		{
			building const &b=*p;
			o << "id=" << b.id << "," << b.name()
				<< ",p=(" << b.br.x0 << "," << b.br.y0 << ")";
			b.dump_available_abilities(o);
			o << "\n";
		}
		o << "units[" << g.units.size() << "]=\n";
		for(auto p: g.units)
		{
			unit const &u=*p;
			o << "id=" << u.id << "," << u.name();
			if(0<u.max_energy_q8)
				o << ",energy=(" << u.energy_q8
					<< "," << std::fixed << std::setprecision(2) << u.energy_q8/256.0 << ")";
			u.dump_available_abilities(o);
			o << "\n";
		}
		o << "map=\n";
		o << "   ";
		for(uint x=0; x<g.map_dx; ++x)
			o << x%10;
		o << "\n";
		for(uint y=g.map_dy; y--;)
		{
		    o << std::setw(2) << y << ' ';
			for(uint x=0; x<g.map_dx; ++x)
			{
				if(g.map_wall[y][x])
					o << print_wall;
				else if(g.map_building[y][x])
					o << (*g.map_building[y][x]).map_cell_code();
				else if(g.map_creep[y][x])
					o << print_creep;
				else if(0<g.map_creep_gen[y][x])
				{
					if(g.creep_spread_candidate(pos(x,y)))
						o << print_creep_candidate;
					else o << print_creep_radius;
				}
				else o << print_empty;
			}
			o << color_default << ' ' << y << "\n";
		}
		o << "   ";
		for(uint x=0; x<g.map_dx; ++x)
			o << x%10;
		o << "\n";
		o << "creep_cover=" << g.creep_cover << std::endl;
		return o;
	}
	int next_id;
	int t_q2;
	int t_limit_q2;
	pos p_base;
	std::vector<building*> buildings;
	std::vector<unit*> units;
	int map_dx,map_dy;
	int map_creep_gen[map_max_dy][map_max_dx];
	bool map_creep[map_max_dy][map_max_dx];
	int creep_cover;
	bool map_wall[map_max_dy][map_max_dx];
	building* map_building[map_max_dy][map_max_dx];
};

void step(game &g, bool turbo)
{
	g.tick();
	//std::cerr << "t=" << g.t_q2 << "\n";
	std::cout << g;
	double speed=
#if defined SPEED
		SPEED;
#else
		1;
#endif
	// 1e6: normál sebesség
	int d=
#if defined FAST_FWD
		turbo?1e6/FAST_FWD:1e6/speed;
#else
		1e6/speed;
#endif
	usleep(d*dt_tick_q8/256);
}

int main(int argc, char **argv)
{
	assert(argc==2 && "./creep map < in");
	game g(argv[1]);
	std::cout << g;
	int N=0;
	scanf("%d",&N);
	for(int n=0; n<N && g.t_q2<g.t_limit_q2; ++n)
	{
		int t,cmd,id,x,y;
		scanf("%d%d%d%d%d",&t,&cmd,&id,&x,&y);
		assert(g.t_q2<=t && "can't go back in time sry");
		while(g.t_q2<t && g.t_q2<g.t_limit_q2)
			step(g,true);
		if(cmd==1)
			g.queen_spawn_creep_tumor(g.get_queen(id),pos(x,y));
		else if(cmd==2)
			g.creep_tumor_spawn_creep_tumor(g.get_creep_tumor(id),pos(x,y));
		else assert(0 && "invalid cmd code");
	}
	while(g.anything_to_do() && g.t_q2<g.t_limit_q2)
		step(g,true);
	return 0;
}
