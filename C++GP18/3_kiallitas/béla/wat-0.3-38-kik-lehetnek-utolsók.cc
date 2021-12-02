#include <cassert>
#include <cstdio>
#include <cstring>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <iostream>
#include <chrono>
typedef unsigned int uint;
typedef unsigned long long int ull;
int Y=0,X=0;
int const max_N=14;
char map_in[max_N][max_N+1];

auto t0=std::chrono::steady_clock::now();
double tl=5.0;

struct pos
{
	void operator+= (pos const &d)
	{
		x+=d.x;
		y+=d.y;
	}
	void operator-= (pos const &d)
	{
		x-=d.x;
		y-=d.y;
	}
	friend bool operator== (pos const &l, pos const &r)
	{
		return l.x==r.x && l.y==r.y;
	}
	friend std::ostream& operator<< (std::ostream &o, pos const &p)
	{
		o << "(" << p.x << "," << p.y << ")";
		return o;
	}
	int x=0,y=0;
};
pos cw4[4]={{1,0},{0,1},{-1,0},{0,-1}};
char const *cw4n=">V<^";
char const *cw4r="RDLU";

bool valid_pos(pos p)
{
	return 0<=p.x && p.x<X && 0<=p.y && p.y<Y;
}

struct source
{
	friend std::ostream& operator<< (std::ostream &o, source const &s)
	{
		o << s.p << "," << s.n;
		return o;
	}
	pos p;
	int n=0;
};

pos target;
std::vector<source> sources;

void load()
{
	scanf("%d%d",&Y,&X);
	assert(1<=X && X<=max_N);
	assert(1<=Y && Y<=max_N);
	// state map méret 3*64 bit
	assert(X*Y<=192);
	bool has_target=0;
	for(int y=0; y<Y; ++y)
	{
		scanf("%s",map_in[y]);
		//std::cout << y << " >" << map_in[y] << "<" << std::endl;
		assert(strlen(map_in[y])==X);
		for(int x=0; x<X; ++x)
		{
			char c=map_in[y][x];
			if(c=='E')
			{
				assert(!has_target);
				has_target=1;
				target={x,y};
			}
			else if('1'<=c && c<='9')
				sources.push_back({{x,y},c-'0'});
			else assert(c=='.');
		}
	}
	assert(has_target);
	assert(!sources.empty());
	// state source map 64 bites
	assert(sources.size()<=64);
}

struct state
{
	friend bool operator== (state const &l, state const&r)
	{
		return l.sm==r.sm
			&& l.mm[0]==r.mm[0] && l.mm[1]==r.mm[1] && l.mm[2]==r.mm[2];
	}
	ull mm[3]={};
	ull sm=0;
};
struct state_hash
{
	size_t operator() (state const &x) const
	{
		size_t r=0;
		r+=size_t(1982361)*x.sm;
		r+=size_t(8712347)*x.mm[0];
		r+=size_t(1286129)*x.mm[1];
		r+=size_t(4765123)*x.mm[2];
		return r;
	}
};

uint nrc=0;
bool rgo=1;
// recursion step stack (used source, direction)
uint rk=0;
std::vector<uint> ru;
std::vector<uint> rd;
// last candidates
ull rlcm=0;
state rs;
std::unordered_map<state,uint,state_hash> rsc;

bool rec(uint k=0)
{
	if(++nrc%1024==0)
	{
		auto t=std::chrono::steady_clock::now();
		std::chrono::duration<double> dt=t-t0;
		if(tl<dt.count())
			rgo=0;
	}
	if(!rgo)
		return false;
	if(rsc[rs]++)
		return false;
	for(uint i=k; i<ru.size(); ++i)
	{
		std::swap(ru[k],ru[i]);
		rs.sm^=1ull<<ru[k];
		source const &s=sources[ru[k]];
		for(uint d=0; d<4; ++d)
		{
			rd[k]=d;
			char buf[max_N];
			pos dp=cw4[d];
			pos p=s.p;
			uint n=0,m=0;
			while(n<max_N && m<s.n && valid_pos(p))
			{
				char c=map_in[p.y][p.x];
				buf[n]=c;
				if(c=='.')
				{
					++m;
					map_in[p.y][p.x]=cw4n[d];
					uint o=p.y*X+p.x;
					rs.mm[o/64]^=1llu<<(o%64);
				}
				else if(c=='E')
				{
					rk=k+1;
					return true;
				}
				p+=dp;
				++n;
			}
			// a lépés nem oldotta meg,
			// de még van felhasználatlan lehetséges utolsó source
			if((rs.sm&rlcm)!=rlcm && rec(k+1))
				return true;
			while(0<n--)
			{
				p-=dp;
				map_in[p.y][p.x]=buf[n];
				if(buf[n]=='.')
				{
					uint o=p.y*X+p.x;
					rs.mm[o/64]^=1llu<<(o%64);
				}
			}
			assert(p==s.p);
		}
		rs.sm^=1ull<<ru[k];
		std::swap(ru[k],ru[i]);
	}
	return false;
}

int main()
{
	load();
	rd.resize(sources.size());
	rlcm=0;
	for(uint i=0; i<sources.size(); ++i)
	{
		ru.push_back(i);
		auto const &s=sources[i];
		if(s.p.x==target.x || s.p.y==target.y)
			rlcm^=1llu<<i;
	}
	//std::cout << "target=" << target << std::endl;
	//for(auto &s: sources)
	//	std::cout << s << std::endl;

	if(rec())
	{
		std::cout << rk << std::endl;
		for(uint i=0; i<rk; ++i)
		{
			auto &s=sources[ru[i]];
			std::cout << s.p.x << " " << s.p.y << " " << cw4r[rd[i]] << std::endl;
		}
		for(int y=0; y<Y; ++y)
			std::cout << map_in[y] << std::endl;
	}
	return 0;
}
