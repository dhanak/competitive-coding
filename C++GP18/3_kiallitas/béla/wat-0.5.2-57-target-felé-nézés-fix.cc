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
char map[max_N][max_N+1];

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
		scanf("%s",map[y]);
		//std::cout << y << " >" << map[y] << "<" << std::endl;
		assert(strlen(map[y])==X);
		for(int x=0; x<X; ++x)
		{
			char c=map[y][x];
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
ull lcm=0;
// source direction map
// milyen irányokat van értelme egyáltalán ellenőrizni
std::vector<uint> sdm;
state rs;
std::unordered_set<state,state_hash> rsc;

bool rec(uint k=0)
{
	/*if(++nrc%1024==0)
	{
		//if(nrc%(1<<20)==0)
		//	std::cerr << rsc.size() << std::endl;
		auto t=std::chrono::steady_clock::now();
		std::chrono::duration<double> dt=t-t0;
		if(tl<dt.count())
			rgo=0;
	}
	if(!rgo)
		return false;*/
	if(!rsc.insert(rs).second)
		return false;
	for(uint i=k; i<ru.size(); ++i)
	{
		std::swap(ru[k],ru[i]);
		rs.sm^=1ull<<ru[k];
		source const &s=sources[ru[k]];
		for(uint d=0; d<4; ++d)
		{
			if(~sdm[ru[k]]&(1<<d))
				continue;
			rd[k]=d;
			char buf[max_N];
			pos dp=cw4[d];
			pos p=s.p;
			uint n=0;
			for(uint m=0; n<max_N && m<s.n && valid_pos(p); )
			{
				char c=map[p.y][p.x];
				buf[n]=c;
				if(c=='.')
				{
					++m;
					map[p.y][p.x]=cw4n[d];
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
			if((rs.sm&lcm)!=lcm && rec(k+1))
				return true;
			while(0<n--)
			{
				p-=dp;
				map[p.y][p.x]=buf[n];
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

void pre()
{
	lcm=0;
	double C=1;
	for(uint i=0; i<sources.size(); ++i)
	{
		ru.push_back(i);
		rd.push_back(0);
		auto const &s=sources[i];
		pos vt=target-s.p;
		bool is_lc= s.p.x==target.x || s.p.y==target.y;
		if(is_lc)
			lcm^=1llu<<i;
		// empty rectangle on the top-left,top-right,bottom-right,bottom-left
		bool er[4]={1,1,1,1};
		for(int y=0; y<s.p.y; ++y)
		{
			for(int x=0; x<s.p.x; ++x)
				if(map[y][x]!='.')
					er[0]=0;
			for(int x=s.p.x+1; x<X; ++x)
				if(map[y][x]!='.')
					er[1]=0;
		}
		for(int y=s.p.y+1; y<Y; ++y)
		{
			for(int x=0; x<s.p.x; ++x)
				if(map[y][x]!='.')
					er[3]=0;
			for(int x=s.p.x+1; x<X; ++x)
				if(map[y][x]!='.')
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
	fprintf(stderr,"S=%zu,C=%.3g\n",sources.size(),C);
	/*fprintf(stderr,"sdms\n");
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
	}*/
}

void dump()
{
	std::cout << rk << std::endl;
	for(uint i=0; i<rk; ++i)
	{
		auto &s=sources[ru[i]];
		std::cout << s.p.x << " " << s.p.y << " " << cw4r[rd[i]] << std::endl;
	}
	for(int y=0; y<Y; ++y)
		std::cout << map[y] << std::endl;
}

int main()
{
	load();
	pre();
	//std::cout << "target=" << target << std::endl;
	//for(auto &s: sources)
	//	std::cout << s << std::endl;

	if(rec())
		dump();
	return 0;
}
