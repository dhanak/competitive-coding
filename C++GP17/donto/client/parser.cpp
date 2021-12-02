#include "stdafx.h"
#include "parser.h"
#include <algorithm>

PARSER::PARSER()
{
	tick = 0;
	level[0] = level[1] = 0;
	match_result = PARSER::ONGOING;
	w=h=0;
	match_id = 0;
}

void PARSER::ParseMap(const std::vector<std::string> &ServerResponse)
{
	sscanf(ServerResponse[0].c_str(), "map %d %d", &w, &h);
	Arena.resize(w*h);
	int r;
	for (r = 0; r<h; r++)
	{
		const std::string &line = ServerResponse[r + 1];
		for (int x = 0; x<w; x++)
		{
			int c = line[x];
			GROUND_TYPE t = EMPTY;
			if (c == '#') t = WALL;
			Arena[x + w*(h-1-r)] = t;
		}
	}
}

void PARSER::ParsePlayers(const std::vector<std::string> &ServerResponse)
{
	Players.clear();
	int count = atoi(ServerResponse[0].substr(8).c_str());
	Players.resize(count);
	int r;
	for (r = 0; r<count; r++)
	{
		std::stringstream ss;
		ss << ServerResponse[r + 1];
		ss >> Players[r].player_id;
		ss >> Players[r].match_wins;
		ss >> Players[r].elo_points;
		ss >> Players[r].qualified;
		char str[31];
		ss.getline(str, 30);
		str[30] = 0;
		Players[r].name = str+1;
	}
}


void PARSER::Parse(const std::vector<std::string> &ServerResponse)
{
	tick = 0;
	match_result = PARSER::ONGOING;
	Units.clear();
	Attacks.clear();
	Controllers.clear();

	int i;
	for(i=0;i<(int)ServerResponse.size();i++)
	{
		if (ServerResponse[i].empty()) continue;
		char c = ServerResponse[i][0];
		if (c=='t' && ServerResponse[i].substr(0, 4)=="tick")
		{
			tick = atoi(ServerResponse[i].substr(5).c_str());
		} else if (c=='m' && ServerResponse[i].substr(0, 5)=="match")
		{
			std::string param = ServerResponse[i].substr(6);
			char type_name[20];
			if (sscanf(param.c_str(), "%d %s", &match_id, type_name) == 2)
			{
				if (!strcmp(type_name, "melee")) match_type = PARSER::MELEE;
				else match_type = PARSER::DUEL;
			}
		} else if (c == 'c' && ServerResponse[i].substr(0, 11)=="controllers")
		{
			int count = atoi(ServerResponse[i].substr(12).c_str());
			for (int r = 0; r<count; r++)
			{
				CONTROLLER_INFO info;
				sscanf(ServerResponse[i + r + 1].c_str(), "%d %d", &info.hero_id, &info.controller_id);
				Controllers.push_back(info);
			}
		} else if (c == 'l' && ServerResponse[i].substr(0, 5) == "level")
		{
			std::string param = ServerResponse[i].substr(6);
			if (sscanf(param.c_str(), "%d %d", &level[0], &level[1]) != 2)
			{
				level[0] = level[1] = 0;
			}
		} else if (c == 'a' && ServerResponse[i].substr(0, 7) == "attacks")
		{
			int count = atoi(ServerResponse[i].substr(8).c_str());
			for (int r = 0; r<count; r++)
			{
				ATTACK_INFO info;
				sscanf(ServerResponse[i + r + 1].c_str(), "%d %d %d %d %d %d", &info.attacker_id, &info.attacker_pos.x, &info.attacker_pos.y, &info.target_id, &info.target_pos.x, &info.target_pos.y);
				Attacks.push_back(info);
			}
		}
		else if (c == 'r' && ServerResponse[i].substr(0, 8) == "respawns")
		{
			int count = atoi(ServerResponse[i].substr(9).c_str());
			for (int r = 0; r<count; r++)
			{
				RESPAWN_INFO info;
				sscanf(ServerResponse[i + r + 1].c_str(), "%d %d %d", &info.hero_id, &info.side, &info.tick);
				Respawns.push_back(info);
			}
		} else if (c == 'u' && ServerResponse[i].substr(0, 5)=="units")
		{
			int count = atoi(ServerResponse[i].substr(6).c_str());
			for (int r = 0; r<count; r++)
			{
				MAP_OBJECT ob;
				char type_name[20];
				sscanf(ServerResponse[i+r+1].c_str(), "%s %d %d %d %d %d", type_name, &ob.id, &ob.side, &ob.hp, &ob.pos.x, &ob.pos.y);
				if (!strcmp(type_name, "hero"))
				{
					ob.t = HERO;
				} else if (!strcmp(type_name, "minion"))
				{
					ob.t = MINION;
				} else if (!strcmp(type_name, "turret"))
				{
					ob.t = TURRET;
				}
				else if (!strcmp(type_name, "base"))
				{
					ob.t = BASE;
				}
				Units.push_back(ob);
			}
		} else if (c == 'f' && ServerResponse[i].substr(0, 8)=="finished")
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

MAP_OBJECT *PARSER::GetUnitByID(int id)
{
	for (std::vector<MAP_OBJECT>::iterator it=Units.begin();it!=Units.end();++it) { if (it->id == id) return &*it; }
	return NULL;
}

PLAYER_INFO *PARSER::GetPlayerByID(int player_id)
{
	for (std::vector<PLAYER_INFO>::iterator it=Players.begin();it!=Players.end();++it) { if (it->player_id == player_id) return &*it; }
	return NULL;
}
