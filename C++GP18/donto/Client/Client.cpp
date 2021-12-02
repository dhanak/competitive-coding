#include "stdafx.h"
#include "Client.h"
#include <sstream>
#define SERVER_PORT 4242

CLIENT::CLIENT()
{
	bNeedDebugLog = true;
#ifdef WIN32
	mConnectionSocket = INVALID_SOCKET;
#else
	mConnectionSocket = -1;
#endif
	mDistCache.LoadFromFile("distcache.bin");
}

CLIENT::~CLIENT()
{
#ifdef WIN32
	if( mConnectionSocket != INVALID_SOCKET )
	{
		closesocket( mConnectionSocket );
	}
	WSACleanup();
#else
	if (mConnectionSocket!=-1)
	{
		close(mConnectionSocket);
	}
#endif
}

bool CLIENT::LinkDead()
{
#ifdef WIN32
	return mConnectionSocket==INVALID_SOCKET;
#else
	return mConnectionSocket == -1;
#endif
}

bool CLIENT::Init()
{
#ifdef WIN32
	static int wsa_startup_done = 0;
	if (!wsa_startup_done)
	{
		wsa_startup_done = 1;
		WSADATA WSAData;

		if( WSAStartup( 0x101,&WSAData ) != 0 )
		{
			std::cout << "Error: Cannot start windows sockets!" << std::endl;
			return false;
		}
	}
#endif
	unsigned long addr = inet_addr( strIPAddress.c_str() );
	sockaddr_in ServerSocketAddress;	
	ServerSocketAddress.sin_addr.s_addr = addr;
	ServerSocketAddress.sin_family = AF_INET;
	ServerSocketAddress.sin_port = htons( SERVER_PORT );
	mConnectionSocket = socket( AF_INET, SOCK_STREAM, 0 );
#ifdef WIN32
	if( mConnectionSocket == INVALID_SOCKET )
#else
	if (mConnectionSocket == -1 )
#endif
	{
		std::cout << "Error: Cannot open a socket!" << std::endl;
		return false;
	}
	if ( connect( mConnectionSocket,(struct sockaddr*)&ServerSocketAddress, sizeof( ServerSocketAddress ) ) )
	{
		std::cout << "Error: Cannot connect to " << strIPAddress << "!" << std::endl;
#ifdef WIN32
		closesocket( mConnectionSocket );
#else
		close( mConnectionSocket );
#endif
		return false;
	}
	SendMessage("login " + strPassword+" "+strPreferredOpponents);
	bReceivedFirstPing = false;
	return true;
}

void CLIENT::ConnectionClosed()
{
	std::cout<<"Connection closed"<<std::endl;
#ifdef WIN32
	mConnectionSocket = INVALID_SOCKET;
#else
	mConnectionSocket = -1;
#endif
}

void CLIENT::SendMessage( std::string aMessage )
{
	if (LinkDead()) return;
	if (aMessage.length()==0) return;
	if (aMessage[aMessage.length()-1]!='\n') aMessage+="\n";
	if (bNeedDebugLog && mDebugLog.is_open())
	{
		mDebugLog<<"Sent: "<<aMessage;
	}	
	int SentBytes = send( mConnectionSocket, aMessage.c_str(), int(aMessage.size()), 0 );
	if (SentBytes!=aMessage.size())
	{
#ifdef WIN32
		closesocket( mConnectionSocket );
#else
		close( mConnectionSocket );
#endif
		ConnectionClosed();
	}
}

void SavePacket(std::vector<std::string> &Lines, const char *filename)
{
	std::ofstream f;
	f.open(filename, std::ofstream::trunc);
	if (f.is_open())
	{
		for (std::vector<std::string>::const_iterator it = Lines.begin();
			it != Lines.end(); ++it)
		{
			f << *it << std::endl;
		}
		f.close();
	}
}

