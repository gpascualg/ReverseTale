// Streaming23_02_16.cpp : Defines the exported functions for the DLL application.
//

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

	printf("\nSend:\n");
	auto packets = packet->decrypt();
	for (std::string data : packets)
	{
		std::cout << ">> " << data << std::endl;
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

	printf("\nRecv:\n");
	auto packets = packet->decrypt();
	for (std::string data : packets)
	{
		std::cout << ">> " << data << std::endl;
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

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, DWORD reserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		// Use the xHacking::CreateConsole function
		CreateConsole();

		// Call our function
		Hooks();
	}

	return true;
}
