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

bool DISTCACHE::AdjecentPos(POS &p, int dir)
{
	POS p2=p.ShiftDir(dir);
	if (p2.x<0 || p2.x>=map_dx || p2.y<0 || p2.y>=map_dy || !mMap[p2.x+p2.y*map_dx])
	{
		return false;
	}
	p=p2;
	return true;
}

POS DISTCACHE::GetNextTowards(const POS &p0, const POS &p1)
{
	if (p0==p1) return POS(0,0);
	if (!mMap[p0.x+p0.y*map_dx] || !mMap[p1.x+p1.y*map_dx]) return POS(0,0);
	int min_dist=0xFF;
	int count = 0;
	POS ret;
	for(int dir=0;dir<4;dir++)
	{
		POS p2=p0;
		if (!AdjecentPos(p2, dir)) continue;
		int d=GetDist(p2, p1);
		assert(d>=0);
		if (d<min_dist)
		{
			min_dist = d;
			count = 1;
			ret = p2;
		} else if (d==min_dist)
		{
			count++;
			if (((p0.x+p0.y)%count)==0)
			{
				ret = p2;
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
					for(int dir=0;dir<4;dir++)
					{
						POS p1=from;
						if (!AdjecentPos(p1, dir)) continue;
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
