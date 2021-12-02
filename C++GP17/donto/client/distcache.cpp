#include "stdafx.h"
#include "parser.h"
#include "distcache.h"



bool DISTCACHE::LoadFromFile(const char * filename)
{
	FILE *f=fopen(filename, "rb");
	if (f==NULL) return false;
	unsigned char dimensions[2];
	fread(dimensions, 1, 2, f);
	map_dx = dimensions[0];
	map_dy = dimensions[1];
	mMap.resize(map_dx*map_dy);
	mDistMap.clear();
	mDistMap.resize(map_dx*map_dy);
	fread(&mMap.front(), 1, mMap.size(), f);
	int x, y;
	for(y=0;y<map_dy;y++)
		for(x=0;x<map_dx;x++)
			if (mMap[x+y*map_dx])
		{
			std::vector<unsigned char> &data = mDistMap[x+y*map_dx];
			data.resize(map_dx*map_dy);
			fread(&data.front(), 1, data.size(), f);
		}
	fclose(f);
	return true;
}

void DISTCACHE::SaveToFile(const char * filename)
{
	FILE *f=fopen(filename, "wb");
	assert(f!=NULL);
	unsigned char dimensions[2];
	dimensions[0]=(unsigned char)map_dx;
	dimensions[1]=(unsigned char)map_dy;
	fwrite(dimensions, 1, 2, f);
	fwrite(&mMap.front(), 1, mMap.size(), f);
	int x, y;
	for(y=0;y<map_dy;y++)
		for(x=0;x<map_dx;x++)
			if (mMap[x+y*map_dx])
			{
				std::vector<unsigned char> &data = mDistMap[x+y*map_dx];
				assert(!data.empty());
				fwrite(&data.front(), 1, data.size(), f);
			}
	fclose(f);
}


POS DISTCACHE::GetNextTowards(const POS &p0, const POS &p1)
{
	if (p0==p1) return POS(0,0);
	if (!mMap[p0.x+p0.y*map_dx] || !mMap[p1.x+p1.y*map_dx]) return POS(0,0);
	int min_dist=0xFF;
	int min_air_dist = 0;
	POS ret;
	for(int dx=-1;dx<=1;dx++)
		for(int dy=-1;dy<=1;dy++)
	{
		if (dx==0 && dy==0) continue;
		POS p2(p0.x+dx, p0.y+dy);
		if (!mMap[p2.x + p2.y*map_dx]) continue;
		int d=GetDist(p2, p1);
		int ad = p1.DistSquare(p2);
		assert(d>=0);
		if (d<min_dist)
		{
			min_dist = d;
			min_air_dist = ad;
			ret = p2;
		} else if (d==min_dist)
		{
			if (ad < min_air_dist)
			{
				ret = p2;
				min_air_dist = ad;
			}
		}
	}
	assert(ret.IsValid());
	return ret;
}

void DISTCACHE::CreateFromParser(PARSER &Parser)
{
	map_dx = Parser.w;
	map_dy = Parser.h;
	mMap.resize(map_dx*map_dy);
	mDistMap.clear();
	mDistMap.resize(map_dx*map_dy);
	std::vector<POS> open_list;
	int x, y;
	for(y=0;y<map_dy;y++)
		for(x=0;x<map_dx;x++)
			mMap[x+y*map_dx] = Parser.GetAt(POS(x, y))==PARSER::WALL?0:1;
	for(y=0;y<map_dy;y++)
		for(x=0;x<map_dx;x++)
			if (mMap[x+y*map_dx])
			{
				std::vector<unsigned char> &data = mDistMap[x+y*map_dx];
				data.resize(map_dx*map_dy);
				memset(&data.front(), 0xFF, data.size());
				data[x+y*map_dx]=0;
				open_list.clear();
				open_list.push_back(POS(x,y));
				unsigned int idx;
				for(idx=0;idx<open_list.size();idx++)
				{
					POS from = open_list[idx];
					int dist = data[from.x+from.y*map_dx];
					assert(dist!=0xFF);
					for(int dx=-1;dx<=1;dx++)
						for(int dy=-1;dy<=1;dy++)
					{
						if (dx==0 && dy==0) continue;
						POS p1(from.x+dx, from.y+dy);
						if (!mMap[p1.x + p1.y*map_dx]) continue;
						if (data[p1.x+p1.y*map_dx]==0xFF)
						{
							data[p1.x+p1.y*map_dx]=dist+1;
							open_list.push_back(p1);
						}
					}
				}
			}
}

int DISTCACHE::GetDist(const POS &p0, const POS &p1)
{
	if (!mMap[p0.x+p0.y*map_dx] || !mMap[p1.x+p1.y*map_dx]) return -1;
	return mDistMap[p1.x+p1.y*map_dx][p0.x+p0.y*map_dx];
}
