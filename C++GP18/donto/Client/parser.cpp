#include "stdafx.h"
#include "parser.h"

PARSER::PARSER()
{
	tick = 0;
	base_health[0] = base_health[1] = 0;
	match_result = PARSER::ONGOING;
	w=h=0;
	match_id = 0;
}

void PARSER::ParseMap(const std::vector<std::string> &ServerResponse)
{
	sscanf(ServerResponse[0].c_str(), "map %d %d", &w, &h);
	ArenaFree.resize(w*h);
	int r;
	for (r = 0; r<h; r++)
	{
		int y = (h - 1 - r);
		const std::string &line = ServerResponse[r + 1];
		for (int x = 0; x<w; x++)
		{
			int c = line[x];
			unsigned char nFree = 1;
			if (c == '#') nFree = 0;
			else if (c == ':') nFree = 2;
			ArenaFree[x + w*y] = nFree;
		}
	}
}

void PARSER::ParseTeams(const std::vector<std::string> &ServerResponse)
{
	Teams.clear();
	int count = atoi(ServerResponse[0].substr(6).c_str());
	Teams.resize(count);
	int r;
	for (r = 0; r<count; r++)
	{
		std::stringstream ss;
		ss << ServerResponse[r + 1];
		ss >> Teams[r].team_id;
		ss >> Teams[r].match_wins;
		ss >> Teams[r].elo_points;
		ss >> Teams[r].qualified;
		char str[31];
		ss.getline(str, 30);
		str[30] = 0;
		Teams[r].name = str+1;
	}
}

ITEM_INFO::eItemType ParseItemType(const char *str)
{
	if (!strcmp(str, "armor"))
	{
		return ITEM_INFO::ARMOR;
	}
	else if (!strcmp(str, "crossbow"))
	{
		return ITEM_INFO::CROSSBOW;
	}
	else if (!strcmp(str, "hammer"))
	{
		return ITEM_INFO::HAMMER;
	}
	else if (!strcmp(str, "potion"))
	{
		return ITEM_INFO::POTION;
	}
	else if (!strcmp(str, "bomb"))
	{
		return ITEM_INFO::BOMB;
	}
	else
	{
		return ITEM_INFO::NO_ITEM;
	}
}

ACTION_INFO::eActionType ParseActionType(const char *str)
{
	if (!strcmp(str, "attack"))
	{
		return ACTION_INFO::ATTACK;
	}
	if (!strcmp(str, "blast"))
	{
		return ACTION_INFO::BLAST;
	}
	if (!strcmp(str, "repair"))
	{
		return ACTION_INFO::REPAIR;
	}
	if (!strcmp(str, "heal"))
	{
		return ACTION_INFO::HEAL;
	}
	return ACTION_INFO::ATTACK;
}

void PARSER::Parse(const std::vector<std::string> &ServerResponse)
{
	tick = 0;
	match_result = PARSER::ONGOING;
	Heroes.clear();
	Items.clear();
	Actions.clear();
	Turrets.clear();

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
		} else if (c == 'b' && ServerResponse[i].substr(0, 5) == "bases")
		{
			std::string param = ServerResponse[i].substr(6);
			if (sscanf(param.c_str(), "%d %d", &base_health[0], &base_health[1]) != 2)
			{
				base_health[0] = base_health[1] = 0;
			}
		} else if (c == 'a' && ServerResponse[i].substr(0, 6) == "action")
		{
			ACTION_INFO info;
			char action_type_name[20];
			sscanf(ServerResponse[i].c_str()+7, "%s %d %d %d %d %d %d", action_type_name, &info.attacker_id, &info.attacker_pos.x, &info.attacker_pos.y, &info.target_id, &info.target_pos.x, &info.target_pos.y);
			info.action_type = ParseActionType(action_type_name);
			Actions.push_back(info);
		}
		else if (c == 't' && ServerResponse[i].substr(0, 6) == "turret")
		{
			TURRET_INFO info;
			sscanf(ServerResponse[i].c_str()+7, "%d %d %d %d %d", &info.id, &info.side, &info.hp, &info.pos.x, &info.pos.y);
			Turrets.push_back(info);
		} else if (c == 'h' && ServerResponse[i].substr(0, 4)=="hero")
		{
			HERO_INFO ob;
			char item_type_name[20];
			sscanf(ServerResponse[i].c_str()+5, "%d %d %d %d %d %s %d %d %d %d", &ob.id, &ob.side, &ob.hp, &ob.pos.x, &ob.pos.y, item_type_name, &ob.held_item_side, &ob.respawn_timer, &ob.controller_id, &ob.score);
			ob.held_item = ParseItemType(item_type_name);
			Heroes.push_back(ob);
		} else if (c == 'i' && ServerResponse[i].substr(0, 4)=="item")
		{
			ITEM_INFO item;
			char type_name[20];
			sscanf(ServerResponse[i].c_str()+5, "%s %d %d %d", type_name, &item.side, &item.pos.x, &item.pos.y);
			item.t = ParseItemType(type_name);
			Items.push_back(item);
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

HERO_INFO *PARSER::GetHeroByID(int id)
{
	for (auto &p : Heroes) { if (p.id == id) return &p; }
	return NULL;
}

TURRET_INFO *PARSER::GetTurretByID(int id)
{
	for (auto &p : Turrets) { if (p.id == id) return &p; }
	return NULL;
}

TEAM_INFO *PARSER::GetTeamByID(int team_id)
{
	for (auto &team : Teams) { if (team.team_id == team_id) return &team; }
	return NULL;
}

