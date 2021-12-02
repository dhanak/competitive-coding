#include <cassert>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <iostream>
typedef unsigned int uint;
typedef unsigned long long int ull;

uint const max_N=1000;
uint const max_M=1000;
int const min_A=-1000;
int const max_A=1000;
uint N=0,M=0;
int A[max_N][max_M]={};

int main(int argc, char const **argv)
{
	assert(argc==3 && "./judge in out");

	FILE *f=fopen(argv[1],"rb");
	assert(f && "can't open in");

	fscanf(f,"%u%u",&N,&M);
	assert(1<=N && N<=max_N);
	assert(1<=M && M<=max_M);
	int sa=0;
	for(uint i=0; i<N; ++i)
		for(uint j=0; j<M; ++j)
		{
			int a=0;
			fscanf(f,"%d",&a);
			assert(min_A<=a && a<=max_A);
			A[i][j]=a;
			sa+=a;
		}
	assert(sa==0);
	fclose(f);

	ull r=0;
	f=fopen(argv[2],"rb");
	assert(f && "can't open out");
	int y0,x0,y1,x1,m;
	while(fscanf(f,"%d%d%d%d%d",&y0,&x0,&y1,&x1,&m)==5)
	{
		assert(1<=y0 && y0<=N);
		assert(1<=x0 && x0<=M);
		assert(1<=y1 && y1<=N);
		assert(1<=x1 && x1<=M);
		--y0;
		--x0;
		--y1;
		--x1;
		assert(0<m && m<=max_A);
		assert(0<=A[y0][x0]-m);
		assert(A[y1][x1]+m<=0);
		A[y0][x0]-=m;
		A[y1][x1]+=m;
		int d=std::abs(y0-y1)+std::abs(x0-x1);
		r+=d*m;
	}
	fclose(f);
	for(uint i=0; i<N; ++i)
		for(uint j=0; j<M; ++j)
			assert(A[i][j]==0);
	std::cout << r << std::endl;

	return 0;
}
