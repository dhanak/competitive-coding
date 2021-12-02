
#include "redirector.h"

Redirector::Redirector(std::vector<std::string> args)
{
	if (!args.empty())
	{
		fin.open(args[0]);
		rdcin = std::cin.rdbuf(fin.rdbuf());
		if (args.size() > 1)
		{
			fout.open(args[1]);
			rdcout = std::cout.rdbuf(fout.rdbuf());
		}
	}
}

Redirector::~Redirector()
{
	if (rdcin)
	{
		std::cin.rdbuf(rdcin);
		if (rdcout)
		{
			std::cout.rdbuf(rdcout);
		}
	}
}
