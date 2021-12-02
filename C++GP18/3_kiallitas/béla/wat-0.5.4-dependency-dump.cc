#include <cassert>
#include <cstdio>
#include <cstring>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <iostream>
#include <chrono>
typedef unsigned int uint;
typedef unsigned long long int ull;
int Y=0,X=0;
int const max_N=14;
// state source map 64 bites
int const max_S=64;
// 0: '.'
// 1: 'E'
// 1000+n: source, id=n
// 2000+n: covered by source id=n
char map_in[max_N][max_N+1]={};
int map[max_N][max_N]={};
#define HARD_AF 42

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
	static int dot(pos const &a, pos const &b)
	{
		return a.x*b.x+a.y*b.y;
	}
	static int cross(pos const &a, pos const &b)
	{
		return a.x*b.y-a.y*b.x;
	}
	friend pos operator+ (pos const &l, pos const &r)
	{
		return {l.x+r.x,l.y+r.y};
	}
	friend pos operator- (pos const &l, pos const &r)
	{
		return {l.x-r.x,l.y-r.y};
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
uint S=0;
std::vector<source> sources;

void load()
{
	scanf("%d%d",&Y,&X);
	assert(1<=X && X<=max_N);
	assert(1<=Y && Y<=max_N);
	std::cerr << "X=" << X << ",Y=" << Y << std::endl;
	// state map méret 3*64 bit
	assert(X*Y<=192);
	bool has_target=0;
	for(int y=0; y<Y; ++y)
	{
		scanf("%s",map_in[y]);
#ifdef HARD_AF
		std::cerr << map_in[y] << std::endl;
#endif
		assert(strlen(map_in[y])==X);
		for(int x=0; x<X; ++x)
		{
			char c=map_in[y][x];
			if(c=='E')
			{
				assert(!has_target);
				has_target=1;
				target={x,y};
				map[y][x]=1;
			}
			else if('1'<=c && c<='9')
			{
				map[y][x]=1000+sources.size();
				sources.push_back({{x,y},c-'0'});
			}
			else
			{
				map[y][x]=0;
				assert(c=='.');
			}
		}
	}
	assert(has_target);
	assert(!sources.empty());
	assert(sources.size()<=max_S);
	S=sources.size();
}

struct state
{
	struct hash
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
	friend bool operator== (state const &l, state const&r)
	{
		return l.sm==r.sm
			&& l.mm[0]==r.mm[0] && l.mm[1]==r.mm[1] && l.mm[2]==r.mm[2];
	}
	ull mm[3]={};
	ull sm=0;
};

ull nrc=0;
bool rgo=1;
// recursion step stack (used source, direction)
uint rk=0;
std::vector<uint> ru;
std::vector<uint> rd;
// last candidates
ull lcm=0;
uint nlc=0;
// source direction map
// milyen irányokat van értelme egyáltalán ellenőrizni
std::vector<uint> sdm;
state rs;
std::unordered_set<state,state::hash> rsc;
bool dep[max_S+1][max_S]={};

void dump_map()
{
	std::cout << "solved map" << std::endl;
	for(int y=0; y<Y; ++y)
	{
		for(int x=0; x<X; ++x)
		{
			if(map[y][x]<2000)
				std::cout << map_in[y][x];
			else
			{
				uint sid=map[y][x]-2000;
				assert(sid<sources.size());
				source const &s=sources[sid];
				if(s.p.x==x)
					std::cout << (s.p.y<y?"V":"^");
				else std::cout << (s.p.x<x?">":"<");
			}
		}
		std::cout << std::endl;
	}
}

void dump_dep()
{
	std::cout << "dep" << std::endl;
	uint c[max_S+1]={};
	uint sd[max_S+1]={};
	c[S]=1;
	for(uint i=0; i<rk; ++i)
		sd[ru[i]]=rd[i];
	for(uint i=0; i<=S; ++i)
		for(uint j=0; j<S; ++j)
			if(dep[i][j])
				++c[j];
	for(uint i=0; i<=S; ++i)
	{
		if(i<S)
		{
			if(c[i])
				std::cout << i << cw4r[sd[i]] << ": ";
			else std::cout << i << "-";
		}
		else std::cout << "E: ";
		for(uint j=0,k=0; j<S; ++j)
			if(dep[i][j])
				std::cout << (k++?",":"") << j;
		std::cout << std::endl;
	}
}

void dump()
{
	std::cout << rk << std::endl;
	for(uint i=0; i<rk; ++i)
	{
		auto &s=sources[ru[i]];
		std::cout << s.p.x << " " << s.p.y << " " << cw4r[rd[i]] << std::endl;
	}
	dump_map();
	dump_dep();
}

bool rec(uint k=0)
{
#ifndef HARD_AF
	if(++nrc%1024==0)
	{
		auto t=std::chrono::steady_clock::now();
		std::chrono::duration<double> dt=t-t0;
		if(tl<dt.count())
			rgo=0;
	}
	if(!rgo)
		return false;
#endif
	if(!rsc.insert(rs).second)
		return false;
	for(uint i=k; i<ru.size(); ++i)
	{
		std::swap(ru[k],ru[i]);
		uint sid=ru[k];
		rs.sm^=1ull<<sid;
		source const &s=sources[sid];
		//! utolsó lc: csak a target irányába kellene mennie
		// macerás leírni és inkább rábízom hogy megtalálja
		// max 4, avg 2.5 próbálkozásból
		// innen már nem megy mélyebbre a rekurzió
		for(uint d=0; d<4; ++d)
		{
			if(~sdm[sid]&(1<<d))
				continue;
			rd[k]=d;
			pos dp=cw4[d];
			pos p=s.p;
			uint n=0;
			for(uint m=0; n<max_N && m<s.n && valid_pos(p); )
			{
				if(map[p.y][p.x]==0)
				{
					++m;
					map[p.y][p.x]=2000+sid;
					uint o=p.y*X+p.x;
					rs.mm[o/64]^=1llu<<(o%64);
				}
				else if(map[p.y][p.x]==1)
				{
					dep[S][sid]=1;
					rk=k+1;
					return true;
				}
				else if(2000<=map[p.y][p.x])
					dep[sid][map[p.y][p.x]-2000]=1;
				p+=dp;
				++n;
			}
			/*if((rs.sm&lcm)==lcm)
			{
				rk=k+1;
				dump();
				std::cout << std::endl;
			}*/
			// a lépés nem oldotta meg,
			// de még van felhasználatlan lehetséges utolsó source
			if((rs.sm&lcm)!=lcm && rec(k+1))
				return true;
			while(0<n--)
			{
				p-=dp;
				if(2000<=map[p.y][p.x])
				{
					if(map[p.y][p.x]==2000+sid)
					{
						map[p.y][p.x]=0;
						uint o=p.y*X+p.x;
						rs.mm[o/64]^=1llu<<(o%64);
					}
					else dep[sid][map[p.y][p.x]-2000]=0;
				}
			}
			assert(p==s.p);
		}
		rs.sm^=1ull<<sid;
		std::swap(ru[k],ru[i]);
	}
	return false;
}

void pre()
{
	lcm=0;
	nlc=0;
	double C=1;
	for(uint i=0; i<sources.size(); ++i)
	{
		ru.push_back(i);
		rd.push_back(0);
		auto const &s=sources[i];
		pos vt=target-s.p;
		bool is_lc= s.p.x==target.x || s.p.y==target.y;
		if(is_lc)
		{
			lcm^=1llu<<i;
			++nlc;
		}
		// empty rectangle on the top-left,top-right,bottom-right,bottom-left
		bool er[4]={1,1,1,1};
		for(int y=0; y<s.p.y; ++y)
		{
			for(int x=0; x<s.p.x; ++x)
				if(map_in[y][x]!='.')
					er[0]=0;
			for(int x=s.p.x+1; x<X; ++x)
				if(map_in[y][x]!='.')
					er[1]=0;
		}
		for(int y=s.p.y+1; y<Y; ++y)
		{
			for(int x=0; x<s.p.x; ++x)
				if(map_in[y][x]!='.')
					er[3]=0;
			for(int x=s.p.x+1; x<X; ++x)
				if(map_in[y][x]!='.')
					er[2]=0;
		}
		uint dm=0,nvd=0;
		for(uint d=0; d<4; ++d)
		{
			pos v=cw4[d];
			// nem a target felé néz, lehet hogy kidobjuk
			if(pos::cross(vt,v)!=0 || pos::dot(vt,v)<0)
			{
				// jobbra irány: jobb felső vagy jobb alsó téglalap üres
				// és így tovább, körben minden irányra a két kb útba eső
				// téglalappal csekkelve
				// ha egy ilyen téglalap üres, akkor onnan nem mászhat
				// senki sem keresztül rajtunk, illetve nem érdekel minket
				// hogy valaki oda be tud-e jutni vagy sem
				if(er[(d+1)%4] || er[(d+2)%4])
					continue;
			}
			dm^=1<<d;
			++nvd;
		}
		sdm.push_back(dm);
		if(nvd)
			C*=(1+i)*nvd;
	}
#ifdef HARD_AF
	fprintf(stderr,"sdms\n");
	for(int y=0; y<Y; ++y)
	{
		for(int x=0; x<X; ++x)
		{
			uint dm=42;
			for(uint i=0; i<sources.size(); ++i)
				if(sources[i].p.x==x && sources[i].p.y==y)
					dm=sdm[i];
			if(dm<42)
				fprintf(stderr,"%x",dm);
			else fprintf(stderr,".");
		}
		fprintf(stderr,"\n");
	}
	std::cerr << "target=" << target << std::endl;
	std::cerr << "sources[" << sources.size() << "]=" << std::endl;
	for(uint i=0; i<sources.size(); ++i)
	{
		auto const &s=sources[i];
		std::cerr << "id=" << i << ",p=" << s.p << ",n=" << s.n
			<< ",dm=" << std::hex << sdm[i] << std::dec
			<< ((lcm&(1ull<<i))?",lc":"")
			<< std::endl;
	}
#endif
	fprintf(stderr,"S=%zu,C=%.3g\n",sources.size(),C);
}

int main()
{
	load();
	pre();

	if(rec())
		dump();
	return 0;
}
