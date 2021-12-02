#include <cassert>
#include <cstdio>
#include <chrono>
#include <vector>
#include <algorithm>
#include <iostream>
#include <inttypes.h>
typedef unsigned int uint;

auto t0=std::chrono::steady_clock::now();
double const tl=5.0; // HR: 1.95

uint Y=0,X=0;
uint const max_map_dx=30;
uint const max_map_dy=30;
uint mod=0;
std::vector<std::vector<uint>> map0;

uint const max_tile_dx=8;
uint const max_tile_dy=8;
struct tile
{
	uint dx,dy;
	std::vector<std::vector<uint>> a;
};
uint N=0;
std::vector<tile> tiles;

struct pos
{
	uint x,y;
};

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
mersenne_twister rng;

void load()
{
	scanf("%u%u%u",&Y,&X,&mod);
	assert(1<=Y && Y<=max_map_dy);
	assert(1<=X && X<=max_map_dx);
	assert(2<=mod && mod<=9);
	map0.assign(Y,std::vector<uint>(X,0));
	for(uint y=0; y<Y; ++y)
		for(uint x=0; x<X; ++x)
		{
			scanf("%u",&map0[y][x]);
			assert(map0[y][x]<mod);
		}
	scanf("%u",&N);
	assert(1<=N && N<=50);
	tiles.clear();
	for(uint n=0; n<N; ++n)
	{
		tile t;
		scanf("%u%u",&t.dy,&t.dx);
		assert(1<=t.dx && t.dx<=8);
		assert(1<=t.dy && t.dy<=8);
		t.a.assign(t.dy,std::vector<uint>(t.dx,0));
		for(uint y=0; y<t.dy; ++y)
			for(uint x=0; x<t.dx; ++x)
			{
				scanf("%u",&t.a[y][x]);
				assert(t.a[y][x]<mod);
			}
		tiles.push_back(t);
	}
}

void random_bullshit()
{
	uint bs=0;
	std::vector<pos> bp;
	auto bm=map0;
	for(bool go=1; go;)
	{
		auto map=map0;
		std::vector<pos> P(N);
		for(uint i=0; i<N; ++i)
		{
			tile const &t=tiles[i];
			P[i]={rng.lt(X+1-t.dx),rng.lt(Y+1-t.dy)};
			for(uint y=0; y<t.dy; ++y)
				for(uint x=0; x<t.dx; ++x)
					map[P[i].y+y][P[i].x+x]+=t.a[y][x];
		}
		uint s=0;
		for(uint y=0; y<Y; ++y)
			for(uint x=0; x<X; ++x)
				s+=(map[y][x]%=mod);
		if(bp.empty() || s<bs)
		{
			bs=s;
			bp=P;
			bm=map;
		}
		auto t=std::chrono::steady_clock::now();
		std::chrono::duration<double> dt=t-t0;
		go=0<bs && dt.count()<tl;
	}

	std::cerr << "err=" << bs << std::endl;
	/*for(uint y=0; y<Y; ++y)
	{
		for(uint x=0; x<X; ++x)
			std::cerr << (x?" ":"") << bm[y][x];
		std::cerr << std::endl;
	}*/

	for(pos p: bp)
		printf("%u %u\n",p.y+1,p.x+1);
}

int main()
{
	load();
	random_bullshit();
	return 0;
}
