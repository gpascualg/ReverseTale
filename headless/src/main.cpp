#include <iostream>
#include <string>
#include <sstream>
#include <time.h>

#include <Cryptography/login.h>
#include <Cryptography/game.h>
#include <Game/packet.h>
#include <Tools/client_socket.h>
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


int main(int argc, char** argv)
{
	srand((uint32_t)time(NULL));

	for (int i = 0; i < (((float)rand() / RAND_MAX) + 1) * 100; ++i)
	{
		Utils::seedRandom(0x989680);
	}

	fs::path dir = argv[0];
	INIReader reader(dir.remove_filename().string() + "/config/default.conf");
	if (reader.ParseError() < 0)
	{
		std::cout << "Couldn't load config file. Press [ENTER] to exit" << std::endl;
		getchar();
		return 1;
	}
	
	Utils::setNostalePath("P:\\Program Files (x86)\\GameforgeLive\\Games\\ESP_spa\\NosTale");
	if (reader.HasKey("MD5", "nostaleX"))
	{
		Utils::setMD5(reader.Get("MD5", "nostaleX", ""), reader.Get("MD5", "nostale", ""));
	}

	if (!reader.HasKey("Login", "User") || !reader.HasKey("Login", "Password"))
	{
		std::cout << "Incomplete configuration file. Press [ENTER] to exit" << std::endl;
		getchar();
		return 1;
	}

	// Save session, user and password
	std::string sessionID;
	std::string username = "blipi"; // reader.Get("Login", "User", "");
	std::string password = "123qwe"; // reader.Get("Login", "Password", "");

	// Connect to Login Server
	{
		using namespace Net;

		// Connect
		ClientSocket socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
		socket.setOption(SOL_SOCKET, SO_REUSEADDR, NULL, 0);
		socket.setOption(IPPROTO_TCP, TCP_NODELAY, NULL, 0);
		//socket.connect("79.110.84.75", 4005);
		socket.connect("127.0.0.1", 4005);

		// Login 
		{
			// Make login packet
			Packet* packet = gFactory->make(PacketType::CLIENT_LOGIN);
			*packet << Utils::Login::makePacket(username, password);

			// Send login
			packet->send(&socket);
		}

		// Parse response
		{
			// Receive login result
			Packet* packet = gFactory->make(PacketType::CLIENT_LOGIN, socket.recv());
			std::vector<NString> response = packet->decrypt();
			NString& data = response[0];

			// It should be NsTeST, otherwise fail
			if (data.tokens().str(0) != "NsTeST")
			{
				std::cout << "Login failed" << std::endl;
				getchar();
				return 1;
			}

			sessionID = data.tokens()[1];
			std::cout << "Session: " << sessionID << std::endl;
		}

		socket.close();
	}

	// Game Server
	{
		using namespace Net;

		// Connect
		ClientSocket socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
		socket.setOption(SOL_SOCKET, SO_REUSEADDR, NULL, 0);
		socket.setOption(IPPROTO_TCP, TCP_NODELAY, NULL, 0);
		//socket.connect("79.110.84.79", 4024);
		socket.connect("127.0.0.1", 4006);

		// Make a session & alive
		Utils::Game::Session session;
		session.setAlive(Utils::seedRandom(0x9680));

		// Send session & alive
		{
			Packet* packet = gFactory->make(PacketType::CLIENT_GAME, &session);
			*packet << sessionID;
			packet->send(&socket);
		}

		// Set session and wait
		session.setID(Utils::decimal_str2hex(sessionID));
		Sleep(1000);

		// Send username and password
		{
			// Make "user + pass" combination
			Packet* user = gFactory->make(PacketType::CLIENT_GAME, &session);
			Packet* pass = gFactory->make(PacketType::CLIENT_GAME, &session);

			// Add info, [alive + " " + data]
			*user << username;
			*pass << password;

			// Combine and send
			Packet* result = *user + *pass;
			result->send(&socket);
		}
		
		// Receive next packet
		while (1) {
			Packet* packet = gFactory->make(PacketType::CLIENT_GAME, &session, socket.recv());
			std::vector<NString> response = packet->decrypt();

			for (NString data : response)
			{
				std::cout << "Recv: " << data.get() << std::endl;
			}
		}
	}
	
	getchar();
	return 0;
}
