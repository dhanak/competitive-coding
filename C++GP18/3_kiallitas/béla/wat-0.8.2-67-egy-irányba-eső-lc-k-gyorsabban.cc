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
int const max_N=16;
char map_in[max_N][max_N+1]={};
// 0: '.'
// 1: 'E'
// 1000+n: source, id=n
// 2000+n: covered by source id=n
int16_t map[max_N][max_N]={};
//#define HARD_AF 42

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
	friend bool operator!= (pos const &l, pos const &r)
	{
		return l.x!=r.x || l.y!=r.y;
	}
	friend std::ostream& operator<< (std::ostream &o, pos const &p)
	{
		o << "(" << p.x << "," << p.y << ")";
		return o;
	}
	int x=0,y=0;
};
pos cw4[4]={{0,-1},{1,0},{0,1},{-1,0}};
char const *cw4r="^>v<";

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
	// target direction, valid for last candidates
	bool lc=0;
	uint td=42;
	// top-left,top-right,bottom-right,bottom-left
	// ha nem a target felé néz és a csekkelt iránytól balra vagy jobbra
	// van egy nagy üres téglalap, akkor nincs értelme
	// abba az irányba próbálkoznunk, mert nem számít ha valaki
	// átlóg rajtunk keresztül az üres téglalapba,
	// illetve az üres téglalapból senki sem akar majd kimászni
	// az egyes irányokban milyen sourcék útjában vagyunk
	std::vector<uint> erd[4];
	// hányan vannak az egyes irányokban útban (source+target összesen)
	// a sourcék felhasználásukkor az erd segítségével csökkentik
	// a többiek számlálóját
	uint ner[4]={};
};

pos target;
// state source map 64 bites
int const max_S=64;
uint S=0;
source sources[max_S];

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
uint ru[max_S];
uint rd[max_S];
// last candidates
uint nlc=0;
uint lcs[max_S];
state rs;
std::unordered_set<state,state::hash> rsc;
bool dep[max_S+1][max_S]={};
// csdx[n] active source count where s[i].p.x==n
uint csdx[max_N]={},csdy[max_N]={};

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
				assert(sid<S);
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
			std::cout << sources[i].p << " ";
			if(c[i])
				std::cout << i << cw4r[sd[i]] << ": ";
			else std::cout << i << "-";
		}
		else std::cout << target << " E: ";
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
		std::cout << s.p.y+1 << " " << s.p.x+1 << " " << cw4r[rd[i]] << std::endl;
	}
#ifdef HARD_AF
	std::cout << "nrc=" << nrc << std::endl;
	dump_map();
	dump_dep();
#endif
}

void who_is_in_rect(uint sid, uint d, int xb, int yb, int xe, int ye)
{
	assert(sid<S);
	uint n=0;
	for(int y=yb; y<ye; ++y)
		for(int x=xb; x<xe; ++x)
			if(map[y][x]!=0)
			{
				++sources[sid].ner[d];
				if(map[y][x]!=1)
				{
					assert(1000<=map[y][x] && map[y][x]<2000);
					uint s=map[y][x]-1000;
					assert(s<S);
					sources[s].erd[d].push_back(sid);
				}
			}
}

bool can_be_finished()
{
	// egy irányba eső felhasználatlan lc-k száma
	uint nd[4]={};
	for(uint l=0; l<nlc; ++l)
		if(!((rs.sm>>lcs[l])&1))
			// több mint egy felhasználatlan lc esik egy irányba,
			// akár még működhet is, nem vizsgáljuk komolyabban
			if(1<++nd[sources[lcs[l]].td])
				return true;
	for(uint l=0; l<nlc; ++l)
	{
		source const &s=sources[lcs[l]];
		pos v=cw4[s.td];
		// olyan üres cellák száma, amik keresztből már nem tölthetők fel
		// mert keresztirányban már nincs felhasználatlan source
		uint n0=0;
		for(pos p=target-v; n0<s.n && p!=s.p; p-=v)
			if(map[p.y][p.x]==0 && (s.td%2==0?csdy[p.y]:csdx[p.x])==0)
				++n0;
		// ez még akár lehet megoldható is
		// persze semmi sem garantálja hogy elér idáig a fel nem használt
		// source, de ha nincs is source, akkor biztos nem fog elérni idáig
		if(n0<s.n)
			return true;
	}
	return false;
}

