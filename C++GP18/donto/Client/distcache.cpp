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
	int min_dist=GetDist(p0, p1);
	if (min_dist == 0xFF) return POS(0, 0); // no path deep into enemy base, out of range
	int count = 0;
	POS ret;
	min_dist*=2;
	for(int dx=-1;dx<=1;dx++)
		for(int dy=-1;dy<=1;dy++)
	{
		if (dx==0 && dy==0) continue;
		POS p2(p0.x+dx, p0.y+dy);
		if (mMap[p2.x + p2.y*map_dx]!=1) continue; // do not check adjacent enemy base ground
		int d=GetDist(p2, p1);
		assert(d>=0);
		d=d*2+(dx!=0 && dy!=0); // penalty for diagonal
		if (d<min_dist)
		{
			min_dist = d;
			count = 1;
			ret = p2;
		} else if (d==min_dist && count>0) // do not pick equal if same distance as p0
		{
			count++;
			if (((p0.x+p0.y)%count)==0) // deterministic pseudo-random tie breaker
			{
				ret = p2;
			}
		}
	}
	return ret;
}

void DISTCACHE::CreateFromParser(PARSER &Parser)
{
	map_dx = Parser.w;
	map_dy = Parser.h;
	mMap.resize(map_dx*map_dy);
	mDistMap.clear();
	mDistMap.resize(map_dx*map_dy);
	int box_size = 1; // for ranged access
	for (; box_size*box_size <= HERO_RANGE_SQ; box_size++);
	box_size--;
	std::vector<POS> open_list;
	int x, y;
	for (y = 0; y < map_dy; y++)
		for (x = 0; x < map_dx; x++)
		{
			bool bEnemyBaseGround = x + y > (map_dx + map_dy) / 2 && Parser.IsBaseGround(POS(x, y));
			mMap[x + y*map_dx] = bEnemyBaseGround ? 2 : Parser.IsFree(POS(x, y)) ? 1 : 0;
		}
	for (y = 0; y < map_dy; y++)
		for (x = 0; x < map_dx; x++)
			if (mMap[x + y*map_dx])
			{
				std::vector<unsigned char> &data = mDistMap[x+y*map_dx];
				data.resize(map_dx*map_dy);
				memset(&data.front(), 0xFF, data.size());
				open_list.clear();
				data[x + y*map_dx] = 0;
				open_list.push_back(POS(x, y));
				unsigned int idx;
				for(idx=0;idx<open_list.size();idx++)
				{
					POS from = open_list[idx];
					int dist = data[from.x+from.y*map_dx];
					assert(dist!=0xFF);
					bool bAccessByRange = false;
					if (mMap[from.x + from.y*map_dx] == 2)
					{
						// enemy base ground approachable by range only
						for (int dx = -box_size; dx <= box_size; dx++)
							for (int dy = -box_size; dy <= box_size; dy++)
								if (dx*dx+dy*dy<=HERO_RANGE_SQ)
							{
								int nx = from.x + dx;
								int ny = from.y + dy;
								if (ny >= 0 && ny < map_dy && nx >= 0 && nx < map_dx && mMap[nx + ny*map_dx] == 1)
								{
									bAccessByRange = true;
									if (data[nx + ny*map_dx] == 0xFF)
									{
										data[nx + ny*map_dx] = dist + 1;
										open_list.push_back(POS(nx, ny));
									}
								}
							}
					}
					if (!bAccessByRange)
					{
						for (int dx = -1; dx <= 1; dx++)
							for (int dy = -1; dy <= 1; dy++)
							{
								if (dx == 0 && dy == 0) continue;
								POS p1(from.x + dx, from.y + dy);
								if (!mMap[p1.x + p1.y*map_dx]) continue;
								if (mMap[from.x + from.y*map_dx] == 1 && mMap[p1.x + p1.y*map_dx] == 2) continue; // entering enemy base ground
								if (data[p1.x + p1.y*map_dx] == 0xFF)
								{
									data[p1.x + p1.y*map_dx] = dist + 1;
									open_list.push_back(p1);
								}
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
