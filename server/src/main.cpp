#include <iostream>
#include <string>
#include <sstream>
#include <time.h>

#include <Cryptography/login.h>
#include <Cryptography/game.h>
#include <Game/packet.h>
#include <Tools/server_socket.h>
#include <Tools/utils.h>

#include <INIReader.h>

#include <ReverseTale.h>
#if HAVE_EXPERIMENTAL_FS
	#include <experimental/filesystem>
	
	namespace fs = std::experimental::filesystem;
#else
	#include <Tools/filesystem.h>
#endif


#ifndef _WIN32
	#define Sleep sleep
#endif


class Client
{};


int main(int argc, char** argv)
{
	srand((uint32_t)time(NULL));

	for (int i = 0; i < (((float)rand() / RAND_MAX) + 1) * 100; ++i)
	{
		Utils::seedRandom(0x989680);
	}


	ServerSocket socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	socket.serve(4005);

	while (1)
	{
		AcceptedSocket* client = socket.accept<Client*>();
		if (client)
		{
			client->setOption(SOL_SOCKET, SO_REUSEADDR, NULL, 0);
			client->setOption(IPPROTO_TCP, TCP_NODELAY, NULL, 0);
			client->setUserData(new Client());
		}
	}
	
	getchar();
	return 0;
}
