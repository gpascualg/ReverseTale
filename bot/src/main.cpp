// Streaming23_02_16.cpp : Defines the exported functions for the DLL application.
//

#include "xHacking.h"
#include "Utilities/Utilities.h"
#include "Loader/Loader.h"
#include "Detour/Detour.h"

#include <Login/login.h>
#include <Game/game.h>

using namespace xHacking;

DWORD baseAddress = 0x68120C;
std::string sessionID;
Game::Session session;

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

	std::string decrypted;
	printf("\nSEND Is Login? %d\n", login);
	if (login == 0)
	{
		decrypted = Login::decryptLogin((char*)buf, len);
		session.reset();
	}
	else
	{
		decrypted = session.decryptPacket((char*)buf, len);

		if (session.id() == -1)
		{
			session.setID(sessionID);
		}
	}

	printf("Decrypted chars:\n");
	printf("%s", decrypted.c_str());

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

	std::string decrypted;
	
	printf("\nRECV Is Login? %d\n", login);
	if (login == 0)
	{
		decrypted = Login::decrytAnswer(buf, ret);
		std::vector<std::string> tokens = Game::tokenize(decrypted);
		sessionID = tokens[1];
	}
	else
	{
		decrypted = session.decryptRecv(buf, ret);
	}

	printf("Decrypted chars:\n %s", decrypted.c_str());

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