bool LoadPacket(const char *filename, std::vector<std::string> &Lines)
{
	std::ifstream debug_file(filename);
	if (debug_file.is_open())
	{
		std::string line;
		while (std::getline(debug_file, line))
		{
			Lines.push_back(line);
		}
		debug_file.close();
		return true;
	}
	return false;
}

void CLIENT::Run()
{
	if (bNeedDebugLog)
	{
		mDebugLog.open("debug.log", std::ofstream::out | std::ofstream::app
#ifdef WIN32
			, SH_DENYWR
#endif
		);
	}

	for (;;)
	{
		if (LinkDead())
		{
			LastServerResponse.clear();
			strLastLineRemaining = "";
			Init();
		}
		if (LinkDead())
		{
#ifdef WIN32
			Sleep(1000);
#else
			sleep(1);
#endif
			continue;
		}
		const size_t ReceiveBufferSize = 1 << 16;
		char ReceiveBuffer[ReceiveBufferSize + 1] = { 0 };

		int ReceivedBytesCount = recv(mConnectionSocket, ReceiveBuffer, ReceiveBufferSize, 0);

		if (ReceivedBytesCount == 0 || ReceivedBytesCount == -1)
		{
			// connection is closed or failed
#ifdef WIN32
			closesocket(mConnectionSocket);
#else
			close(mConnectionSocket);
#endif
			ConnectionClosed();
			continue;
		}
		ReceiveBuffer[ReceivedBytesCount] = 0;
		ReceivedData(ReceiveBuffer);
	}
}

