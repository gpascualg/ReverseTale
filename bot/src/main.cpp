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


bool threadCreated = false;
bool running;
std::thread inputThread;
std::vector<std::string> filterSend;
std::vector<std::string> filterRecv;
bool showAsHex = false;


DWORD baseAddress = 0x68120C;
std::string sessionID;
Utils::Game::Session session;


void processInput();


inline bool isLogin()
{
	DWORD pointer1 = *(DWORD*)(baseAddress);
	DWORD pointer2 = *(DWORD*)(pointer1);
	return *(BYTE*)(pointer2 + 0x31) == 0x00;
}

struct special_compare : public std::unary_function<std::string, bool>
{
	explicit special_compare(const std::string &baseline) : baseline(baseline) {}
	bool operator() (const std::string &arg)
	{
		if (baseline.size() > arg.size())
		{
			return false;
		}

		for (size_t pos = 0; pos < baseline.size(); ++pos)
		{
			if (baseline[pos] == '*')
			{
				return true;
			}

			if (baseline[pos] != arg[pos])
			{
				return false;
			}
		}

		return true;
	}
	std::string baseline;
};

Detour<int, int, const char*, int, int>* sendDetour = NULL;
Detour<int, int, char*, int, int>* recvDetour = NULL;
int WINAPI nuestro_send(SOCKET s, const char *buf, int len, int flags)
{
	int ret = (*sendDetour)(s, buf, len, flags);
	
	__asm PUSHAD;
	__asm PUSHFD;


	if (!threadCreated)
	{
		threadCreated = true;
		std::cout << "Initializing thread" << std::endl;
		running = true;
		inputThread = std::thread(processInput);
		std::cout << "Done, joining" << std::endl;
	}


	bool login = isLogin();
	Packet* packet = nullptr;
	if (login)
	{
		packet = gFactory->make(PacketType::SERVER_LOGIN, &session, NString(buf, len));
		session.reset();
	}
	else
	{
		packet = gFactory->make(PacketType::SERVER_GAME, &session, NString(buf, len));
	}

	auto packets = packet->decrypt();
	for (auto packet : packets)
	{
		if (!packet.empty())
		{
			if (packet.tokens().length() < 2)
			{
				continue;
			}

			//std::cout << tokens[1] << std::endl;
			if (std::find(filterSend.begin(), filterSend.end(), special_compare(packet.tokens().str(1))) != filterSend.end())
			{
				printf("\nSend:\n");
				if (!showAsHex)
				{
					std::cout << "<< " << packet.get() << std::endl;
				}
				else
				{
					for (int i = 0; i < packet.length(); ++i)
					{
						printf("%.2X ", (uint8_t)packet[i]);
					}
					printf("\n");
				}
			}
		}
	}

	// Set session after decrypting
	if (!login && session.id() == -1)
	{
		session.setID(sessionID);
		session.setAlive(packets[0].tokens().from_int<uint32_t>(0));
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
		packet = gFactory->make(PacketType::CLIENT_LOGIN, &session, NString(buf, ret));
	}
	else
	{
		packet = gFactory->make(PacketType::CLIENT_GAME, &session, NString(buf, ret));
	}

	auto packets = packet->decrypt();
	for (NString packet : packets)
	{
		if (!packet.empty())
		{
			if (packet.tokens().length() < 1)
			{
				continue;
			}

			if (std::find(filterRecv.begin(), filterRecv.end(), special_compare(packet.tokens().str(0))) != filterRecv.end())
			{
				printf("\nRecv:\n");
				if (!showAsHex)
				{
					std::cout << "<< " << packet.get() << std::endl;
				}
				else
				{
					for (int i = 0; i < packet.length(); ++i)
					{
						printf("%.2X ", (uint8_t)packet[i]);
					}
					printf("\n");
				}
			}
		}
	}

	if (login)
	{
		sessionID = packets[0].tokens().str(1);
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
			if (input.compare("toggle_hex") == 0)
			{
				showAsHex = !showAsHex;
				continue;
			}

			std::vector<std::string>* filterVec;
			bool recv = input[0] == '<';
			bool rem = input[1] == '-';

			std::cout << "Recv: " << recv << " Rem: " << rem << " -- " << input << std::endl;

			if (recv)
			{
				filterVec = &filterRecv;
			}
			else
			{
				filterVec = &filterSend;
			}

			if (rem)
			{
				//std::string filter = input.substr(2);
				//filterVec->erase(std::remove(filterVec->begin(), filterVec->end(), filter), filterVec->end());
			}
			else
			{
				std::string filter = input.substr(1);
				filterVec->push_back(filter);
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
	}
	else if (reason == DLL_PROCESS_DETACH)
	{
		running = false;
		inputThread.join();
	}

	return true;
}
