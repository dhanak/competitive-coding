#include "stdafx.h"
#include "parser.h"

PARSER::PARSER()
{
	tick = 0;
	versus[0] = versus[1] = 0;	
	match_result = PARSER::ONGOING;
	w=h=0;
}

void PARSER::ParseUnits(const std::vector<std::string> &ServerResponse, int &index, int count, std::vector<MAP_OBJECT> &container)
{
	container.resize(count);
	for(int i=0;i<count;i++)
	{
		index++;
		MAP_OBJECT &ob = container[i];
		sscanf(ServerResponse[index].c_str(), "%d %d %d %d %d %d", &ob.id, &ob.side, &ob.pos.x, &ob.pos.y, &ob.hp, &ob.energy);
	}
}

void PARSER::Parse(const std::vector<std::string> &ServerResponse)
{
	tick = 0;
	match_result = PARSER::ONGOING;
	Arena.clear();
	Units.clear();	
	CreepTumors.clear();
	int i;
	for(i=0;i<(int)ServerResponse.size();i++)
	{
		if (ServerResponse[i].substr(0, 3)=="map")
		{
			sscanf(ServerResponse[i].c_str(), "map %d %d", &w, &h);
			Arena.resize(w*h);
			int r;
			for(r=0;r<h;r++)
			{
				const std::string &line = ServerResponse[i+r+1];
				for(int x=0;x<w;x++)
				{
					int c=line[x];
					eGroundType t = EMPTY;
					if (c=='#') t = WALL;
					else if (c=='+') t = CREEP;
					else if (c=='.') t = CREEP_CANDIDATE_FRIENDLY;
					else if (c==',') t = CREEP_CANDIDATE_ENEMY;
					else if (c==';') t = CREEP_CANDIDATE_BOTH;
					else if (c=='x') t = ENEMY_CREEP;
					else t = EMPTY;
					Arena[x + w*r] = t;
				}
			}
			i+=h;
		} else if (ServerResponse[i].substr(0, 4)=="tick")
		{
			tick = atoi(ServerResponse[i].substr(5).c_str());
		} else if (ServerResponse[i].substr(0, 6)=="versus")
		{
			std::string str_vs = ServerResponse[i].substr(7);
			if (sscanf(str_vs.c_str(), "%d %d", &versus[0], &versus[1])!=2)
			{
				versus[0]=versus[1]=0;
			}
		} else if (ServerResponse[i].substr(0, 10)=="hatcheries")
		{
			std::vector<MAP_OBJECT> Hatcheries;
			ParseUnits(ServerResponse, i, atoi(ServerResponse[i].substr(11).c_str()), Hatcheries);
			for(unsigned r=0;r<Hatcheries.size();r++)
			{
				if (Hatcheries[r].side==0) OwnHatchery = Hatcheries[r];
				else EnemyHatchery = Hatcheries[r];
			}
		} else if (ServerResponse[i].substr(0, 12) == "creep_tumors")
		{
			ParseUnits(ServerResponse, i, atoi(ServerResponse[i].substr(13).c_str()), CreepTumors);
		} else if (ServerResponse[i].substr(0, 5) == "units")
		{
			ParseUnits(ServerResponse, i, atoi(ServerResponse[i].substr(6).c_str()), Units);
		} else if (ServerResponse[i].substr(0, 8)=="finished")
		{
			std::string res = ServerResponse[i].substr(9);
			if (res=="victory")
				match_result = PARSER::VICTORY;
			else if (res=="draw")
				match_result = PARSER::DRAW;
			else if (res=="defeat")
				match_result = PARSER::DEFEAT;
		}
	}
}

std::ostream &operator<<(std::ostream& o, POS p)
{
	o << p.x << ' ' << p.y;
	return o;
}
