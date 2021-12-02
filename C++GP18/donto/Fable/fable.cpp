#include "stdafx.h"
#include "client.h"

using namespace std;
/*
todo:
+ melee ne tudjon rogton indulni, a pingre se tudnak valaszolni
+ csatlakozas utan timeout helyett state-et kuldjon inkabb, es tick timer reset
+ disconnectnel a melee-nel keressen masik controllert (vagy melee-s csapattarsat?)
+ parser lUnits
+ match type-ot at kene megis kuldeni
+ match id is kell, plusz console log uj meccsnek, +ki ellen
+ unit test
+ viewer bmp aktualizalasa (2017)
+ ures meccset ne folytassa, forfeit se kell akkor
+ base fix 11, 12 id
+ minion attack es move kozott nincs die
+ optimalizalt perm.out
+ pontozas? win=1, vagy base hp alapjan? hero level szamitson egyaltalan?
+ viewernek miert 100-as id-t kuld
+ viewer logikat egyszerusiteni leiras alapjan
+ start & rec melee showdown (//todo)
+ player nem mindig van, kell createplayerforteam(), ha nem konnektal valaki showdownban
+ RealMatch nem tiszta, mit jelent... 0 stake-es duel es stake-es melee?
+ viewer rogton kuldje szet a herokat, kulonben az all in ellen eselytelen
+ showdown record
+ server game replay
+ showdown fileban nincs szorzo mentve
+ showdown gen (gen helyett ne irjon felul semmit?) + uzenet, hogy franko
+ all in legyen a default client?
+ palyan atlos lepes miatt map.txt-t valtoztatni
+ console szineket rendbeszedni
+ viewer szin kek/piros legyen a szoveg is!
+ melee win-nel ir playert
+ respawn idot novelni? all in tul eros
+ melee modban header clipping
+ melee player lista kijelzes, serverben, joining lista jo vajon? inkabb ne?
+ headerben szurkiteni aki halott
+ respawn time ellenfel leveletol fuggjon
+ 1,1-ben healt tesztelni, game.html-be rakni -- kiszedve
+ render nelkul is megy a szerver? -- nem
+ game.html-be hogy 2 klienst allitsanak be
+ exclusie log
+ viewerben instant replay
+ distcache map utan init
+ viewer ha meg nem volt connectelve, akkor mar nem fog becsatlakozni a meccsbe 
+ response ido kijelzes
+ 2 client/player showdown?
+ showdown match/melee-ben letesztelni a reconnectet

nem lesz:
? distcache a parserbe?
? backupnak a 4v4-es perm
- samplet lecserelni, +uj tesztek
- reconnectnel meg mindig van timeout
- timeoutnal sokat is tud varni a szerver, kb 5 sec-et is tick 100+-nal
- timeout nem latszik, minden tickre kuld warning timeoutot
- timeoutos kliensekre ne varjon

low pri:
- viewer szetesik instant replaynel
- LAB kiiras rossz? font?
- server replay test

high pri:
*/

class FABLE : public CLIENT
{
public:
	//persistent:
	int version;
	//int id;
	//int opp_type;
	FABLE()// int _id = 0)
	{
		version = 0;// GetTickCount() % 4;
		//id = _id;
		strPassword = "test1#1";
		strPreferredOpponents = "test";
	}
	
	int GetHeroDamage()
	{
		return HERO_DAMAGE;
	}
	HERO_INFO* GetAdjecentTarget(int x, int y, bool bPush, bool &bWasSkip)
	{
		HERO_INFO* ret = NULL;
		int min_dist = 10000;
		int min_hp = 0;
		int hero_damage = GetHeroDamage();
		
		for (auto &p : mParser.Heroes)
		{
			if (p.side != 0 && p.hp>0)
			{
				int d_sq = p.pos.DistSquare(POS(x, y));
				if (d_sq <= HERO_RANGE_SQ && (d_sq<min_dist || (d_sq == min_dist && p.hp<min_hp)))
				{
					if (p.id>=35 && !bPush && p.hp>hero_damage) // minion not last-hitting
					{
						// skip
						bWasSkip = true;
					} else
					{
						min_dist = d_sq;
						min_hp = p.hp;
						//ret = p.id;
						ret = &p;
					}
				}
			}
		}
		return ret;
	}


