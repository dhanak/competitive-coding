#include <cassert>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <iostream>
typedef unsigned int uint;

uint const max_N=100;
uint const max_M=100;
int const min_A=-1000;
int const max_A=1000;
uint N=0,M=0;
int A[max_N][max_M]={};
int const inf=1e9;

struct pos
{
	int x,y;
};

struct edge
{
	uint i,j;
	uint u;
	int c;
};

uint const max_E= 50000000; // 2*max_N*max_M+ max_N*max_M*max_N*max_M;
uint E=0;
edge edges[max_E];
uint V=0;

void link(uint i, uint j, uint u, int c)
{
	assert(E+2<=max_E);
	edges[E++]={i,j,u,c};
	edges[E++]={j,i,0,-c};
}

int main()
{
	scanf("%u%u",&N,&M);
	assert(1<=N && N<=max_N);
	assert(1<=M && M<=max_M);

	E=0;
	uint V=0;
	uint s=V++;
	uint v0=V;
	V+=N*M;
	uint t=V++;

	int sa=0;
	std::vector<uint> supply,demand;
	std::vector<pos> p(V);
	for(uint i=0,k=v0; i<N; ++i)
		for(uint j=0; j<M; ++j,++k)
		{
			p[k]={int(j),int(i)};
			int a=0;
			scanf("%d",&a);
			assert(min_A<=a && a<=max_A);
			A[i][j]=a;
			sa+=a;
			if(0<a)
			{
				link(s,k,a,0);
				supply.push_back(k);
			}
			else if(a<0)
			{
				link(k,t,-a,0);
				demand.push_back(k);
			}
		}
	assert(sa==0);

	for(uint i: supply)
	{
		for(uint j: demand)
		{
			int d=std::abs(p[i].x-p[j].x)+std::abs(p[i].y-p[j].y);
			link(i,j,inf,d);
		}
	}
	std::cerr << "V=" << V << ",E=" << E << ",max_E=" << max_E << std::endl;

	uint tf=0;
	int tc=0;
	std::vector<uint> back(V);
	while(1)
	{
		std::vector<int> d(V,inf);
		d[s]=0;
		bool go=1;
		for(uint i=1; go && i<V; ++i)
		{
			go=0;
			for(uint k=0; k<E; ++k)
			{
				edge &e=edges[k];
				if(0<e.u && d[e.i]!=inf && d[e.i]+e.c<d[e.j])
				{
					d[e.j]=d[e.i]+e.c;
					back[e.j]=k;
					go=1;
				}
			}
		}
		if(d[t]==inf)
			break;
		uint u=inf;
		for(uint n=t; n!=s;)
		{
			edge &e0=edges[back[n]];
			edge &e1=edges[back[n]^1];
			assert(e0.i!=t);
			assert(0<e0.u);
			if(e0.u<u)
				u=e0.u;
			n=e0.i;
		}
		assert(0<u && u<inf);
		for(uint n=t; n!=s;)
		{
			edge &e0=edges[back[n]];
			edge &e1=edges[back[n]^1];
			e0.u-=u;
			e1.u+=u;
			n=e0.i;
		}
		tf+=u;
		tc+=u*d[t];
	}
	std::cerr << "tf=" << tf << ",tc=" << tc << std::endl;

	for(uint i=0; i<E; i+=2)
	{
		edge const &e0=edges[i];
		edge const &e1=edges[i+1];
		if(e0.i==s || e0.j==t)
			continue;
		if(e1.u==0)
			continue;
		pos u=p[e0.i];
		pos v=p[e0.j];
		printf("%d %d %d %d %d\n",u.y+1,u.x+1,v.y+1,v.x+1,e1.u);
	}
	printf("0\n");
	return 0;
}