bool rec(uint k=0)
{
	++nrc;
#ifndef HARD_AF
	if(nrc%1024==0)
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
	for(uint rui=k; rui<S; ++rui)
	{
		std::swap(ru[k],ru[rui]);
		uint sid=ru[k];
		source const &s=sources[sid];
		--csdx[s.p.x];
		--csdy[s.p.y];
		rs.sm^=1ull<<sid;
		for(uint d=0; d<4; ++d)
			for(uint o: s.erd[d])
				--sources[o].ner[d];
		if(s.lc)
		{
			uint i=0;
			while(lcs[i]!=sid && i<max_S)
				++i;
			assert(i<max_S);
			std::swap(lcs[i],lcs[nlc-1]);
			--nlc;
		}
		for(uint d=0; d<4; ++d)
		{
			// ha a target felé nézünk, akkor kipróbáljuk
			// ha balra vagy jobbra er van akkor nem megyünk el erre
			// utolsó lc csak a helyes irányban próbálkozzon
			if(s.td!=d && (!s.ner[d%4] || !s.ner[(d+1)%4] || !nlc))
				continue;
			rd[k]=d;
			pos v=cw4[d];
			pos p=s.p+v;
			uint n=0;
			// van még valaki aki keresztezhetné az utunkat később?
			// number of possible later crossings
			uint npc=0;
			for(uint m=0; m<s.n && valid_pos(p); )
			{
				if(map[p.y][p.x]==0)
				{
					++m;
					map[p.y][p.x]=2000+sid;
					npc+=csdx[p.x]+csdy[p.y];
					uint o=p.y*X+p.x;
					rs.mm[o/64]^=1llu<<(o%64);
				}
				else if(2000<=map[p.y][p.x])
					dep[sid][map[p.y][p.x]-2000]=1;
				else if(map[p.y][p.x]==1)
				{
					dep[S][sid]=1;
					rk=k+1;
					return true;
				}
				p+=v;
				++n;
			}

			// a lépés nem oldotta meg,
			// de még van felhasználatlan lehetséges utolsó source
			if(0<nlc && 0<npc && can_be_finished() && rec(k+1))
				return true;
			while(0<n--)
			{
				p-=v;
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
			p-=v;
			assert(p==s.p);
		}
		for(uint d=0; d<4; ++d)
			for(uint o: s.erd[d])
				++sources[o].ner[d];
		nlc+=s.lc;
		rs.sm^=1ull<<sid;
		++csdx[s.p.x];
		++csdy[s.p.y];
		std::swap(ru[k],ru[rui]);
	}
	return false;
}

void load()
{
	scanf("%d%d",&Y,&X);
	assert(1<=X && X<=max_N);
	assert(1<=Y && Y<=max_N);
#ifdef HARD_AF
	std::cerr << "X=" << X << ",Y=" << Y << std::endl;
#endif
	// state map méret 3*64 bit
	assert(X*Y<=192);
	bool has_target=0;
	S=0;
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
				map[y][x]=1000+S;
				sources[S++]={{x,y},c-'0'};
			}
			else
			{
				map[y][x]=0;
				assert(c=='.');
			}
		}
	}
	assert(has_target);
	assert(0<S);
	assert(S<=max_S);

	rk=0;
	nlc=0;
	double C=1;
	for(uint i=0; i<S; ++i)
	{
		source &s=sources[i];
		++csdx[s.p.x];
		++csdy[s.p.y];
		ru[i]=i;
		rd[i]=0;
		who_is_in_rect(i,0,0,0,s.p.x,s.p.y);
		who_is_in_rect(i,1,s.p.x+1,0,X,s.p.y);
		who_is_in_rect(i,2,s.p.x+1,s.p.y+1,X,Y);
		who_is_in_rect(i,3,0,s.p.y+1,s.p.x,Y);
		uint nvd=0;
		pos vt=target-s.p;
		for(uint d=0; d<4; ++d)
		{
			pos v=cw4[d];
			// target felé néz
			if(pos::cross(vt,v)==0 && 0<pos::dot(vt,v))
			{
				assert(!s.lc);
				s.td=d;
				s.lc=1;
				lcs[nlc++]={i};
			}
		}
		C*=(1+i)*4;
	}

#ifdef HARD_AF
	std::cerr << "target=" << target << std::endl;
	std::cerr << "sources[" << S << "]=" << std::endl;
	for(uint i=0; i<S; ++i)
	{
		auto const &s=sources[i];
		std::cerr << "id=" << i << ",p=" << s.p << ",n=" << s.n;
		if(s.lc)
			std::cerr << ",td=" << s.td << "(lc)";
		std::cerr << ",ner=(";
		for(uint d=0; d<4; ++d)
			std::cerr << (d?",":"") << s.ner[d];
		std::cerr << "),erd=(";
		for(uint d=0; d<4; ++d)
		{
			std::cerr << (d?",":"") << "(";
			uint n=0;
			for(uint k: s.erd[d])
				std::cerr << (n++?",":"") << k;
			std::cerr << ")";
		}
		std::cerr << ")" << std::endl;
	}
	fprintf(stderr,"S=%u,C=%.3g\n",S,C);
#endif
}

int main()
{
	load();
	if(rec())
		dump();
	return 0;
}
