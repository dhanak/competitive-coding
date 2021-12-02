#include <cassert>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <vector>
typedef unsigned int uint;
typedef long long int ll;
typedef unsigned long long int ull;
uint NA[]={1,2,3,5,7,10,15,20,35,50,75,100,150,200,350,500,750,1000};
uint NB[]={3,5,7,10,15,20,35,50,75,100};
int const min_A=-1000;
int const max_A=1000;

struct pos
{
	friend bool operator== (pos const &l, pos const &r)
	{
		return l.x==r.x && l.y==r.y;
	}
	friend bool operator!= (pos const &l, pos const &r)
	{
		return l.x!=r.x || l.y!=r.y;
	}
	int x,y;
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
	/*
	ull u64()
	{
		uint a=u32();
		uint b=u32();
		return (ull(a)<<32)|b;
	}
	ull lt(ull n)
	{
		while(1)
		{
			ull x=u64();
			ull q=x/n;
			ull b=q*n;
			ull r=x-b;
			if(b<=0-n)
				return r;
		}
	}
	// [l,h]
	uint between(uint l, uint h)
	{
		return l+lt(h-l+1);
	}
	ull between(ull l, ull h)
	{
		return l+lt(h-l+1);
	}
	// 32 bit [0,1]
	double _f32_ge0_le1()
	{
		return u32()*(1.0/4294967295.0);
	}
	// 32 bit [0,1)
	double _f32_ge0_lt1()
	{
		return u32()*(1.0/4294967296.0);
	}
	// 32 bit (0,1)
	double _f32_gt0_lt1()
	{
		return (u32()+0.5)*(1.0/4294967296.0);
	}
	// 53 bit [0,1)
	double _f53_ge0_lt1()
	{
		uint a=u32()>>5;
		uint b=u32()>>6;
		return ((ll(a)<<26)+b)*(1.0/9007199254740992.0);
	}*/

private:
	static uint const N=624;
	uint _mt[N];
	uint _at;
};

uint id=0;

template <typename T>
void save(uint N, uint M, T const &A, char const *file_name)
{
	++id;
	char buf[256];
	snprintf(buf,256,"move%02u.in",id);

	std::cout << buf << " " << file_name << " " << N << " " << M << std::endl;
	FILE *f=fopen(file_name,"wb");
	//FILE *f=fopen(buf,"wb");
	assert(f);
	fprintf(f,"%u %u\n",N,M);
	for(uint i=0; i<N; ++i)
	{
		for(uint j=0; j<M; ++j)
			fprintf(f,"%s%d",(j?" ":""),A[i][j]);
		fprintf(f,"\n");
	}
	fclose(f);
}

// 1-N,M.in
// N*M random mozgatás az üres pályáról indulva
void gen_1()
{
	mersenne_twister rng(283746);
	for(uint N: NB) // NA: túl nagy, túl sok
	{
		uint M=N-rng.lt(std::max(1u,N/10));
		std::vector<std::vector<int>> A(N,std::vector<int>(M,0));
		uint K=N*M;
		for(uint i=0; i<K; ++i)
		{
			uint ai=rng.lt(N);
			uint aj=rng.lt(M);
			uint bi=rng.lt(N);
			uint bj=rng.lt(M);
			uint max_n=std::min(max_A-A[ai][aj],A[bi][bj]-min_A);
			int n=rng.lt(max_n+1);
			//std::cout << ai << "," << aj << " "
			//	<< bi << "," << bj << " "
			//	<< A[ai][aj] << "," << A[bi][bj] << " "
			//	<< max_n << "," << n << std::endl;
			A[ai][aj]+=n;
			A[bi][bj]-=n;
			assert(A[ai][aj]<=max_A);
			assert(min_A<=A[bi][bj]);
		}
		char name[256];
		snprintf(name,256,"1-%u,%u.in",N,M);
		save(N,M,A,name);
	}
}

