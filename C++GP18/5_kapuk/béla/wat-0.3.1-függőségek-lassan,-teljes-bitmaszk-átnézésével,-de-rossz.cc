#include <cassert>
#include <cstdio>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
typedef unsigned int uint;
typedef unsigned long long ull;
uint const max_K=32;
uint const max_L=32;
uint const max_N=32768;
uint K=0,L=0;
std::map<uint,uint> sample;

// key: felső 32 bit az érintett bitek maszkja, alsó 32 bit a konkrét értékek
// value: node id
ull p_key(uint m, uint v)
{
	return ull(m)<<32|v;
}
std::map<ull,uint> p_in;

struct node
{
	std::string name;
	std::vector<uint> in;
};
std::vector<node> network;

uint not_in_0=0;

uint build_sel_r(uint b, uint e, uint m, uint v)
{
	assert(b<e);
	if(b+1==e)
		return v?b:not_in_0+b;
	ull k=p_key(m,v);
	auto at=p_in.find(k);
	if(at!=p_in.end())
		return (*at).second;
	uint o=(b+e)/2;
	uint lm=(1u<<o)-1;
	uint lid=build_sel_r(b,o,m&lm,v&lm);
	uint rid=build_sel_r(o,e,m&~lm,v&~lm);
	uint id=network.size();
	p_in[k]=id;
	network.push_back({"AND",{lid,rid}});
	return id;
}

uint build_sel(uint v)
{
	return build_sel_r(0,K,(1llu<<K)-1,v);
}

void stats()
{
	uint M=1<<(K-1);
	for(uint l=0; l<L; ++l)
	{
		uint nd=0,md=0;
		for(uint k=0; k<K; ++k)
		{
			uint km=(1<<k)-1;
			//printf("%u %08x %u %08x\n",l,~km<<1,k,km);
			bool dep=0;
			for(uint m=0; !dep && m<M; ++m)
			{
				uint x0=((m&~km)<<1)|m&km;
				uint x1=x0|(1<<k);
				//printf("x0=%08x,x1=%08x\n",x0,x1);
				auto at0=sample.find(x0);
				if(at0==sample.end())
					continue;
				auto at1=sample.find(x1);
				if(at1==sample.end())
					continue;
				uint v0=(*at0).second;
				uint v1=(*at1).second;
				uint b0=(v0>>l)&1;
				uint b1=(v1>>l)&1;
				//printf("v0=%08x,v1=%08x,b0=%d,b1=%d\n",v0,v1,b0,b1);
				dep= b0!=b1;
			}
			//printf("%u %u %d\n",l,k,dep);
			nd+=dep;
			md|=dep<<k;
		}
		printf("%u %u %08x\n",l,nd,md);
	}
}

int main()
{
	scanf("%u%u",&K,&L);
	assert(2<=K && K<=max_K);
	assert(1<=L && L<=max_L);
	uint N=0;
	scanf("%u",&N);
	assert(1<=N && N<=max_N && N<=(1ll<<K));
	for(uint n=0; n<N; ++n)
	{
		char si[33],so[33];
		scanf("%32s%32s",si,so);
		uint mi=0,mo=0;
		for(uint i=0; i<K; ++i)
			mi|=(si[i]-'0')<<i;
		for(uint i=0; i<L; ++i)
			mo|=(so[i]-'0')<<i;
		auto i=sample.find(mi);
		if(i!=sample.end())
			assert((*i).second==mo);
		else sample[mi]=mo;
	}
	uint S=uint(sample.size());
	//std::cerr << "sample size " << S << std::endl;
	stats();

/*
	// [0,K) inputok
	for(uint i=0; i<K; ++i)
	{
		p_in[p_key(1<<i,1<<i)]=network.size();
		network.push_back({"EQ",{i}});
	}

	// K db inputra kötött NOT
	not_in_0=network.size();
	for(uint i=0; i<K; ++i)
	{
		p_in[p_key(1<<i,0)]=network.size();
		network.push_back({"NOT",{i}});
	}

	// fix 0 jel előállítása AND(x,NOT(x)) módon
	uint zero_0=network.size();
	network.push_back({"AND",{0,not_in_0+0}});

	// selector
	// S db NOT+AND hálózat az egyes samplékat választja ki
	// 2<=K ugye
	std::vector<uint> sel;
	std::vector<uint> o1s[max_L];
	for(auto x: sample)
	{
		for(uint i=0,m=x.second; i<L; ++i,m>>=1)
			if(m&1)
				o1s[i].push_back(sel.size());
		sel.push_back(build_sel(x.first));
	}
	uint G=network.size()-K;
	uint O=network.size();

	// az outputok marha nagy OR kapukkal,
	// samplénként a selectorok kimenetére vannak kötve ahol egyes kell
	// amelyik sampléban nem kell egyes, azt be sem kötjük,
	// arra az esetre nyilván nullát fog adni
	uint out_0=O;
	G+=L;
	O+=L;

	// belül minden id 0 alapú, mindehol kell a +1
	std::cout << G << std::endl;
	uint cost=0;

	for(uint i=K; i<network.size(); ++i)
	{
		node const &n=network[i];
		std::cout << n.name << " " << n.in.size();
		cost+=n.in.size()+1;
		for(uint k=0; k<n.in.size(); ++k)
			std::cout << " " << n.in[k]+1;
		std::cout << std::endl;
	}
	for(uint l=0; l<L; ++l)
	{
		uint zeros=2-std::min<uint>(o1s[l].size(),2);
		std::cout << "OR " << zeros+o1s[l].size();
		cost+=zeros+o1s[l].size()+1;
		for(uint i=0; i<zeros; ++i)
			std::cout << " " << zero_0+1;
		for(uint i: o1s[l])
			std::cout << " " << sel[i]+1;
		std::cout << std::endl;
	}

	for(uint l=0; l<L; ++l)
		std::cout << out_0+l+1 << std::endl;

	std::cerr << cost << std::endl;*/
	return 0;
}