	void MatchEnd()
	{
	}
	void MoveTowards(HERO_INFO &hero, POS &p)
	{
		POS pos = mDistCache.GetNextTowards(hero.pos, p);
		if (pos.IsValid())
		{
			Move(hero.id, pos);
		}
	}
	std::vector<HERO_INFO*> Enemies()
	{
		std::vector<HERO_INFO*> res;
		for (auto &h : mParser.Heroes)
		{
			if (h.side == 1 && h.hp > 0)
			{
				res.push_back(&h);
			}
		}
		return res;
	}
	std::vector<HERO_INFO*> InRange(POS pos, std::vector<HERO_INFO*> &v)
	{
		std::vector<HERO_INFO*> res;
		for (auto h : v)
		{
			if (h->pos.DistSquare(pos) <= HERO_RANGE_SQ)
			{
				res.push_back(h);
			}
		}
		return res;
	}

	HERO_INFO &BestTarget(std::vector<HERO_INFO*> &v)
	{
		int lowest_hp = v[0]->hp;
		HERO_INFO &res = *v[0];
		for (int i = 1; i < v.size(); i++)
		{
			if (v[i]->hp <= lowest_hp)
			{
				res = *v[i];
				lowest_hp = res.hp;
			}
		}
		return res;
	}

	HERO_INFO &Closest(POS pos, std::vector<HERO_INFO*> &v)
	{
		int min_dist = mDistCache.GetDist(pos, v[0]->pos);
		HERO_INFO &res = *v[0];
		for (int i = 1; i < v.size(); i++)
		{
			int d = mDistCache.GetDist(pos, v[i]->pos);
			if (d < min_dist)
			{
				res = *v[i];
				min_dist = d;
			}
		}
		return res;
	}