// N*M random mozgatás felső feléből alsó felébe
void gen_2_1()
{
	mersenne_twister rng(32412);
	for(uint N: NB)
	{
		uint M=N-rng.lt(std::max(1u,N/10));
		std::vector<std::vector<int>> A(N,std::vector<int>(M,0));
		uint K=N*M;
		for(uint i=0; i<K; ++i)
		{
			uint ai=rng.lt(N/2);
			uint aj=rng.lt(M);
			uint bi=N/2+rng.lt(N-N/2);
			uint bj=rng.lt(M);
			uint max_n=std::min(max_A-A[ai][aj],A[bi][bj]-min_A);
			int n=rng.lt(max_n+1);
			A[ai][aj]+=n;
			A[bi][bj]-=n;
			assert(A[ai][aj]<=max_A);
			assert(min_A<=A[bi][bj]);
		}
		char name[256];
		snprintf(name,256,"2.1-%u,%u.in",N,M);
		save(N,M,A,name);
	}
}

// N*M random mozgatás páros sorból (0,2,..) páratlanba
void gen_3()
{
	mersenne_twister rng(12343);
	for(uint N: NB)
	{
		uint M=N-rng.lt(std::max(1u,N/10));
		std::vector<std::vector<int>> A(N,std::vector<int>(M,0));
		uint K=N*M;
		for(uint i=0; i<K; ++i)
		{
			uint ai=0+2*rng.lt((N+1)/2);
			uint aj=rng.lt(M);
			uint bi=1+2*rng.lt(N/2);
			uint bj=rng.lt(M);
			uint max_n=std::min(max_A-A[ai][aj],A[bi][bj]-min_A);
			int n=rng.lt(max_n+1);
			A[ai][aj]+=n;
			A[bi][bj]-=n;
			assert(A[ai][aj]<=max_A);
			assert(min_A<=A[bi][bj]);
		}
		char name[256];
		snprintf(name,256,"3-%u,%u.in",N,M);
		save(N,M,A,name);
	}
}

// néhány random pozíción lévő pontban lévő sok bicajból visz sok helyre keveset
template <typename T>
void gen_4_one(uint N, uint M, T &A, mersenne_twister &rng, double a)
{
	for(uint i=0; i<N; ++i)
		for(uint j=0; j<M; ++j)
			A[i][j]=0;
	uint K=N*M;
	std::vector<pos> P(std::max(1u,uint(K*a+0.5)));
	for(uint i=0; i<P.size(); ++i)
	{
		pos p={0,0};
		for(bool ok=0; !ok;)
		{
			p=pos{int(rng.lt(M)),int(rng.lt(N))};
			ok=1;
			for(uint j=0; ok && j<i; ++j)
				ok=P[j]!=p;
		}
		P[i]=p;
	}
	for(uint k=0; k<K; ++k)
	{
		pos q={int(rng.lt(M)),int(rng.lt(N))};
		pos p=P[rng.lt(P.size())];
		int max_n=std::min(max_A-A[p.y][p.x],A[q.y][q.x]-min_A);
		int n=1+rng.lt(max_A*P.size()/K/2);
		if(max_n<n)
			n=max_n;
		A[p.y][p.x]+=n;
		A[q.y][q.x]-=n;
		assert(A[p.y][p.x]<=max_A);
		assert(min_A<=A[q.y][q.x]);
	}
	char name[256];
	snprintf(name,256,"4-%.3f-%u,%u.in",a,N,M);
	save(N,M,A,name);
}

void gen_4()
{
	mersenne_twister rng(876253);
	double const as[]={0.05,0.1,0.25,0.5,0.85};
	for(double a: as)
	{
		for(uint N: NB)
		{
			uint M=N-rng.lt(std::max(1u,N/4));
			std::vector<std::vector<int>> A(N,std::vector<int>(M,0));
			gen_4_one(N,M,A,rng,a);
		}
	}
}

int main()
{
/*	scanf("%u%u",&N,&M);
	assert(1<=N && N<=1000);
	assert(1<=M && M<=1000);
	int sa=0;
	for(uint i=0; i<N; ++i)
		for(uint j=0; j<M; ++j)
		{
			int a=0;
			scanf("%d",&a);
			assert(-1000<=a && a<=1000);
			A[i][j]=a;
			sa+=a;
		}
	assert(sa==0);*/

	gen_1();
	gen_2_1();
	gen_3();
	gen_4();

	return 0;
}
