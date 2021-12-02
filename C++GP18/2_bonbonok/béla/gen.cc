#include <cstdio>
#include <iostream>
#include <vector>
#include <inttypes.h>
typedef unsigned int uint;

uint Y=0,X=0;
uint const max_map_dx=30;
uint const max_map_dy=30;
uint mod=0;
std::vector<std::vector<uint>> map;

struct pos
{
	uint x,y;
};

uint const max_tile_dx=8;
uint const max_tile_dy=8;
struct tile
{
	pos p;
	uint dx,dy;
	std::vector<std::vector<uint>> a;
};
uint N=0;
uint const max_N=50;
std::vector<tile> tiles;

void add(std::vector<std::vector<uint>> &map, uint &score,
	pos const &p, tile const &t)
{
	for(uint y=0; y<t.dy; ++y)
		for(uint x=0; x<t.dx; ++x)
		{
			uint &a=map[p.y+y][p.x+x];
			score-=a;
			a=(a+t.a[y][x])%mod;
			score+=a;
		}
}

void remove(std::vector<std::vector<uint>> &map, uint &score,
	pos const &p, tile const &t)
{
	for(uint y=0; y<t.dy; ++y)
		for(uint x=0; x<t.dx; ++x)
		{
			uint &a=map[p.y+y][p.x+x];
			score-=a;
			a=(a+mod-t.a[y][x])%mod;
			score+=a;
		}
}

class mersenne_twister
{
public:
	mersenne_twister(uint seed_value=0x6ec1be1a): _at(0)
	{
		seed(seed_value);
	}
	void seed(uint s)
	{
		_mt[0]=s;
		for(uint i=1; i<N; ++i)
			_mt[i]= 1812433253*(_mt[i-1]^(_mt[i-1]>>30))+i;
		_at=N;
	}
	uint u32()
	{
		if(N<=_at)
		{
			uint const M=397;
			uint32_t const H=0x80000000u;
			uint32_t const L=0x7fffffffu;
			uint32_t const A[2]={0, 0x9908b0dfu};
			uint k=0;
			for(; k<N-M; ++k)
			{
				uint32_t y=(_mt[k]&H)|(_mt[k+1]&L);
				_mt[k]=_mt[k+M]^(y>>1)^A[y&1];
			}
			for(; k<N-1; ++k)
			{
				uint32_t y=(_mt[k]&H)|(_mt[k+1]&L);
				_mt[k]=_mt[k+M-N]^(y>>1)^A[y&1];
			}
			uint32_t y=(_mt[N-1]&H)|(_mt[0]&L);
			_mt[N-1]=_mt[M-1]^(y>>1)^A[y&1];
			_at=0;
		}

		uint32_t r=_mt[_at++];
		r^=(r>>11);
		r^=(r<<7)&0x9d2c5680u;
		r^=(r<<15)&0xefc60000u;
		r^=(r>>18);
		return r;
	}
	// [0,n)
	uint lt(uint n)
	{
		while(1)
		{
			uint x=u32();
			uint q=x/n;
			uint b=q*n;
			uint r=x-b;
			if(b<=0-n)
				return r;
		}
	}
	template <typename random_access_iterator>
	void perm(random_access_iterator begin, random_access_iterator end)
	{
		uint n=end-begin;
		for(uint i=0; i+1<n; ++i)
		{
			uint k=lt(n-i);
			std::swap(*(begin+i),*(begin+i+k));
		}
	}
	template <typename random_access_container>
	void perm(random_access_container &c)
	{
		perm(c.begin(),c.end());
	}

private:
	static uint const N=624;
	uint _mt[N];
	uint _at;
};

void compute()
{
	map.assign(Y,std::vector<uint>(X,0));
	for(uint i=0; i<N; ++i)
	{
		tile const &t=tiles[i];
		for(uint y=0; y<t.dy; ++y)
			for(uint x=0; x<t.dx; ++x)
			{
				uint &a=map[t.p.y+y][t.p.x+x];
				a=(a+mod-t.a[y][x])%mod;
			}
	}
}

void dump()
{
	std::cout << Y << " " << X << " " << mod << std::endl;
	for(uint y=0; y<Y; ++y)
		for(uint x=0; x<X; ++x)
			std::cout << map[y][x] << (x+1==X?"\n":" ");
	std::cout << N << std::endl;
	for(uint i=0; i<N; ++i)
	{
		tile const &t=tiles[i];
		std::cout << t.dy << " " << t.dx << std::endl;
		for(uint y=0; y<t.dy; ++y)
		{
			for(uint x=0; x<t.dx; ++x)
				std::cout << (x?" ":"") << t.a[y][x];
			std::cout << std::endl;
		}
	}
	for(uint i=0; i<N; ++i)
		std::cerr << tiles[i].p.y+1 << " " << tiles[i].p.x+1 << std::endl;
}

int main()
{
	// sok tile kerül a keretre
	// keret-1
	if(0)
	{
		mersenne_twister rng(23455);
		X=23;
		Y=29;
		mod=7;
		N=50;
		tiles.clear();
		for(uint i=0; i<N; ++i)
		{
			tile t;
			t.dx=7+rng.lt(2);
			t.dy=7+rng.lt(2);
			t.a.assign(t.dy,std::vector<uint>(t.dx,0));
			for(uint y=0; y<t.dy; ++y)
				for(uint x=0; x<t.dx; ++x)
					t.a[y][x]=1+rng.lt(mod-1);
			t.p={rng.lt(X+1-t.dx),rng.lt(Y+1-t.dy)};
			if(i<N*3/4)
			{
				switch(rng.lt(4))
				{
				case 0: t.p.x=0; break;
				case 1: t.p.x=X-t.dx; break;
				case 2: t.p.y=0; break;
				case 3: t.p.y=Y-t.dy; break;
				}
			}
			tiles.push_back(t);
		}
		compute();
		dump();
	}

	// tilék az első sorokban
	// első-sor-1
	if(1)
	{
		mersenne_twister rng(352362);
		X=30;
		Y=3;
		mod=5;
		N=50;
		tiles.clear();
		for(uint i=0; i<N; ++i)
		{
			tile t;
			t.dx=5+rng.lt(4);
			t.dy=1+rng.lt(2);
			t.a.assign(t.dy,std::vector<uint>(t.dx,0));
			for(uint y=0; y<t.dy; ++y)
				for(uint x=0; x<t.dx; ++x)
					t.a[y][x]=rng.lt(mod);
			t.p={rng.lt(X+1-t.dx),0};
			tiles.push_back(t);
		}
		compute();
		dump();
	}
	return 0;
}