	struct HEATMAP {
		std::vector<int> heat;
		int dx, dy;
		void mark(int weight, POS p, HERO_INFO &enemy)
		{
			int power = enemy.hp;
			if (enemy.held_item == ITEM_INFO::ARMOR || enemy.held_item == ITEM_INFO::CROSSBOW) power = power * 7 / 5;
			heat[p.x + p.y*dx]+=power;
		}
		void setup(PARSER &mParser)
		{
			dx = mParser.w;
			dy = mParser.h;
			int range;
			for (range = 0; range*range <= HERO_RANGE_SQ; range++);
			range--;
			heat.resize(dx*dy);
			for (auto &h : mParser.Heroes)
			{
				if (h.hp > 0 && h.side == 1)
				{

					mark(100, h.pos, h);
				}
			}
		}
		int at(POS p)
		{
			if (p.x < 0 || p.y < 0 || p.x >= dx || p.y >= dy) return 0;
			return heat[p.x + p.y*dx];
		}

	};
	void Process()
	{
		// all in
		//if (version!=0) Process1(); else
		auto enemies = Enemies();
		for(auto &ctrl: mParser.Heroes)
		{
			if (ctrl.controller_id == 0)
			{
				HERO_INFO *pHero = &ctrl;
				if (pHero->hp==0) continue; // respawning
				mDebugLog << "marker " << pHero->pos.x << " " << pHero->pos.y << " hero: " << pHero->id << std::endl;
				bool bWasAttack = false;
				{
					auto v = InRange(pHero->pos, enemies);
					if (!v.empty())
					{
						auto &t = BestTarget(v);
						Attack(pHero->id, t.id);
						bWasAttack = true;
					}
				}
				// check flee -> heat map
				/*if (!bWasAttack)
				{
					auto pl = AdjacentPositions(pHero->pos);
					for (auto p : pl)
					{
						auto v = InRange(p, enemies);
						if (v.size()>
					}
				}*/
				
				if (!bWasAttack)
				{
					int min_dist = 1000;
					int best_tur = -1;
					int which_hero_attacks_turret[3] = { -1,-1,-1 };
					bool bSplitItitialAttack = false;
					if (bSplitItitialAttack)
					for (size_t t = 0; t < mParser.Turrets.size() && t<3; t++)
					{
						if (mParser.Turrets[t].side ==2)
						{
							int best_d = 1000;
							int best_h = -1;
							for (int h = 0; h < 5; h++)
							{
								int k;
								for (k = 0; k < t && which_hero_attacks_turret[k]!= mParser.Heroes[h].id; k++);
								if (k < t) continue;
								int d = mDistCache.GetDist(mParser.Heroes[h].pos, mParser.Turrets[t].pos);
								if (mParser.Heroes[h].hp > 0 && d < best_d)
								{
									best_d = d;
									best_h = h;
								}
							}
							if (best_h != -1) which_hero_attacks_turret[t] = mParser.Heroes[best_h].id;
						}
					}

					for(size_t t=0;t<mParser.Turrets.size();t++)
					{
						if (mParser.Turrets[t].side!=0)
						{
							if (which_hero_attacks_turret[t] == pHero->id)
							{
								best_tur = t;
								break;
							}
							int d = mDistCache.GetDist(pHero->pos, mParser.Turrets[t].pos)+(mParser.Turrets[t].side==2?100:0);
							if (d<min_dist) { min_dist = d; best_tur = t; }
						}
					}
					if (best_tur!=-1)
					{
						POS pos = mParser.Turrets[best_tur].pos;
						if (pHero->pos.DistSquare(pos) <= HERO_RANGE_SQ)
						{
							auto v = InRange(pos, enemies); // enemy heroes also attacking turret
							if (!v.empty())
							{
								auto tt = InRange(pHero->pos, v);
								if (!tt.empty())
								{
									auto &t = BestTarget(v);
									Attack(pHero->id, t.id);
									bWasAttack = true;
								}
								else
								{
									auto &t = Closest(pHero->pos, v);
									MoveTowards(*pHero, t.pos);
									bWasAttack = true;
								}
							}
							else
							{
								// attack turret
								Attack(pHero->id, mParser.Turrets[best_tur].id);
							}
						}
						else
						{
							MoveTowards(*pHero, pos);
						}
					}
				}
			}
		}
	}
	/*
	void Process1()
	{		
		//if (1) return;
		std::vector<MAP_OBJECT* > lEnemies;
		for (auto &p : mParser.Units)
		{
			if (p.side == 1) lEnemies.push_back(&p);
		}
		std::map<int, int> LastTarget;
		for (auto &it : mParser.Attacks) LastTarget[it.attacker_id] = it.target_id;

		//for (auto &p : mParser.Heroes)
		for(auto &ctrl: mParser.Controllers)
		{
			if (ctrl.controller_id == 0)
			{
				MAP_OBJECT *pObj = mParser.GetUnitByID(ctrl.hero_id);
				if (pObj == NULL) continue;
				MAP_OBJECT &p = *pObj;
				
				bool bFlee = false;
				for (auto m : lEnemies)
				{
					int dist_sq = m->pos.DistSquare(p.pos);
					int range_sq = 
						m->t==HERO ? HERO_RANGE_SQ : 
						m->t == MINION ? MINION_RANGE_SQ :
						m->t == TURRET ? TURRET_RANGE_SQ : 0;
					if (dist_sq <= range_sq)
					{
						// check if they attack us
						if (LastTarget[m->id] == p.id)
						{
							// flee
							bFlee = true;
						}
						else
						{
							int nFodder = 0;
							for (auto &minion : mParser.Units)
							{
								if (minion.t == MINION && minion.side == 0 && m->pos.DistSquare(minion.pos) <= range_sq)
								{
									nFodder++;
								}
							}
							if (nFodder == 0) bFlee = true;
							//else if (nFodder == 1 && m->t==TURRET) bFlee = true;
						}
					}
				}
				int limit = 100;
				if (version==1) limit = 50;
				else if (version==2) limit = 200;
				bool bPush = false;
				if (mParser.level[0]>limit) bPush = true;
				if (bPush) bFlee = false;
				if (bFlee)
				{
					POS t = mDistCache.GetNextTowards(p.pos, POS(1, 1));
					if (t == p.pos)
					{
						// nada
					}
					else
					{
						//unit_had_command.insert(p.id);
						Move(p.id, t);
						//command_buffer << "move " << p.id << " " << t.x << " " << t.y << "\n";
					}
				}
				else
				{
					bool bWasSkip = false;
					MAP_OBJECT *adjacent = GetAdjecentTarget(p.pos.x, p.pos.y, bPush, bWasSkip);
					if (adjacent != NULL)
					{
						Attack(p.id, adjacent->id);//command_buffer << "attack " << p.id << " " << adjacent->id << "\n";
						adjacent->hp-=GetHeroDamage();
					}
					else if (bWasSkip)
					{
						 // wait
					} else
					{
						// move towards closest target
						int best_dist = 1000;
						MAP_OBJECT *best_enemy = NULL;
						int best_enemy_type = 0;
						for (auto m : lEnemies)
						{
							int graph_dist = mDistCache.GetDist(p.pos, m->pos);
							if (graph_dist>10 && !bPush)
							{
								int lane = mParser.w - m->pos.x < mParser.h-m->pos.y-5?2: // right
									mParser.w - m->pos.x > mParser.h-m->pos.y+5?0://left
									1;//mid
								int pref_lane = p.id==1 || p.id==2?0:
									p.id==3?1:2;
								if (version == 3) pref_lane = 0;
								if (lane!=pref_lane) graph_dist+=30;
							}
							if (graph_dist<best_dist)
							{
								best_enemy = m;
								best_dist = graph_dist;
								best_enemy_type = (int)m->t;
							}
						}
						if (best_enemy != NULL)
						{
							POS t = best_enemy->pos;
							if (best_enemy->id==20 && best_dist>10)
							{
								t = POS(34,5);
							}
							t = mDistCache.GetNextTowards(p.pos, t);
							if (t == p.pos || !t.IsValid())
							{
								// nada
							}
							else
							{
								bool bMoveIntoRange = true;
								if (best_enemy_type == 3)
								{
									if (best_enemy->pos.DistSquare(t) <= TURRET_RANGE_SQ)
									{
										int nFodders = 0;
										for (auto &minion : mParser.Units)
										{
											if (minion.t == MINION && minion.side == 0 && best_enemy->pos.DistSquare(minion.pos) <= TURRET_RANGE_SQ)
											{
												nFodders++;
											}
										}
										bMoveIntoRange = nFodders >= 2;
									}
								}
								if (bMoveIntoRange)
								{
									//unit_had_command.insert(p.id);
									//command_buffer << "move " << p.id << " " << t.x << " " << t.y << "\n";
									Move(p.id, t);
								}
							}
						}
					}
				}
			}
		}
	}*/

