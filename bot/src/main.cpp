// Streaming23_02_16.cpp : Defines the exported functions for the DLL application.
//

#include "xHacking.h"
#include "Utilities/Utilities.h"
#include "Loader/Loader.h"
#include "Detour/Detour.h"

#include <Tools/utils.h>
#include <Cryptography/login.h>
#include <Cryptography/game.h>

using namespace xHacking;

DWORD baseAddress = 0x68120C;
std::string sessionID;
Utils::Game::Session session;

Detour<int, int, const char*, int, int>* sendDetour = NULL;
Detour<int, int, char*, int, int>* recvDetour = NULL;
int WINAPI nuestro_send(SOCKET s, const char *buf, int len, int flags)
{
	int ret = (*sendDetour)(s, buf, len, flags);
	
	__asm PUSHAD;
	__asm PUSHFD;

	DWORD pointer1 = *(DWORD*)(baseAddress);
	DWORD pointer2 = *(DWORD*)(pointer1);
	BYTE login = *(BYTE*)(pointer2 + 0x31);

	printf("Encrypted bytes: ");
	for (int i = 0; i < len; ++i) printf("%.2X ", (uint8_t)buf[i]);
	printf("\n");

	std::string packet(buf, len);
	printf("\nSEND Is Login? %d\n", login);
	if (login == 0)
	{
		Crypto::Server::Login::Decrypter::get()->parse(packet);
		session.reset();
	}
	else
	{
		Crypto::Server::Game::Decrypter::get()->parse(packet, &session);

		if (session.id() == -1)
		{
			session.setID(sessionID);
		}
	}

	printf("Decrypted chars:\n");
	printf("%s", packet.c_str());

	printf("\n\n");
	
	__asm POPFD;
	__asm POPAD;

	return ret;
}

int WINAPI nuestro_recv(SOCKET s, char *buf, int len, int flags)
{
	int ret = (*recvDetour)(s, buf, len, flags);

	__asm PUSHAD;
	__asm PUSHFD;

	DWORD pointer1 = *(DWORD*)(baseAddress);
	DWORD pointer2 = *(DWORD*)(pointer1);
	BYTE login = *(BYTE*)(pointer2 + 0x31);

	std::string packet(buf, ret);	
	printf("\nRECV Is Login? %d\n", login);
	if (login == 0)
	{
		Crypto::Client::Login::Decrypter::get()->parse(packet);
		std::vector<std::string> tokens = Utils::tokenize(packet);
		sessionID = tokens[1];
	}
	else
	{
		Crypto::Client::Game::Decrypter::get()->parse(packet);
	}

	printf("Decrypted chars:\n %s", packet.c_str());

	printf("\n\n");

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
