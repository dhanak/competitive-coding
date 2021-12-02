#pragma once
#include "parser.h"
#include "distcache.h"

class CLIENT
{
public:
	PARSER mParser;	
	std::stringstream command_buffer;
	DISTCACHE mDistCache;
	CLIENT();
	virtual ~CLIENT();
	bool Init(); // connect
	std::string strIPAddress;
	std::string strPassword;
	std::string strPreferredOpponents;
	bool bNeedDebugLog;
	bool bReceivedFirstPing;
	bool LinkDead();
	void Run();

	std::string DebugResponse(std::vector<std::string> &text) { return HandleServerResponse(text); }

protected:
	void PrintNewMatch();
	std::string HandleServerResponse(std::vector<std::string> &ServerResponse); // setup parser, call Process, handle mUnitTarget
	void SendMessage( std::string aMessage );

	void Attack(int hero_id, int target_id);
	void Move(int hero_id, POS target_pos);
	virtual void Process() = 0;
	virtual void MatchEnd() {}; // reset any data here which is persistent between ticks
	virtual void ConnectionClosed();
	std::ofstream mDebugLog;

	std::string strLastLineRemaining;
	std::vector<std::string> LastServerResponse;
	void ReceivedData(char *str);
#ifdef WIN32
	SOCKET mConnectionSocket;
#else
	int mConnectionSocket;
#endif
};

CLIENT *CreateClient();