	void SetNonblocking()
	{
		u_long iMode = 1;
		ioctlsocket(mConnectionSocket, FIONBIO, &iMode);
	}
	int last_check_t = 0;
	bool bDisabled = false;
	bool bDeadlock = false;
	bool bTimeout = false;
	bool DoSendRecv()
	{
		int t = GetTickCount();
		if (t > last_check_t+1000)
		{
			last_check_t = t;
			if (rand() % 100 == 0)
			{
				int opp_type = rand() % 3;
				strPreferredOpponents = std::string(opp_type == 0 ? "test" : opp_type == 1 ? "duel" : "melee");
				int stress_test_type = rand() % 10;
				bDisabled = stress_test_type == 0;
				bDeadlock = stress_test_type == 1;
				bTimeout = stress_test_type == 2;
				if (!bDeadlock && !bTimeout)
				{
					closesocket(mConnectionSocket);
					mConnectionSocket = INVALID_SOCKET;
				}
			}
		}
		if (bDisabled || bDeadlock) return true;

		if (LinkDead())
		{
			LastServerResponse.clear();
			strLastLineRemaining = "";
			Init();
			SetNonblocking();
		}
		if (LinkDead())
		{
			return false;
		}
		const size_t ReceiveBufferSize = 1 << 16;
		char ReceiveBuffer[ReceiveBufferSize + 1] = { 0 };

		int ReceivedBytesCount = recv(mConnectionSocket, ReceiveBuffer, ReceiveBufferSize, 0);
		if (ReceivedBytesCount == -1)
		{
			int nError = WSAGetLastError();
			if (nError == WSAEWOULDBLOCK)
			{
				return true;
			}
		}

		if (ReceivedBytesCount == 0 || ReceivedBytesCount == -1)
		{
			// connection is closed or failed
#ifdef WIN32
			closesocket(mConnectionSocket);
#else
			close(mConnectionSocket);
#endif
			ConnectionClosed();
			return false;
		}
		ReceiveBuffer[ReceivedBytesCount] = 0;
		if (!bTimeout) ReceivedData(ReceiveBuffer);
		return true;
	}
};
#ifdef INTERNAL_BOT
CLIENT *CreateClient_Fable() { return new FABLE(); }
#else
CLIENT *CreateClient() { return new FABLE(); }
CLIENT *CreateClient_()
{
	std::vector<FABLE *> lClients;
	for (int i = 0; i < 15; i++)
	{
		FABLE *c = new FABLE();
		int opp_type = rand()%3;
		c->strPreferredOpponents = std::string(opp_type == 0 ? "test" : opp_type == 1 ? "duel" : "melee");
		char tmp[10];
		int num_pl = 5;
		int id = i;
		sprintf(tmp, "test%d#%d", (id % num_pl) + 1, (id / num_pl) + 1);
		c->strPassword = tmp;
		c->strIPAddress = "127.0.0.1";
		c->version = i % 4;
		c->Init();
		c->SetNonblocking();
		lClients.push_back(c);
	}
	for (;;)
	{
		for (auto &c : lClients)
		{
			c->DoSendRecv();
		}
	}
	return NULL;
}
#endif
