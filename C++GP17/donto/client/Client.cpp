#include "stdafx.h"
#include "Client.h"
#include <sstream>
#define SERVER_PORT 4242

CLIENT::CLIENT()
{
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
	SendMessage("login " + GetPassword()+" "+GetPreferredOpponents());
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
	if (NeedDebugLog() && mDebugLog.is_open())
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
	if (NeedDebugLog())
	{
		mDebugLog.open("debug.log", std::ofstream::out | std::ofstream::app
#ifdef WIN32
		, SH_DENYWR
#endif
		);
	}
	
	std::string strLastLineRemaining;
	std::vector<std::string> LastServerResponse;
	int last_connect_try_time = GetTickCount();
	for(;;)
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
		const size_t ReceiveBufferSize = 1<<16;
		char ReceiveBuffer[ ReceiveBufferSize+1 ] = {0};

		int ReceivedBytesCount = recv( mConnectionSocket, ReceiveBuffer, ReceiveBufferSize, 0 );

		if( ReceivedBytesCount == 0 || ReceivedBytesCount == -1)
		{
			// connection is closed or failed
#ifdef WIN32
			closesocket( mConnectionSocket );
#else
			close( mConnectionSocket );
#endif
			ConnectionClosed();
			continue;
		}
		ReceiveBuffer[ReceivedBytesCount]=0;
		char *line_start = ReceiveBuffer;
		for(;;)
		{
			char *s = strchr(line_start, '\n');
			if (!s)
			{
				strLastLineRemaining = line_start;
				break;
			} else
			{
				std::string alma=strLastLineRemaining;
				*s=0;
				alma+=line_start;
				line_start = s+1;
				strLastLineRemaining = "";
				if (alma.empty())
				{
				} else
				if (alma=="fail")
				{
					std::cout<<"Login failed :("<<std::endl;
				} else
				if (alma == "fail-reconnect")
				{
					std::cout << "Login failed, too many connections" << std::endl;
				}
				else
				if (alma=="ping")
				{
					SendMessage(std::string("pong"));
					if (!bReceivedFirstPing)
					{
						std::cout<<"Login OK"<<std::endl;
						bReceivedFirstPing = true;
					} else
					{
						time_t tt;
						time(&tt);
						struct tm *tm = localtime(&tt);
						char str[20];
						sprintf(str, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
						std::cout<<"PING "<<str<<std::endl;
					}
				} else if (alma[0]=='w' && alma.substr(0, 7)=="warning")
				{
					std::cout << "WARNING " << alma.substr(8) << std::endl;
				} else
				{
					LastServerResponse.push_back(alma);
					if (alma==".")
					{
						if (LastServerResponse.front().substr(0, 7)=="players")
						{
							mParser.ParsePlayers(LastServerResponse);
							SavePacket(LastServerResponse, "players.txt");
						} else
						if (LastServerResponse.front().substr(0, 3)=="map")
						{
							mParser.ParseMap(LastServerResponse);
							SavePacket(LastServerResponse, "map.txt");
							if (mDistCache.mDistMap.empty())
							{
								mDistCache.CreateFromParser(mParser);
								mDistCache.SaveToFile("distcache.bin");
							}
						} else
						{
							if (NeedDebugLog() && mDebugLog.is_open())
							{
								for (std::vector<std::string>::const_iterator it = LastServerResponse.begin();
									it != LastServerResponse.end(); ++it)
									mDebugLog<<*it<<std::endl;
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
}

void CLIENT::PrintNewMatch()
{
	if (mParser.Controllers.size()!=10) return;
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
			if (mParser.Controllers[i].controller_id == 0) std::cout << "You";
			else
			{
				PLAYER_INFO *pPlayer = mParser.GetPlayerByID(mParser.Controllers[i].controller_id);
				if (pPlayer == NULL) std::cout << "unknown";
				else std::cout << pPlayer->name;
			}
			std::cout << std::endl;
			if (i == 4) std::cout << "---- versus ----" << std::endl;
		}
	}
	else
	{
		std::cout << "duel versus ";
		if (mParser.Controllers[5].controller_id==100)
		{
			std::cout<<"viewer";
		} else
		{
			PLAYER_INFO *pPlayer = mParser.GetPlayerByID(mParser.Controllers[5].controller_id);
			if (pPlayer == NULL) std::cout << "unknown";
			else std::cout << pPlayer->name;
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
	if (argc<2)
	{
		server_address = "10.112.0.36";
		std::cout<<"using default server address: " + server_address <<std::endl;
	} else
	{
		server_address = argv[1];
	}
	CLIENT *pClient = CreateClient();
	/* for debugging:  */
	std::vector<std::string> test_state;
	if (LoadPacket("test.txt", test_state))
	{
		std::vector<std::string> players, map;
		if (LoadPacket("players.txt", players) &&
			LoadPacket("map.txt", map))
		{
			pClient->mParser.ParsePlayers(players);
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


