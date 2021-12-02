#include <cassert>
#include <cstdio>
#include <vector>
#include <queue>
#include <algorithm>
#include <iostream>
typedef unsigned int uint;

uint const max_N=100;
uint const max_M=100;
int const min_A=-1000;
int const max_A=1000;
int const inf=1e9;

struct pos
{
	int x,y;
};

struct edge
{
	uint u,v;
	uint cap;
	int cost;
};

std::vector<edge> edges;
std::vector<std::vector<uint>> adj;
uint V=0;

void link(uint u, uint v, uint cap, int cost)
{
	adj[u].push_back(edges.size());
	edges.push_back({u,v,cap,cost});
	adj[v].push_back(edges.size());
	edges.push_back({v,u,0,-cost});
}

struct qe
{
	friend bool operator< (qe const &l, qe const &r)
	{
		return l.d>r.d;
	}
	int d;
	uint u;
};

int main()
{
	uint N=0,M=0;
	scanf("%u%u",&N,&M);
	assert(1<=N && N<=max_N);
	assert(1<=M && M<=max_M);

	uint V=0;
	uint s=V++;
	uint v0=V;
	V+=N*M;
	uint t=V++;
	edges.clear();
	adj.assign(V,{});

	int sa=0;
	std::vector<uint> supply,demand;
	std::vector<pos> p(V);
	std::vector<std::vector<int>> A(N,std::vector<int>(M,0));
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
	std::cerr << "V=" << V << ",E=" << edges.size() << std::endl;

	uint tf=0;
	int tc=0;
	std::vector<uint> back(V);
	std::vector<int> pi(V,0);
	std::vector<int> d;
	std::vector<bool> vis;
	while(1)
	{
		d.assign(V,inf);
		d[s]=0;
		std::priority_queue<qe> q;
		q.push({0,s});
		vis.assign(V,0);
		while(!q.empty())
		{
			uint u=q.top().u;
			int du=q.top().d;
			q.pop();
			if(vis[u])
				continue;
			vis[u]=1;
			for(uint k: adj[u])
			{
				edge &e=edges[k];
				uint v=e.v;
				if(e.cap==0 || vis[v])
					continue;
				int dv=du+e.cost+pi[u]-pi[v];
				if(dv<d[v])
				{
					d[v]=dv;
					back[e.v]=k;
					q.push({dv,v});
				}
			}
		}
		if(d[t]==inf)
			break;
		int w=inf;
		for(uint n=t; n!=s;)
		{
			edge &e0=edges[back[n]];
			assert(e0.u!=t);
			assert(0<e0.cap);
			if(e0.cap<w)
				w=e0.cap;
			n=e0.u;
		}
		tf+=w;
		tc+=w*(d[t]-pi[s]+pi[t]);
		for(uint n=t; n!=s;)
		{
			edge &e0=edges[back[n]];
			edge &e1=edges[back[n]^1];
			e0.cap-=w;
			e1.cap+=w;
			n=e0.u;
		}
		for(uint i=0; i<V; ++i)
			pi[i]=std::min(pi[i]+d[i],inf);
	}
	std::cerr << "tf=" << tf << ",tc=" << tc << std::endl;

	for(uint i=0; i<edges.size(); i+=2)
	{
		edge const &e0=edges[i];
		edge const &e1=edges[i+1];
		if(e0.u==s || e0.v==t)
			continue;
		if(e1.cap==0)
			continue;
		pos u=p[e0.u];
		pos v=p[e0.v];
		printf("%d %d %d %d %d\n",u.y+1,u.x+1,v.y+1,v.x+1,e1.cap);
	}
	printf("0\n");
	return 0;
}