void CLIENT::ReceivedData(char *str)
{
	char *line_start = str;
	for (;;)
	{
		char *s = strchr(line_start, '\n');
		if (!s)
		{
			strLastLineRemaining = line_start;
			break;
		}
		else
		{
			std::string alma = strLastLineRemaining;
			*s = 0;
			alma += line_start;
			line_start = s + 1;
			strLastLineRemaining = "";
			if (alma.empty())
			{
			}
			else if (alma == "fail")
			{
				std::cout << "Login failed :(" << std::endl;
			}
			else if (alma == "fail-reconnect")
			{
				std::cout << "Login failed, too many connections" << std::endl;
			}
			else if (alma == "ping")
			{
				SendMessage(std::string("pong"));
				if (!bReceivedFirstPing)
				{
					std::cout << "Login OK" << std::endl;
					bReceivedFirstPing = true;
				}
				else
				{
					time_t tt;
					time(&tt);
					struct tm *tm = localtime(&tt);
					char str[20];
					sprintf(str, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
					std::cout << "PING " << str << std::endl;
				}
			}
			else if (alma[0] == 'w' && alma.substr(0, 7) == "warning")
			{
				std::cout << "WARNING " << alma.substr(8) << std::endl;
			}
			else
			{
				LastServerResponse.push_back(alma);
				if (alma == ".")
				{
					if (LastServerResponse.front().substr(0, 5) == "teams")
					{
						mParser.ParseTeams(LastServerResponse);
						SavePacket(LastServerResponse, "teams.txt");
					}
					else if (LastServerResponse.front().substr(0, 3) == "map")
					{
						mParser.ParseMap(LastServerResponse);
						SavePacket(LastServerResponse, "map.txt");
						if (mDistCache.mDistMap.empty())
						{
							mDistCache.CreateFromParser(mParser);
							mDistCache.SaveToFile("distcache.bin");
						}
					}
					else
					{
						if (bNeedDebugLog && mDebugLog.is_open())
						{
							for (std::vector<std::string>::const_iterator it = LastServerResponse.begin();
								it != LastServerResponse.end(); ++it)
								mDebugLog << *it << std::endl;
						}
						std::string strResponse = HandleServerResponse(LastServerResponse);
						if (!strResponse.empty())
						{
							SendMessage(strResponse);
						}
					}
					LastServerResponse.clear();
				}
			}
		}
	}
}

void CLIENT::PrintNewMatch()
{
	if (mParser.Heroes.size()!=10) return;
	if (mParser.tick == 1)
	{
		std::cout << "started match #" << mParser.match_id;
	}
	else
	{
		std::cout << "joined match #" << mParser.match_id << " at tick " << mParser.tick;
	}
	std::cout << ": ";
	if (mParser.match_type == PARSER::MELEE)
	{
		std::cout << std::endl;
		for (int i = 0; i<10; i++)
		{
			if (mParser.Heroes[i].controller_id == 0) std::cout << "You";
			else
			{
				TEAM_INFO *pTeam = mParser.GetTeamByID(mParser.Heroes[i].controller_id);
				if (pTeam == NULL) std::cout << "unknown";
				else std::cout << pTeam->name;
			}
			std::cout << std::endl;
			if (i == 4) std::cout << "---- versus ----" << std::endl;
		}
	}
	else
	{
		std::cout << "duel versus ";
		if (mParser.Heroes[5].controller_id==100)
		{
			std::cout<<"viewer";
		} else
		{
			TEAM_INFO *pTeam = mParser.GetTeamByID(mParser.Heroes[5].controller_id);
			if (pTeam == NULL) std::cout << "unknown";
			else std::cout << pTeam->name;
		}
		std::cout << std::endl;
	}
}

std::string CLIENT::HandleServerResponse(std::vector<std::string> &ServerResponse)
{
	int prev_match_id = mParser.match_id;
	mParser.Parse(ServerResponse);
	if (prev_match_id!=mParser.match_id)
	{
		PrintNewMatch();
	}
	std::stringstream ss;
	if (mParser.match_result==PARSER::ONGOING)
	{
		ss << "tick "<<mParser.tick<<"\n";
		Process();
		ss<<command_buffer.str();
		command_buffer.str(std::string());
		ss<<".";
	} else
	{
		MatchEnd();
		ss<<".";
	}
	return ss.str();
}

void CLIENT::Attack(int hero_id, int target_id)
{
	command_buffer << "attack " << hero_id << " " << target_id << "\n";
}

void CLIENT::Move(int hero_id, POS target_pos)
{
	command_buffer << "move " << hero_id << " " << target_pos.x << " " << target_pos.y << "\n";
}

int main(int argc, char* argv[])
{
	std::cout.sync_with_stdio(false);
	std::string server_address;
	std::string password, opponent;
	int need_debug_log = -1;
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			if (argv[i][1] == 'p') password = argv[i] + 2;
			else if (argv[i][1]=='o') opponent = argv[i] + 2;
			else if (argv[i][1]=='d') need_debug_log=atoi(argv[i]+2);
			else {
				std::cout << "invalid argument: " << argv[i] << std::endl;
			}
		} else
		{
			server_address = argv[i];
		}
	}
	if (server_address.empty())
	{
		//server_address = "172.22.22.48";
		server_address = "127.0.0.1";
		std::cout<<"using default server address: " + server_address <<std::endl;
	}
	CLIENT *pClient = CreateClient();
	if (!password.empty()) pClient->strPassword = password;
	if (!opponent.empty()) pClient->strPreferredOpponents = opponent;
	if (need_debug_log!=-1) pClient->bNeedDebugLog = need_debug_log!=0;
	/* for debugging:  */
	std::vector<std::string> test_state;
	if (LoadPacket("test.txt", test_state))
	{
		std::vector<std::string> teams, map;
		if (LoadPacket("teams.txt", teams) &&
			LoadPacket("map.txt", map))
		{
			pClient->mParser.ParseTeams(teams);
			pClient->mParser.ParseMap(map);

			std::string resp = pClient->DebugResponse(test_state);
			std::cout<<"response: "<<resp <<std::endl;
		}
	}
	/**/

	pClient->strIPAddress = server_address;
	if (!pClient->Init())
	{
		std::cout<<"Connection failed"<<std::endl;
	} else
	{
		pClient->Run();
	}
	delete pClient;
	return 0;
}


