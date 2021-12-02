#include <cassert>
#include <cstdio>
#include <cstring>
#include <vector>
#include <algorithm>
#include <iostream>
typedef unsigned int uint;
int Y=0,X=0;
int const max_N=12;
char map_in[max_N][max_N+1];

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
	scanf("%u%u",&Y,&X);
	assert(1<=X && X<=max_N);
	assert(1<=Y && Y<=max_N);
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
}

struct step
{
	source s;
	uint d;
};
std::vector<step> steps;

bool rec(uint k)
{
	for(uint i=0; i<k; ++i)
	{
		source s=sources[i];
		sources[i]=sources[k-1];
		for(uint d=0; d<4; ++d)
		{
			steps.push_back({s,d});
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
				}
				else if(c=='E')
					return true;
				p+=dp;
				++n;
			}
			if(rec(k-1))
				return true;
			while(0<n--)
			{
				p-=dp;
				map_in[p.y][p.x]=buf[n];
			}
			assert(p==s.p);
			steps.pop_back();
		}
		sources[i]=s;
	}
	return false;
}

int main()
{
	load();
	//std::cout << "target=" << target << std::endl;
	//for(auto &s: sources)
	//	std::cout << s << std::endl;

	if(rec(sources.size()))
	{
		std::cout << steps.size() << std::endl;
		for(auto &s: steps)
			std::cout << s.s.p.x << " " << s.s.p.y << " " << cw4r[s.d] << std::endl;
		for(int y=0; y<Y; ++y)
			std::cout << map_in[y] << std::endl;
	}
	return 0;
}

