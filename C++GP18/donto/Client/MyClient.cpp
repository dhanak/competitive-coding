#include "stdafx.h"
#include "Client.h"
#include "parser.h"

// sample

class MYCLIENT : public CLIENT
{
public:
	MYCLIENT();
protected:
	virtual std::string GetPassword() { return std::string("******"); }
	virtual std::string GetPreferredOpponents() { return std::string("test"); }
	virtual bool NeedDebugLog() { return true; }
	virtual void Process();
};

MYCLIENT::MYCLIENT()
{
}

void MYCLIENT::Process()
{
}


CLIENT *CreateClient()
{
	return new MYCLIENT();
}
