#pragma once
#include "parser.h"
#include "distcache.h"

class CLIENT
{
public:
	struct PLAYER
	{
		int id;
		int match_wins;
		int elo_points;
		std::string name;
	};
	std::vector<PLAYER> Players;
	PARSER mParser;	
	std::stringstream command_buffer;
	DISTCACHE mDistCache;
	enum eUnitCommand {
		CMD_MOVE,
		CMD_ATTACK,
		CMD_SPAWN
	};
	struct CMD
	{
		eUnitCommand c = CMD_MOVE;
		POS pos = POS(0,0);
		int target_id = 0;

		CMD() {}
		CMD(eUnitCommand _c, POS _pos, int _target_id = 0) : c(_c), pos(_pos), target_id(_target_id) {}
		bool operator==(const CMD &o) const { return c == o.c && pos == o.pos && target_id == o.target_id; }
		bool operator!=(const CMD &o) const { return !(*this == o); }
	};
	std::map<int, CMD> mUnitTarget;

	void ParsePlayers(std::vector<std::string> &ServerResponse);

	CLIENT();
	virtual ~CLIENT();
	bool Init(); // connect
	std::string strIPAddress;
	bool bReceivedFirstPing;
	bool LinkDead();

	// Runs the client
	void Run();

	std::string DebugResponse(std::vector<std::string> &text) { return HandleServerResponse(text); }

protected:
	std::string HandleServerResponse(std::vector<std::string> &ServerResponse); // setup parser, call Process, handle mUnitTarget
	void SendMessage( std::string aMessage );

	virtual void Process() = 0;
	virtual void MatchEnd() {}; // reset any data here which is persistent between ticks
	virtual void ConnectionClosed();
	virtual std::string GetPassword() = 0;
	virtual std::string GetPreferredOpponents() = 0;
	virtual bool NeedDebugLog() = 0;
	std::ofstream mDebugLog;
#ifdef WIN32
	SOCKET mConnectionSocket;
#else
	int mConnectionSocket;
#endif
};

CLIENT *CreateClient();
