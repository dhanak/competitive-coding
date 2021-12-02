#pragma once

class DISTCACHE
{
public:
	int map_dx, map_dy;
	std::vector<unsigned char> mMap;
	std::vector<std::vector<unsigned char> > mDistMap;
	bool LoadFromFile(const char *filename);
	void SaveToFile(const char *filename);
	void CreateFromParser(PARSER &Parser);

	bool AdjecentPos(POS &pos, int dir);
	int GetDist(const POS &p0, const POS &p1);
	POS GetNextTowards(const POS &p0, const POS &p1);
};
