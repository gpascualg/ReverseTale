#include <iostream>
#include <thread>

#include "xHacking.h"
#include "Utilities/Utilities.h"
#include "Loader/Loader.h"
#include "Detour/Detour.h"

#include <Tools/utils.h>
#include <Game/packet.h>
#include <Cryptography/login.h>
#include <Cryptography/game.h>

using namespace xHacking;
using namespace Net;


bool running;
std::thread inputThread;
std::vector<std::string> filterSend;
std::vector<std::string> filterRecv;


DWORD baseAddress = 0x68120C;
std::string sessionID;
Utils::Game::Session session;

inline bool isLogin()
{
	DWORD pointer1 = *(DWORD*)(baseAddress);
	DWORD pointer2 = *(DWORD*)(pointer1);
	return *(BYTE*)(pointer2 + 0x31) == 0x00;
}

Detour<int, int, const char*, int, int>* sendDetour = NULL;
Detour<int, int, char*, int, int>* recvDetour = NULL;
int WINAPI nuestro_send(SOCKET s, const char *buf, int len, int flags)
{
	int ret = (*sendDetour)(s, buf, len, flags);
	
	__asm PUSHAD;
	__asm PUSHFD;

	bool login = isLogin();
	Packet* packet = nullptr;
	if (login)
	{
		packet = gFactory->make(PacketType::SERVER_LOGIN, &session, std::string(buf, len));
		session.reset();
	}
	else
	{
		packet = gFactory->make(PacketType::SERVER_GAME, &session, std::string(buf, len));
	}

	auto packets = packet->decrypt();
	for (auto packet : packets)
	{
		if (!packet.empty())
		{
			auto tokens = Utils::tokenize(packet);

			if (tokens.size() < 2)
			{
				continue;
			}

			//std::cout << tokens[1] << std::endl;
			if (std::find(filterSend.begin(), filterSend.end(), tokens[1]) != filterSend.end())
			{
				printf("\nSend:\n");
				std::cout << ">> " << packet << std::endl;
			}
		}
	}

	// Set session after decrypting
	if (!login && session.id() == -1)
	{
		auto tokens = Utils::tokenize(packets[0]);
		session.setID(sessionID);
		session.setAlive(Utils::decimal_str2hex(tokens[0]));
	}
	
	__asm POPFD;
	__asm POPAD;

	gFactory->recycle(packet);

	return ret;
}

int WINAPI nuestro_recv(SOCKET s, char *buf, int len, int flags)
{
	int ret = (*recvDetour)(s, buf, len, flags);

	__asm PUSHAD;
	__asm PUSHFD;

	bool login = isLogin();
	Packet* packet = nullptr;
	if (login)
	{
		packet = gFactory->make(PacketType::CLIENT_LOGIN, &session, std::string(buf, ret));
	}
	else
	{
		packet = gFactory->make(PacketType::CLIENT_GAME, &session, std::string(buf, ret));
	}

	auto packets = packet->decrypt();
	for (std::string packet : packets)
	{
		if (!packet.empty())
		{
			auto tokens = Utils::tokenize(packet);
			if (tokens.size() < 1)
			{
				continue;
			}

			if (std::find(filterRecv.begin(), filterRecv.end(), tokens[0]) != filterRecv.end())
			{
				printf("\nRecv:\n");
				std::cout << "<< " << packet << std::endl;
			}
		}
	}

	if (login)
	{
		auto tokens = Utils::tokenize(packets[0]);
		sessionID = tokens[1];
	}

	gFactory->recycle(packet);

	__asm POPFD;
	__asm POPAD;

	return ret;
}

void Hooks()
{
	sendDetour = new Detour<int, int, const char*, int, int>();
	sendDetour->Wait("WS2_32.dll", "send", (BYTE*)nuestro_send);

	recvDetour = new Detour<int, int, char*, int, int>();
	recvDetour->Wait("wsock32.dll", "recv", (BYTE*)nuestro_recv);
}

void processInput()
{
	std::cout << "Welcome to ReverseTale-Bot, you can filter packets by issuing:" << std::endl;
	std::cout << "\t<packetOpcode : Adds a recv for `packetOpcode`" << std::endl;
	std::cout << "\t<-packetOpcode : Removes a recv for `packetOpcode`" << std::endl;
	std::cout << "\t>packetOpcode : Adds a send for `packetOpcode`" << std::endl;
	std::cout << "\t>-packetOpcode : Removes a send for `packetOpcode`" << std::endl;

	while (running)
	{
		std::string input;
		std::cout << "Enter Filter: ";
		std::cin >> input;

		if (input.length() > 2)
		{
			std::vector<<std::string>* filter;
			bool recv = input[0] == '<';
			bool rem = input[1] == '-';

			if (recv)
			{
				filter = &filterRecv;
			}
			else
			{
				filter = &filterSend;
			}

			if (rem)
			{
				std::string filter = input.substr(2);
				vec.erase(std::remove(vec.begin(), vec.end(), filter), vec.end());
			}
			else
			{
				std::string filter = input.substr(1);
				vec.push_back(filter);
			}
		}
	}
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, DWORD reserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		// Use the xHacking::CreateConsole function
		CreateConsole();

		// Call our function
		Hooks();

		running = true;
		inputThread = std::thread(processInput);
	}
	else if (reason == DLL_PROCESS_DETTACH)
	{
		running = false;
		inputThread.join();
	}

	return true;
}
