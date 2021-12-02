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
	bool bReceivedFirstPing;
	bool LinkDead();
	void Run();

	std::string DebugResponse(std::vector<std::string> &text) { return HandleServerResponse(text); }

protected:
	void PrintNewMatch();
	std::string HandleServerResponse(std::vector<std::string> &ServerResponse); // setup parser, call Process, handle mUnitTarget
	void SendMessage( std::string aMessage );

public:
	void Attack(int hero_id, int target_id);
	void Move(int hero_id, POS target_pos);
protected:
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
