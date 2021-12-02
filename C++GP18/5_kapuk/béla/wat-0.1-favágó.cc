#include <cassert>
#include <cstdio>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
typedef unsigned int uint;
uint const max_K=31;
uint const max_L=31;
uint const max_N=32767;
uint K=0,L=0;
std::map<uint,uint> sample;

int main()
{
	scanf("%u%u",&K,&L);
	assert(2<=K && K<=max_K);
	assert(1<=L && L<=max_L);
	uint N=0;
	scanf("%u",&N);
	assert(1<=N && N<=max_N);
	for(uint n=0; n<N; ++n)
	{
		char si[32],so[32];
		scanf("%31s%31s",si,so);
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

	// [0,K) inputok
	uint O=K;
	uint G=0;
	// K db inputra kötött NOT
	uint not_in_0=O;
	G+=K;
	O+=K;
	// fix 0 jel előállítása AND(x,NOT(x)) módon
	uint zero_0=O;
	O+=1;
	G+=1;
	// selector
	// S db K bemenetű AND kapu ami az egyes samplékat választja ki
	// 2<=K ugye
	uint sel_0=O;
	G+=S;
	O+=S;
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

	for(uint i=0; i<K; ++i)
	{
		std::cout << "NOT 1 " << i+1 << std::endl;
		cost+=2;
	}

	std::cout << "AND 2 1 " << not_in_0+1 << std::endl;
	cost+=3;

	std::vector<uint> o1s[max_L];
	{
		uint s=0;
		for(auto x: sample)
		{
			std::cout << "AND " << K;
			cost+=K+1;
			uint mi=x.first;
			for(uint i=0; i<K; ++i,mi>>=1)
				std::cout << " " << (mi&1?i+1:not_in_0+i+1);
			std::cout << std::endl;
			uint mo=x.second;
			for(uint i=0; i<L; ++i,mo>>=1)
				if(mo&1)
					o1s[i].push_back(s);
			++s;
		}
	}

	for(uint l=0; l<L; ++l)
	{
		uint zeros=2-std::min<uint>(o1s[l].size(),2);
		std::cout << "OR " << zeros+o1s[l].size();
		cost+=zeros+o1s[l].size()+1;
		for(uint i=0; i<zeros; ++i)
			std::cout << " " << zero_0+1;
		for(uint i: o1s[l])
			std::cout << " " << sel_0+i+1;
		std::cout << std::endl;
	}

	for(uint l=0; l<L; ++l)
		std::cout << out_0+l+1 << std::endl;

	std::cerr << cost << std::endl;
	return 0;
}

/*
for i in `cd in && ls`; do o="${i%in}out"; echo $i $o; ./a.out <in/$i >out/$o; done 2> os
for i in `cd in && ls`; do f="${i%.in}"; ./checker in/$i out/$f.out > out/$f.score; cat out/$f.score; done > oc
*/
